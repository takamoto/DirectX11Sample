#include "../DX11ThinWrapper.h"

namespace DX11ThinWrapper {
	namespace d3 {
		namespace {
			DXGI_FORMAT TextureFormat2DepthStencilViewFormat(DXGI_FORMAT format) {
				switch (format) {
				case DXGI_FORMAT_R8_TYPELESS: return DXGI_FORMAT_R8_UNORM;
				case DXGI_FORMAT_R16_TYPELESS: return DXGI_FORMAT_D16_UNORM;
				case DXGI_FORMAT_R32_TYPELESS: return DXGI_FORMAT_D32_FLOAT;
				case DXGI_FORMAT_R24G8_TYPELESS: return DXGI_FORMAT_D24_UNORM_S8_UINT;
				default: return format;
				}
			}

			D3D11_TEXTURE2D_DESC CreateDepthBufferDESC(IDXGISwapChain * swapChain) {
				// �X���b�v�`�F�[���̐ݒ���擾����
				DXGI_SWAP_CHAIN_DESC chainDesc;
				auto hr = swapChain->GetDesc(&chainDesc);
				if (FAILED(hr)) throw;

				D3D11_TEXTURE2D_DESC descDS;
				::ZeroMemory(&descDS, sizeof(D3D11_TEXTURE2D_DESC));
				descDS.Width = chainDesc.BufferDesc.Width;
				descDS.Height = chainDesc.BufferDesc.Height;
				descDS.MipLevels = 1;                             // �~�b�v�}�b�v���쐬���Ȃ�
				descDS.ArraySize = 1;
				descDS.Format = DXGI_FORMAT_R32_TYPELESS;
				descDS.SampleDesc.Count = chainDesc.SampleDesc.Count;
				descDS.SampleDesc.Quality = chainDesc.SampleDesc.Quality;
				descDS.Usage = D3D11_USAGE_DEFAULT;
				descDS.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
				descDS.CPUAccessFlags = 0;
				descDS.MiscFlags = 0;
				return descDS;
			}
		};

		std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(IDXGISwapChain * swapChain) {
			ID3D11RenderTargetView * view = nullptr;
			auto hr = AccessD3Device(swapChain)->CreateRenderTargetView(AccessBackBuffer(swapChain).get(), nullptr, &view);
			if (FAILED(hr)) throw;
			return std::shared_ptr<ID3D11RenderTargetView>(view, ReleaseIUnknown);
		}
		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(IDXGISwapChain * swapChain) {
			auto device = AccessD3Device(swapChain);
			auto descDS = CreateDepthBufferDESC(swapChain);
			auto depthBuffer = CreateTexture2D(device.get(), descDS);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			::ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			descDSV.Format = TextureFormat2DepthStencilViewFormat(descDS.Format);
			descDSV.ViewDimension = (descDS.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;

			ID3D11DepthStencilView * depthStencilView = nullptr;
			auto hr = device->CreateDepthStencilView(depthBuffer.get(), &descDSV, &depthStencilView);
			if (FAILED(hr)) throw;

			return std::shared_ptr<ID3D11DepthStencilView>(depthStencilView, ReleaseIUnknown);
		}


		std::shared_ptr<IDXGISwapChain> CreateSwapChain(
			DXGI_MODE_DESC * displayMode, HWND hWnd, ID3D11Device * device, bool useMultiSample
		) {
			DXGI_SWAP_CHAIN_DESC sd;
			::ZeroMemory(&sd, sizeof(sd));

			::CopyMemory(&(sd.BufferDesc), displayMode, sizeof(DXGI_MODE_DESC));
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
			sd.BufferCount = 1;
			sd.OutputWindow = hWnd;
			sd.Windowed = TRUE;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			sd.SampleDesc.Count = (useMultiSample) ? 4 : 1;
			sd.SampleDesc.Quality = 0;

			// �T���v�����O���̌���
			int numOfSample = (useMultiSample) ? D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT : 1;

			UINT quality = 0;
			IDXGISwapChain * swapChain = nullptr;
			for (int i = numOfSample; i >= 0; i--) {
				// �T�|�[�g����N�H���e�B���x���̍ő�l���擾����
				auto hr = device->CheckMultisampleQualityLevels(displayMode->Format, (UINT)i, &quality);
				if (FAILED(hr)) throw;

				// 0 �ȊO�̂Ƃ��t�H�[�}�b�g�ƃT���v�����O���̑g�ݍ��킹���T�|�[�g����
				// �I�����C���w���v�ł�CheckMultisampleQualityLevels�̖߂�l�� 0 �̂Ƃ��T�|�[�g����Ȃ��Ƃ�����Ă��邪
				// pNumQualityLevels �̖߂�l�� 0 ���Ŕ��f����B
				// Direct3D 10 �� �̃I�����C���w���v�ɂ͂���������Ă���̂ŁA�I�����C���w���v�̊ԈႢ�B
				if (quality != 0) {
					sd.SampleDesc.Count = i;
					sd.SampleDesc.Quality = quality - 1;

					hr = gi::AccessGIFactory(device)->CreateSwapChain(device, &sd, &swapChain);
					if (SUCCEEDED(hr)) break;
				}
			}
			if (swapChain == nullptr) throw;
			return std::shared_ptr<IDXGISwapChain>(swapChain, ReleaseIUnknown);
		}
	}
}