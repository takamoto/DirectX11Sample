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
	class FXAntiAliasing;

	class Canvas{
	private:
		// fxaa
		std::shared_ptr<ID3D11Texture2D> proxyBuffer;
		std::shared_ptr<ID3D11RenderTargetView> renderTargetView_proxyBuffer;
		std::shared_ptr<FXAntiAliasing> fxaa;

		// �r���[
		std::shared_ptr<ID3D11RenderTargetView> renderTargetView_swapChain;
		std::shared_ptr<ID3D11DepthStencilView> depthStencilView;

		// �`��ݒ�
		std::shared_ptr<ID3D11BlendState> blendState;
		std::shared_ptr<ID3D11RasterizerState> rasterizerState;
		std::shared_ptr<ID3D11DepthStencilState> depthStencilState;

		// �萔�o�b�t�@
		using CameraBuffer = primitive::ConstantBuffer<std::array<CameraParam, 4>>;
		using ViewportBuffer = primitive::ConstantBuffer<ViewportParam>;
		CameraBuffer cameraBuffer;
		ViewportBuffer viewportBuffer;

		// 
		std::shared_ptr<IDXGISwapChain> swapChain;
		std::shared_ptr<ID3D11DeviceContext> deviceContext;

		// 
		int numOfViewport = -1;
		bool enableFXAA = true;
	public:
		Canvas() = delete;
		Canvas(const std::shared_ptr<ID3D11DeviceContext> & deivceContext, const std::shared_ptr<IDXGISwapChain> & swapChain);

		void begin(
			const std::vector<std::shared_ptr<Viewport>> & viewports,
			const std::vector<std::shared_ptr<Camera>> & cameras
		);
		bool end();
		void draw(const Model * model, const std::bitset<4>& drawable = std::bitset<4>(15));

		// �ꎞ�I�ɗp��
		void setDrawable(const std::bitset<4> & drawable = std::bitset<4>(15));
	};
}