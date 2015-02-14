#pragma once
#include <memory>
#include <d3d11.h>
#include <d2d1_1.h>
#include <d3dcompiler.h>
#include <functional>
#include <vector>

namespace DX11ThinWrapper {
	inline void ReleaseIUnknown(IUnknown * p) { p->Release(); }

	namespace gi {
		// i番目のディスプレイを取得
		std::shared_ptr<IDXGIOutput> AccessDisplay(UINT i);

		// 利用可能な表示モードの取得
		void GetDisplayModes(DXGI_MODE_DESC* pModeDesc, UINT display_i, DXGI_FORMAT format, UINT * pNum = nullptr);

		// 利用可能な表示モードの数を取得
		UINT GetNumOfDisplayModes(UINT display_i, DXGI_FORMAT format);

		// 適切な解像度のディスプレイモードを取得
		DXGI_MODE_DESC GetOptDisplayMode(int width, int height, UINT display_i = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

		// IDXGIAdapter -> IDXGIFactory
		std::shared_ptr<IDXGIFactory> AccessGIFactory(IDXGIAdapter * adapter);
		// ID3D11Device -> IDXGIAdapter -> IDXGIFactory
		std::shared_ptr<IDXGIFactory> AccessGIFactory(ID3D11Device * device);

		// ID3D11Device -> IDXGIDevice1
		std::shared_ptr<IDXGIDevice1> AccessInterface(ID3D11Device * device);
		// ID3D11Device -> IDXGIDevice1 -> IDXGIAdapter
		std::shared_ptr<IDXGIAdapter> AccessAdapter(ID3D11Device * device);

		// ID3D11Texture2D -> IDXGISurface
		std::shared_ptr<IDXGISurface> AccessSurface(ID3D11Texture2D * tex);
	};

	namespace d3 {

		// Direct3Dの初期化
		std::shared_ptr<ID3D11Device> InitDirect3D();

		// シェーダファイルのコンパイル
		std::shared_ptr<ID3DBlob> CompileShader(TCHAR source[], CHAR entryPoint[], CHAR target[]);

		// ピクセルシェーダの生成
		std::shared_ptr<ID3D11PixelShader> CreatePixelShader(ID3D11Device* device, ID3DBlob * blob);
		// バーテックスシェーダの生成
		std::shared_ptr<ID3D11VertexShader> CreateVertexShader(ID3D11Device* device, ID3DBlob * blob);
		// ジオメトリシェーダの生成
		std::shared_ptr<ID3D11GeometryShader> CreateGeometryShader(ID3D11Device* device, ID3DBlob * blob);
		// 入力レイアウトの生成
		std::shared_ptr<ID3D11InputLayout> CreateInputLayout(
			ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numOfLayout, ID3DBlob * blob
		);

		// 頂点バッファの生成
		std::shared_ptr<ID3D11Buffer> CreateVertexBuffer(
			ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
		);
		// インデックスバッファの生成
		std::shared_ptr<ID3D11Buffer> CreateIndexBuffer(
			ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
		);
		// 定数バッファの生成
		std::shared_ptr<ID3D11Buffer> CreateConstantBuffer(
			ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
		);

		// テクスチャの設定を取得
		D3D11_TEXTURE2D_DESC GetTexture2DDescription(ID3D11Texture2D * texture);

		// 空のテクスチャリソースの生成
		std::shared_ptr<ID3D11Texture2D> CreateTexture2D(ID3D11Device * device, const D3D11_TEXTURE2D_DESC & descDS);

		// シェーダーリソースビューの生成
		std::shared_ptr<ID3D11ShaderResourceView> CreateShaderResourceView(
			ID3D11Device * device,
			ID3D11Resource * resource,
			const D3D11_SHADER_RESOURCE_VIEW_DESC * desc = NULL
		);

		// レンダーターゲットビューの生成
		std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(
			ID3D11Device * device,
			ID3D11Texture2D * texture,
			const D3D11_RENDER_TARGET_VIEW_DESC * desc = NULL
		);
		std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(
			IDXGISwapChain * swapChain,
			const D3D11_RENDER_TARGET_VIEW_DESC * desc = NULL
		);


		// 深度ステンシルビューの生成
		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(
			IDXGISwapChain * swapChain, const D3D11_TEXTURE2D_DESC & descDB, const D3D11_DEPTH_STENCIL_VIEW_DESC * descDSV = NULL
		);
		
		// 深度ステンシルステートの生成
		std::shared_ptr<ID3D11DepthStencilState> CreateDepthStencilState(
			ID3D11Device * device, const D3D11_DEPTH_STENCIL_DESC & desc
		);

		// スワップチェインの生成
		std::shared_ptr<IDXGISwapChain> CreateSwapChain(ID3D11Device * device, DXGI_SWAP_CHAIN_DESC sd);

		// マルチサンプリングで利用可能な品質レベルの数を取得
		UINT CheckMultisampleQualityLevels(ID3D11Device * device, DXGI_FORMAT format, UINT sampleCount);

		// スワップチェインの設定を取得
		DXGI_SWAP_CHAIN_DESC GetSwapChainDescription(IDXGISwapChain * swapChain);
		// スワップチェインのバックバッファへアクセス
		std::shared_ptr<ID3D11Texture2D> AccessBackBuffer(IDXGISwapChain * swapChain);
		// スワップチェインを生成したデバイスへアクセス
		std::shared_ptr<ID3D11Device> AccessD3Device(IDXGISwapChain * swapChain);

		// ID3D11Device -> ID3D11DeviceContext
		std::shared_ptr<ID3D11DeviceContext> AccessD3ImmediateContext(ID3D11Device * device);
		// IDXGISwapChain -> ID3D11Device -> ID3D11DeviceContext
		std::shared_ptr<ID3D11DeviceContext> AccessD3ImmediateContext(IDXGISwapChain * swapChain);

		// ID3D11DeviceContext -> ID3D11DepthStencilView
		std::shared_ptr<ID3D11DepthStencilView> AccessDepthStencilView(ID3D11DeviceContext * context);
		// ID3D11DeviceContext -> ID3D11RenderTargetView
		std::vector<std::shared_ptr<ID3D11RenderTargetView>> AccessRenderTargetViews(
			ID3D11DeviceContext * context, UINT numOfViews
		);

		// ID3D11View -> ID3D11Resource
		std::shared_ptr<ID3D11Resource> AccessResource(ID3D11View * view);


		// WICTextureLoader.h を用いたテクスチャファイル読み込み
		std::shared_ptr<ID3D11ShaderResourceView> CreateWICTextureFromFile(ID3D11Device * device, const wchar_t * path);

		// サンプラーステートの生成
		std::shared_ptr<ID3D11SamplerState> CreateSampler(ID3D11Device * device, const D3D11_SAMPLER_DESC & desc);

		// ラスタライザーステートの生成
		std::shared_ptr<ID3D11RasterizerState> CreateRasterizerState(ID3D11Device * device, const D3D11_RASTERIZER_DESC & desc);

		// ブレンドステートの生成
		std::shared_ptr<ID3D11BlendState> CreateBlendState(ID3D11Device * device, const D3D11_BLEND_DESC & desc);

		// ID3D11Resource -> ID3D11Device
		std::shared_ptr<ID3D11Device> AccessDevice(ID3D11Resource * resource);

		void mapping(ID3D11Resource * buffer, ID3D11DeviceContext * context, std::function<void(D3D11_MAPPED_SUBRESOURCE)> function);
	};


};