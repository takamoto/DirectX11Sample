#pragma once
#include <d3d11.h>

namespace dx11 {
	namespace default_setting{
		D3D11_RENDER_TARGET_VIEW_DESC RenderTargetView();
		D3D11_RASTERIZER_DESC Rasterizer(
			IDXGISwapChain * swapChain, D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK
		);

		// Ç‡Ç§è≠Çµó˚ÇÈ
		D3D11_SAMPLER_DESC SamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);

		D3D11_TEXTURE2D_DESC DepthBuffer(IDXGISwapChain * swapChain);
		D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilView(const D3D11_TEXTURE2D_DESC & descDB);
		D3D11_DEPTH_STENCIL_DESC DepthStencil();
		D3D11_BLEND_DESC Blend(const D3D11_RENDER_TARGET_BLEND_DESC & descRT);
		D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlend();
		D3D11_TEXTURE2D_DESC RenderTargetBuffer(IDXGISwapChain * swapChain);
	}
}