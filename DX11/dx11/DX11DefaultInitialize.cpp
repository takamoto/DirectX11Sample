#include "DX11DefaultInitialize.h"
#include "DX11ThinWrapper.h"
#include <stdexcept>

namespace dx11 {

	std::shared_ptr<IDXGISwapChain> CreateDefaultSwapChain(
		DXGI_MODE_DESC * displayMode, HWND hWnd, ID3D11Device * device, bool useMultiSample
	) {
		DXGI_SWAP_CHAIN_DESC sd;
		::ZeroMemory(&sd, sizeof(sd));

		::CopyMemory(&(sd.BufferDesc), displayMode, sizeof(DXGI_MODE_DESC)); // �f�B�X�v���C���[�h�̐ݒ���R�s�[
		sd.BufferCount = 1; // �o�b�N�E�o�b�t�@��
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT; // �o�b�N�o�b�t�@�̎g�p���@
		sd.OutputWindow = hWnd;
		sd.Windowed = TRUE; // ������Ԃ̓E�B���h�E���[�h��
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // ���[�h�������؂�ւ��\��

		if (!useMultiSample) {
			sd.SampleDesc.Count = 1;
			sd.SampleDesc.Quality = 0;
			return DX11ThinWrapper::d3::CreateSwapChain(device, sd);
		} else {
			for (int i = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i >= 0; i--) {
				auto quality = DX11ThinWrapper::d3::CheckMultisampleQualityLevels(device, displayMode->Format, i);
				if (quality == 0) continue;

				sd.SampleDesc.Count = i;
				sd.SampleDesc.Quality = quality - 1;
				return DX11ThinWrapper::d3::CreateSwapChain(device, sd);
			}
		}
	}

	void SetDefaultRenderTarget(IDXGISwapChain * swapChain) {
		auto targetView = DX11ThinWrapper::d3::CreateRenderTargetView(swapChain);
		ID3D11RenderTargetView * targetViews[] = { targetView.get() };
		DX11ThinWrapper::d3::AccessD3Context(swapChain)->OMSetRenderTargets(
			1, targetViews, DX11ThinWrapper::d3::CreateDepthStencilView(swapChain).get()
		);
	}
	void SetDefaultViewport(IDXGISwapChain * swapChain) {
		auto chainDesc = DX11ThinWrapper::d3::GetSwapChainDescription(swapChain);

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

		// �X���b�v�`�F�[���̃}���`�T���v�����O�̐ݒ�ɂ��킹��
		DXGI_SWAP_CHAIN_DESC swapDesc;
		swapChain->GetDesc(&swapDesc);
		rsState.MultisampleEnable = (swapDesc.SampleDesc.Count != 1) ? TRUE : FALSE;

		ID3D11RasterizerState* rasterState_raw = nullptr;
		HRESULT hr = DX11ThinWrapper::d3::AccessD3Device(swapChain)->CreateRasterizerState(&rsState, &rasterState_raw);
		if (FAILED(hr)) throw std::runtime_error("ID3D11RasterizerState�̐����Ɏ��s���܂���.");
		auto rasterState = std::shared_ptr<ID3D11RasterizerState>(rasterState_raw, DX11ThinWrapper::ReleaseIUnknown);
		DX11ThinWrapper::d3::AccessD3Context(swapChain)->RSSetState(rasterState_raw);
	}
};
