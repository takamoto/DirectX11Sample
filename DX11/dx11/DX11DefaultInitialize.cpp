#include "DX11DefaultInitialize.h"
#include "DX11ThinWrapper.h"

namespace dx11 {
	void SetDefaultRenderTarget(IDXGISwapChain * swapChain) {
		auto targetView = DX11ThinWrapper::d3::CreateRenderTargetView(swapChain);
		ID3D11RenderTargetView * targetViews[] = { targetView.get() };
		DX11ThinWrapper::d3::AccessD3Context(swapChain)->OMSetRenderTargets(
			1, targetViews, DX11ThinWrapper::d3::CreateDepthStencilView(swapChain).get()
		);
	}
	void SetDefaultViewport(IDXGISwapChain * swapChain) {
		// スワップチェーンの設定を取得する
		DXGI_SWAP_CHAIN_DESC chainDesc;
		auto hr = swapChain->GetDesc(&chainDesc);
		if (FAILED(hr)) throw;

		D3D11_VIEWPORT Viewport[1];
		Viewport[0].TopLeftX = 0;
		Viewport[0].TopLeftY = 0;
		Viewport[0].Width = (FLOAT)chainDesc.BufferDesc.Width;
		Viewport[0].Height = (FLOAT)chainDesc.BufferDesc.Height;
		Viewport[0].MinDepth = 0.0f;
		Viewport[0].MaxDepth = 1.0f;
		DX11ThinWrapper::d3::AccessD3Context(swapChain)->RSSetViewports(1, Viewport);
	}
	void SetDefaultRasterize(IDXGISwapChain * swapChain) {
		D3D11_RASTERIZER_DESC rsState;
		rsState.FillMode = D3D11_FILL_SOLID;
		rsState.CullMode = D3D11_CULL_NONE;
		rsState.FrontCounterClockwise = FALSE;
		rsState.DepthBias = 0;
		rsState.DepthBiasClamp = 0;
		rsState.SlopeScaledDepthBias = 0;
		rsState.DepthClipEnable = TRUE;
		rsState.ScissorEnable = FALSE;
		rsState.AntialiasedLineEnable = FALSE;

		// スワップチェーンのマルチサンプリングの設定にあわせる
		DXGI_SWAP_CHAIN_DESC swapDesc;
		swapChain->GetDesc(&swapDesc);
		rsState.MultisampleEnable = (swapDesc.SampleDesc.Count != 1) ? TRUE : FALSE;

		ID3D11RasterizerState* rasterState_raw = nullptr;
		HRESULT hr = DX11ThinWrapper::d3::AccessD3Device(swapChain)->CreateRasterizerState(&rsState, &rasterState_raw);
		if (FAILED(hr)) throw;
		auto rasterState = std::shared_ptr<ID3D11RasterizerState>(rasterState_raw, DX11ThinWrapper::ReleaseIUnknown);
		DX11ThinWrapper::d3::AccessD3Context(swapChain)->RSSetState(rasterState_raw);
	}
};
