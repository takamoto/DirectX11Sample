#pragma once
#include <memory>
#include <d3d11.h>
#include <vector>
#include <bitset>
#include <array>
#include <DirectXMath.h>
#include "DX11WrappedPrimitive.h"

class Model;
class Camera;
class Viewport;

namespace dx11 {
	struct CameraParam;
	struct ViewportParam;

	class Canvas{
	private:
		// ビュー
		std::shared_ptr<ID3D11RenderTargetView> renderTargetView;
		std::shared_ptr<ID3D11DepthStencilView> depthStencilView;

		// 描画設定
		std::shared_ptr<ID3D11BlendState> blendState;
		std::shared_ptr<ID3D11RasterizerState> rasterizerState;
		std::shared_ptr<ID3D11DepthStencilState> depthStencilState;

		// 定数バッファ
		using CameraBuffer = primitive::ConstantBuffer<std::array<CameraParam, 4>>;
		using ViewportBuffer = primitive::ConstantBuffer<ViewportParam>;
		CameraBuffer cameraBuffer;
		ViewportBuffer viewportBuffer;

		// 
		std::shared_ptr<IDXGISwapChain> swapChain;
		std::shared_ptr<ID3D11DeviceContext> deviceContext;

		// 
		int numOfViewport = -1;
	public:
		Canvas() = delete;
		Canvas(const std::shared_ptr<ID3D11DeviceContext> & deivceContext, const std::shared_ptr<IDXGISwapChain> & swapChain);

		void begin(
			const std::vector<std::shared_ptr<Viewport>> & viewports,
			const std::vector<std::shared_ptr<Camera>> & cameras
		);
		bool end();
		void draw(const Model * model, const std::bitset<4>& drawable = std::bitset<4>(15));

		// 一時的に用意
		void setDrawable(const std::bitset<4> & drawable = std::bitset<4>(15));
	};
}