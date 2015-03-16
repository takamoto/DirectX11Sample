#include "FXAA.h"

#include <tchar.h>
#include <DirectXMath.h>
#include "DirectXShaderLevelDefine.h"
#include "DX11ThinWrapper.h"
#include "DX11DefaultSetting.h"

namespace dx11{
	struct FXAAParam{
		DirectX::XMFLOAT4 fxaa;
	};

	FXAntiAliasing::FXAntiAliasing(IDXGISwapChain * swapChain, ID3D11Texture2D * sourceBuffer){
		auto device = DX11ThinWrapper::d3::AccessD3Device(swapChain);

		// source
		sourceShaderResourceView = DX11ThinWrapper::d3::CreateShaderResourceView(
			device.get(), sourceBuffer
		);

		// shader
		vertexShader = DX11ThinWrapper::d3::CreateVertexShader(
			device.get(), DX11ThinWrapper::d3::CompileShader(_T("FXAA.hlsl"), "FxaaVS", VERTEX_SHADER_PROFILE).get()
		);
		pixelShader = DX11ThinWrapper::d3::CreatePixelShader(
			device.get(), DX11ThinWrapper::d3::CompileShader(_T("FXAA.hlsl"), "FxaaPS", PIXEL_SHADER_PROFILE).get()
		);

		// constant buffer
		constantBuffer = FXAABuffer(device.get(), nullptr, D3D11_CPU_ACCESS_WRITE);

		// state
		rasterizerState = DX11ThinWrapper::d3::CreateRasterizerState(
			device.get(), default_setting::Rasterizer(swapChain)
		);

		samplerState = DX11ThinWrapper::d3::CreateSampler(
			device.get(),
			dx11::default_setting::SamplerState(D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT, D3D11_TEXTURE_ADDRESS_CLAMP)
		);

		D3D11_BLEND_DESC blendDesc;
		ZeroMemory(&blendDesc, sizeof(blendDesc));
		blendDesc.IndependentBlendEnable = FALSE;
		blendDesc.RenderTarget[0].BlendEnable = FALSE;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		blendState = DX11ThinWrapper::d3::CreateBlendState(device.get(), blendDesc);

		// multi sample ‘Î‰ž
		auto desc = DX11ThinWrapper::d3::GetTexture2DDescription(sourceBuffer);
		if (desc.SampleDesc.Count > 1){
			multiSampling = true;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.MipLevels = 1;
			copyBuffer = DX11ThinWrapper::d3::CreateTexture2D(device.get(), desc);
			copyBufferShaderResourceView = DX11ThinWrapper::d3::CreateShaderResourceView(device.get(), copyBuffer.get());
		}
	}

	void FXAntiAliasing::apply(
		ID3D11DeviceContext * deviceContext,
		ID3D11RenderTargetView * outputView,
		const D3D11_VIEWPORT & viewport
	){
		deviceContext->OMSetRenderTargets(1, &outputView, 0);
		
		// reset
		deviceContext->IASetInputLayout(nullptr);
		deviceContext->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		deviceContext->GSSetShader(nullptr, nullptr, 0);
		deviceContext->DSSetShader(nullptr, nullptr, 0);
		deviceContext->HSSetShader(nullptr, nullptr, 0);

		// shader
		deviceContext->VSSetShader(vertexShader.get(), nullptr, 0);
		deviceContext->PSSetShader(pixelShader.get(), nullptr, 0);

		// state
		deviceContext->OMSetBlendState(blendState.get(), 0, 0xffffffff);
		deviceContext->RSSetState(rasterizerState.get());

		auto samplerStates = samplerState.get();
		deviceContext->PSSetSamplers(3, 1, &samplerStates);

		// viewport
		deviceContext->RSSetViewports(1, &viewport);

		// constant buffer
		constantBuffer.mapping(deviceContext, [&](FXAABuffer::ResourceType resource){
			auto&& data = *resource.pData;
			data.fxaa = DirectX::XMFLOAT4(1.0f / viewport.Width, 1.0f / viewport.Height, 0.0f, 0.0f);
		});
		auto constantBuffers = constantBuffer.get();
		deviceContext->PSSetConstantBuffers(1, 1, &constantBuffers);
		deviceContext->VSSetConstantBuffers(1, 1, &constantBuffers);

		// for multisampling
		if (multiSampling){
			deviceContext->ResolveSubresource(copyBuffer.get(), 0, DX11ThinWrapper::d3::AccessResource(sourceShaderResourceView.get()).get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
			auto shaderResourceViews = copyBufferShaderResourceView.get();
			deviceContext->PSSetShaderResources(0, 1, &shaderResourceViews);
		}
		else {
			auto shaderResourceViews = sourceShaderResourceView.get();
			deviceContext->PSSetShaderResources(0, 1, &shaderResourceViews);
		}

		// draw
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		deviceContext->Draw(4, 0);
	}
}