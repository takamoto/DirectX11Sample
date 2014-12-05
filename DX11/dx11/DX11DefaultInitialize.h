#pragma once
struct IDXGISwapChain;
namespace dx11 {
	void SetDefaultRenderTarget(IDXGISwapChain * swapChain);
	void SetDefaultViewport(IDXGISwapChain * swapChain);
	void SetDefaultRasterize(IDXGISwapChain * swapChain);
};