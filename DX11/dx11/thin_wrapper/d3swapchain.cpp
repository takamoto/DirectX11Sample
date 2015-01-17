#include "../DX11ThinWrapper.h"
#include <stdexcept>

namespace DX11ThinWrapper {
	namespace d3 {
		DXGI_SWAP_CHAIN_DESC GetSwapChainDescription(IDXGISwapChain * swapChain){
			DXGI_SWAP_CHAIN_DESC chainDesc;
			auto hr = swapChain->GetDesc(&chainDesc);
			if (FAILED(hr)) throw std::runtime_error("�X���b�v�`�F�[���̐ݒ肪�擾�ł��܂���ł���.");
			return chainDesc;
		}
		
		std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(
			IDXGISwapChain * swapChain,
			const D3D11_RENDER_TARGET_VIEW_DESC * desc
		) {
			ID3D11RenderTargetView * view;
			auto hr = AccessD3Device(swapChain)->CreateRenderTargetView(AccessBackBuffer(swapChain).get(), desc, &view);
			if (FAILED(hr)) throw std::runtime_error("�����_�[�^�[�Q�b�g�r���[�̐����Ɏ��s���܂���.");
			return std::shared_ptr<ID3D11RenderTargetView>(view, ReleaseIUnknown);
		}

		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(
			IDXGISwapChain * swapChain, const D3D11_TEXTURE2D_DESC & descDB, const D3D11_DEPTH_STENCIL_VIEW_DESC * descDSV
		) {
			auto device = AccessD3Device(swapChain);
			auto depthBuffer = CreateTexture2D(device.get(), descDB);


			ID3D11DepthStencilView * depthStencilView = nullptr;
			auto hr = device->CreateDepthStencilView(depthBuffer.get(), descDSV, &depthStencilView);
			if (FAILED(hr)) throw std::runtime_error("�[�x�X�e���V���r���[�̐����Ɏ��s���܂���.");

			return std::shared_ptr<ID3D11DepthStencilView>(depthStencilView, ReleaseIUnknown);
		}

		std::shared_ptr<ID3D11DepthStencilState> CreateDepthStencilState(
			ID3D11Device * device, const D3D11_DEPTH_STENCIL_DESC & desc
		) {
			ID3D11DepthStencilState* depthStencil;
			auto hr = device->CreateDepthStencilState(&desc, &depthStencil);
			if (FAILED(hr)) throw std::runtime_error("�[�x�X�e���V���X�e�[�g�̐����Ɏ��s���܂����D");
			return std::shared_ptr<ID3D11DepthStencilState>(depthStencil, ReleaseIUnknown);
		}

		// DXGI_SWAP_CHAIN_DESC �� ��const����v�������
		std::shared_ptr<IDXGISwapChain> CreateSwapChain(ID3D11Device * device, DXGI_SWAP_CHAIN_DESC sd) {
			IDXGISwapChain * swapChain;
			auto hr = gi::AccessGIFactory(device)->CreateSwapChain(device, &sd, &swapChain);
			if (FAILED(hr)) throw std::runtime_error("�X���b�v�`�F�[���̐����Ɏ��s���܂���.");
			return std::shared_ptr<IDXGISwapChain>(swapChain, ReleaseIUnknown);
		}

		UINT CheckMultisampleQualityLevels(ID3D11Device * device, DXGI_FORMAT format, UINT sampleCount) {
			UINT quality = 0;
			auto hr = device->CheckMultisampleQualityLevels(format, sampleCount, &quality);
			if (FAILED(hr)) throw std::runtime_error("�i�����x���̐��̎擾�Ɏ��s���܂���.");
			return quality;
		}
	}
}