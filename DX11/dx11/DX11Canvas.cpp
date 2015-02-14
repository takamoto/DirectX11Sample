#include "DX11Canvas.h"

#include <array>
#include <tchar.h>
#include "DirectXShaderLevelDefine.h"
#include "DX11ThinWrapper.h"
#include "DX11GlobalDevice.h"
#include "DX11DefaultSetting.h"
#include "FXAA.h"
#include "../Camera.h"
#include "../Viewport.h"

static float ClearColor[4] = { 0.3f, 0.3f, 1.0f, 1.0f };

namespace dx11{
	struct CameraParam {
		DirectX::XMFLOAT4X4	matView;
		DirectX::XMFLOAT4X4	matProjection;
		DirectX::XMFLOAT4 eyePosition;

		CameraParam(const std::shared_ptr<Viewport> & viewport, const std::shared_ptr<Camera> & camera) {
			using namespace DirectX;
			XMStoreFloat4x4(
				&matView,
				XMMatrixTranspose(XMMatrixLookAtLH(
					XMLoadFloat3(&camera->getPosition()),
					XMLoadFloat3(&camera->getFocus()),
					XMLoadFloat3(&camera->getUpDirection())
				))
			);

			XMStoreFloat4x4(
				&matProjection,
				XMMatrixTranspose(XMMatrixPerspectiveFovLH(
					viewport->getFovAngleY(),
					viewport->getAspectRatio(),
					viewport->getNearZ(),
					viewport->getFarZ()
				))
			);

			auto pos = camera->getPosition();
			eyePosition = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1);
		}
	};

	struct ViewportParam{
		float drawable;
		float numOfViewport;
		DirectX::XMFLOAT2 space;
		ViewportParam(float drawable, float numOfViewport) : drawable(drawable), numOfViewport(numOfViewport){}
	};
}

namespace dx11{
	Canvas::Canvas(
		const std::shared_ptr<ID3D11DeviceContext> & deivceContext,
		const std::shared_ptr<IDXGISwapChain> & swapChain
	) : deviceContext(deivceContext), swapChain(swapChain){
		auto device = DX11ThinWrapper::d3::AccessD3Device(swapChain.get());
		
		// swapchain周り
		auto descDB = default_setting::DepthBuffer(swapChain.get());
		depthStencilView = DX11ThinWrapper::d3::CreateDepthStencilView(
			swapChain.get(), descDB, &default_setting::DepthStencilView(descDB)
		);
		renderTargetView_swapChain = DX11ThinWrapper::d3::CreateRenderTargetView(swapChain.get());

		// 描画設定の生成
		depthStencilState = DX11ThinWrapper::d3::CreateDepthStencilState(device.get(), default_setting::DepthStencil());
		blendState = DX11ThinWrapper::d3::CreateBlendState(
			device.get(), default_setting::Blend(default_setting::RenderTargetBlend())
		);
		rasterizerState = DX11ThinWrapper::d3::CreateRasterizerState(device.get(), default_setting::Rasterizer(swapChain.get()));

		// 定数バッファ生成
		cameraBuffer = CameraBuffer(device.get(), nullptr, D3D11_CPU_ACCESS_WRITE);
		viewportBuffer = ViewportBuffer(device.get(), nullptr, D3D11_CPU_ACCESS_WRITE);

		// fxaa
		proxyBuffer = DX11ThinWrapper::d3::CreateTexture2D(
			device.get(), dx11::default_setting::RenderTargetBuffer(swapChain.get())
		);
		renderTargetView_proxyBuffer = DX11ThinWrapper::d3::CreateRenderTargetView(device.get(), proxyBuffer.get());
		fxaa = std::make_shared<FXAntiAliasing>(swapChain.get(), proxyBuffer.get());
	}

	void Canvas::begin(
		const std::vector<std::shared_ptr<Viewport>> & viewports,
		const std::vector<std::shared_ptr<Camera>> & cameras
	) {
		// Canvas::begin(), Canvas::end()が正しいタイミングで呼ばれているかチェック
		assert(numOfViewport == -1);
		numOfViewport = viewports.size();

		// ビューポートとカメラの数が正しいかどうかチェック
		assert(viewports.size() <= cameras.size());
		assert(viewports.size() <= 4);

		// キャンバスのクリア
		auto & renderTargetView = (enableFXAA) ? renderTargetView_proxyBuffer : renderTargetView_swapChain;
		deviceContext->ClearRenderTargetView(renderTargetView.get(), ClearColor);
		deviceContext->ClearDepthStencilView(depthStencilView.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

		// 描画設定
		deviceContext->RSSetState(rasterizerState.get());
		deviceContext->OMSetDepthStencilState(depthStencilState.get(), 0);

		// キャンバスを描画先に設定
		auto renderTargetViews = renderTargetView.get();
		deviceContext->OMSetRenderTargets(1, &renderTargetViews, depthStencilView.get());
		deviceContext->OMSetBlendState(blendState.get(), 0, 0xffffffff);

		// 描画位置について
		cameraBuffer.mapping(deviceContext.get(), [&](CameraBuffer::ResourceType resource) {
			auto&& data = *resource.pData;
			for (int i = 0; i<viewports.size(); ++i) data[i] = CameraParam(viewports[i], cameras[i]);
		});
		auto cameraBuffers = cameraBuffer.get();
		deviceContext->GSSetConstantBuffers(1, 1, &cameraBuffers); // 現状slot決め打ち

		// 描画先について
		std::vector<D3D11_VIEWPORT>	d3vs;
		for (auto&& viewport : viewports) {
			d3vs.push_back({
				viewport->getScreenPosition().x,
				viewport->getScreenPosition().y,
				viewport->getScreenSize().x,
				viewport->getScreenSize().y,
				0.0f, 1.0f
			});
		}
		deviceContext->RSSetViewports(d3vs.size(), d3vs.data());
	}

	bool Canvas::end() {
		assert(numOfViewport>-1);
		numOfViewport = -1;
		if (enableFXAA){
			auto bufferDesc = DX11ThinWrapper::d3::GetSwapChainDescription(swapChain.get()).BufferDesc;
			fxaa->apply(
				deviceContext.get(),
				renderTargetView_swapChain.get(),
				{ 0, 0, bufferDesc.Width, bufferDesc.Height, 0.0f, 1.0f }
			);
		}
		return SUCCEEDED(swapChain->Present(0, 0));
	}

	void Canvas::setDrawable(const std::bitset<4>& drawable) {
		viewportBuffer.mapping(deviceContext.get(), [&](ViewportBuffer::ResourceType resource) {
			auto&& data = *resource.pData;
			data.drawable = static_cast<float>(drawable.to_ulong());
			data.numOfViewport = static_cast<float>(numOfViewport);
		});
		auto viewportBuffers = viewportBuffer.get();
		deviceContext->GSSetConstantBuffers(2, 1, &viewportBuffers); // 現状slot決め打ち
	}
}