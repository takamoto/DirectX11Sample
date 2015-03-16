#pragma once
#include <memory>
#include <d3d11.h>
#include "DX11WrappedPrimitive.h"

namespace dx11{
	struct FXAAParam;
	class FXAntiAliasing{
	public:
		FXAntiAliasing(IDXGISwapChain * swapChain, ID3D11Texture2D * sourceBuffer);

		void apply(
			ID3D11DeviceContext * deviceContext,
			ID3D11RenderTargetView * outputView,
			const D3D11_VIEWPORT & viewport
		);
	private:
		// shader
		std::shared_ptr<ID3D11VertexShader> vertexShader;
		std::shared_ptr<ID3D11PixelShader> pixelShader;

		// view
		std::shared_ptr<ID3D11ShaderResourceView> sourceShaderResourceView;
		
		// constant buffer
		using FXAABuffer = primitive::ConstantBuffer<FXAAParam>;
		FXAABuffer constantBuffer;

		// state
		std::shared_ptr<ID3D11BlendState> blendState;
		std::shared_ptr<ID3D11RasterizerState> rasterizerState;
		std::shared_ptr<ID3D11SamplerState> samplerState;

		// for multisampling
		std::shared_ptr<ID3D11Texture2D> copyBuffer;
		std::shared_ptr<ID3D11ShaderResourceView> copyBufferShaderResourceView;

		bool multiSampling = false;
	};
}