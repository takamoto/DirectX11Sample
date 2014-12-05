#pragma once
#include <memory>
#include <d3d11.h>
#include <d2d1_1.h>
#include <d3dcompiler.h>
#include <functional>

namespace DX11ThinWrapper {
	inline void ReleaseIUnknown(IUnknown * p) { p->Release(); }

	namespace gi {
		// i番目のディスプレイを取得
		std::shared_ptr<IDXGIOutput> AccessDisplay(UINT i);

		// 利用可能な表示モードの取得
		void GetDisplayModes(DXGI_MODE_DESC* pModeDesc, UINT * pNum = nullptr);

		// 利用可能な表示モードの数を取得
		UINT GetNumOfDisplayModes();

		// 適切な解像度のディスプレイモードを取得
		DXGI_MODE_DESC GetOptDisplayMode(int width, int height);

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

		// 空のテクスチャリソースの生成
		std::shared_ptr<ID3D11Texture2D> CreateTexture2D(ID3D11Device * device, D3D11_TEXTURE2D_DESC descDS);
		
		// レンダーターゲットビューの生成
		std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(IDXGISwapChain * swapChain);
		// 深度バッファビューの生成
		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(IDXGISwapChain * swapChain);


		// スワップチェインの生成
		std::shared_ptr<IDXGISwapChain> CreateSwapChain(
			DXGI_MODE_DESC * displayMode, HWND hWnd, ID3D11Device * device, bool useMultiSample
		);

		// スワップチェインのバックバッファへアクセス
		std::shared_ptr<ID3D11Texture2D> AccessBackBuffer(IDXGISwapChain * swapChain);
		// スワップチェインを生成したデバイスへアクセス
		std::shared_ptr<ID3D11Device> AccessD3Device(IDXGISwapChain * swapChain);

		// ID3D11Device -> AccessD3Context
		std::shared_ptr<ID3D11DeviceContext> AccessD3Context(ID3D11Device * device);
		// IDXGISwapChain -> ID3D11Device -> AccessD3Context
		std::shared_ptr<ID3D11DeviceContext> AccessD3Context(IDXGISwapChain * swapChain);

		void mapping(ID3D11Resource * buffer, ID3D11DeviceContext * context, std::function<void(D3D11_MAPPED_SUBRESOURCE)> function);
	};


};