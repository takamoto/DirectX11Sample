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
		// i�Ԗڂ̃f�B�X�v���C���擾
		std::shared_ptr<IDXGIOutput> AccessDisplay(UINT i);

		// ���p�\�ȕ\�����[�h�̎擾
		void GetDisplayModes(DXGI_MODE_DESC* pModeDesc, DXGI_FORMAT format, UINT * pNum = nullptr);

		// ���p�\�ȕ\�����[�h�̐����擾
		UINT GetNumOfDisplayModes(DXGI_FORMAT format);

		// �K�؂ȉ𑜓x�̃f�B�X�v���C���[�h���擾
		DXGI_MODE_DESC GetOptDisplayMode(int width, int height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);

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

		// Direct3D�̏�����
		std::shared_ptr<ID3D11Device> InitDirect3D();

		// �V�F�[�_�t�@�C���̃R���p�C��
		std::shared_ptr<ID3DBlob> CompileShader(TCHAR source[], CHAR entryPoint[], CHAR target[]);

		// �s�N�Z���V�F�[�_�̐���
		std::shared_ptr<ID3D11PixelShader> CreatePixelShader(ID3D11Device* device, ID3DBlob * blob);
		// �o�[�e�b�N�X�V�F�[�_�̐���
		std::shared_ptr<ID3D11VertexShader> CreateVertexShader(ID3D11Device* device, ID3DBlob * blob);
		// �W�I���g���V�F�[�_�̐���
		std::shared_ptr<ID3D11GeometryShader> CreateGeometryShader(ID3D11Device* device, ID3DBlob * blob);
		// ���̓��C�A�E�g�̐���
		std::shared_ptr<ID3D11InputLayout> CreateInputLayout(
			ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numOfLayout, ID3DBlob * blob
		);

		// ���_�o�b�t�@�̐���
		std::shared_ptr<ID3D11Buffer> CreateVertexBuffer(
			ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
		);
		// �C���f�b�N�X�o�b�t�@�̐���
		std::shared_ptr<ID3D11Buffer> CreateIndexBuffer(
			ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
		);
		// �萔�o�b�t�@�̐���
		std::shared_ptr<ID3D11Buffer> CreateConstantBuffer(
			ID3D11Device* device, void* data, size_t size, UINT cpuAccessFlag = 0
		);

		// ��̃e�N�X�`�����\�[�X�̐���
		std::shared_ptr<ID3D11Texture2D> CreateTexture2D(ID3D11Device * device, D3D11_TEXTURE2D_DESC descDS);
		
		// �����_�[�^�[�Q�b�g�r���[�̐���
		// ��⌈�ߑł��̕�������
		std::shared_ptr<ID3D11RenderTargetView> CreateRenderTargetView(IDXGISwapChain * swapChain);
		// �[�x�o�b�t�@�r���[�̐���
		// ��⌈�ߑł��̕�������
		std::shared_ptr<ID3D11DepthStencilView> CreateDepthStencilView(IDXGISwapChain * swapChain);


		// �X���b�v�`�F�C���̐���
		std::shared_ptr<IDXGISwapChain> CreateSwapChain(ID3D11Device * device, DXGI_SWAP_CHAIN_DESC sd);

		// �}���`�T���v�����O�ŗ��p�\�ȕi�����x���̐����擾
		UINT CheckMultisampleQualityLevels(ID3D11Device * device, DXGI_FORMAT format, UINT sampleCount);

		// �X���b�v�`�F�C���̐ݒ���擾
		DXGI_SWAP_CHAIN_DESC GetSwapChainDescription(IDXGISwapChain * swapChain);
		// �X���b�v�`�F�C���̃o�b�N�o�b�t�@�փA�N�Z�X
		std::shared_ptr<ID3D11Texture2D> AccessBackBuffer(IDXGISwapChain * swapChain);
		// �X���b�v�`�F�C���𐶐������f�o�C�X�փA�N�Z�X
		std::shared_ptr<ID3D11Device> AccessD3Device(IDXGISwapChain * swapChain);

		// ID3D11Device -> ID3D11DeviceContext
		std::shared_ptr<ID3D11DeviceContext> AccessD3Context(ID3D11Device * device);
		// IDXGISwapChain -> ID3D11Device -> ID3D11DeviceContext
		std::shared_ptr<ID3D11DeviceContext> AccessD3Context(IDXGISwapChain * swapChain);

		// ID3D11DeviceContext -> ID3D11DepthStencilView
		std::shared_ptr<ID3D11DepthStencilView> AccessDepthStencilView(ID3D11DeviceContext * context);
		// ID3D11DeviceContext -> ID3D11RenderTargetView
		std::vector<std::shared_ptr<ID3D11RenderTargetView>> AccessRenderTargetViews(
			ID3D11DeviceContext * context, UINT numOfViews
		);

		void mapping(ID3D11Resource * buffer, ID3D11DeviceContext * context, std::function<void(D3D11_MAPPED_SUBRESOURCE)> function);
	};


};