#include "DX11DefaultSetting.h"

#include "DX11ThinWrapper.h"

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
}

namespace dx11 {
	namespace default_setting{

		D3D11_RENDER_TARGET_BLEND_DESC RenderTargetBlend(){
			D3D11_RENDER_TARGET_BLEND_DESC desc;
			desc.BlendEnable = TRUE;
			desc.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.BlendOp = D3D11_BLEND_OP_ADD;
			desc.SrcBlendAlpha = D3D11_BLEND_ZERO;
			desc.DestBlendAlpha = D3D11_BLEND_ZERO;
			desc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			return desc;
		}

		D3D11_BLEND_DESC Blend(const D3D11_RENDER_TARGET_BLEND_DESC & descRT) {
			D3D11_BLEND_DESC desc;
			::ZeroMemory(&desc, sizeof(desc));
			desc.AlphaToCoverageEnable = FALSE;

			// TRUEの場合、マルチレンダーターゲットで各レンダーターゲットのブレンドステートの設定を個別に設定できる
			// FALSEの場合、0番目のみが使用される
			desc.IndependentBlendEnable = FALSE;

			::CopyMemory(&desc.RenderTarget, &descRT, sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
			return desc;
		}

		D3D11_RENDER_TARGET_VIEW_DESC RenderTargetView() {
			D3D11_RENDER_TARGET_VIEW_DESC desc;
			memset(&desc, 0, sizeof(desc));
			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

			return desc;
		}

		D3D11_RASTERIZER_DESC Rasterizer(IDXGISwapChain * swapChain) {
			D3D11_RASTERIZER_DESC desc;
			desc.FillMode = D3D11_FILL_SOLID;
			desc.CullMode = D3D11_CULL_NONE;
			desc.FrontCounterClockwise = FALSE;
			desc.DepthBias = 0;
			desc.DepthBiasClamp = 0;
			desc.SlopeScaledDepthBias = 0;
			desc.DepthClipEnable = TRUE;
			desc.ScissorEnable = FALSE;
			desc.AntialiasedLineEnable = FALSE;

			// スワップチェーンのマルチサンプリングの設定にあわせる
			auto swapDesc = DX11ThinWrapper::d3::GetSwapChainDescription(swapChain);
			desc.MultisampleEnable = (swapDesc.SampleDesc.Count != 1) ? TRUE : FALSE;

			return desc;
		}

		D3D11_SAMPLER_DESC SamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode) {
			D3D11_SAMPLER_DESC desc;
			ZeroMemory(&desc, sizeof(desc));

			desc.Filter = filter;
			desc.AddressU = addressMode;
			desc.AddressV = addressMode;
			desc.AddressW = addressMode;
			desc.MaxAnisotropy = 16;
			desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			desc.MaxLOD = D3D11_FLOAT32_MAX;

			return desc;
		}

		D3D11_TEXTURE2D_DESC DepthBuffer(IDXGISwapChain * swapChain) {
			auto chainDesc = DX11ThinWrapper::d3::GetSwapChainDescription(swapChain);
			D3D11_TEXTURE2D_DESC desc;
			::ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
			desc.Width = chainDesc.BufferDesc.Width;
			desc.Height = chainDesc.BufferDesc.Height;
			desc.MipLevels = 1;                             // ミップマップを作成しない
			desc.ArraySize = 1;
			desc.Format = DXGI_FORMAT_R32_TYPELESS;
			desc.SampleDesc.Count = chainDesc.SampleDesc.Count;
			desc.SampleDesc.Quality = chainDesc.SampleDesc.Quality;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			return desc;
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilView(const D3D11_TEXTURE2D_DESC & descDB) {
			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
			::ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
			descDSV.Format = TextureFormat2DepthStencilViewFormat(descDB.Format);
			descDSV.ViewDimension = (descDB.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;
			return descDSV;
		}


		D3D11_DEPTH_STENCIL_DESC DepthStencil(){
			D3D11_DEPTH_STENCIL_DESC desc;
			ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			desc.StencilEnable = FALSE;
			return desc;
		}
	}
}