#include <DirectXMath.h>
#include "./win/Window.h"
#include "./win/WindowProcedure.h"
#include "./dx11/DX11ThinWrapper.h"
#include "./dx11/DX11GlobalDevice.h"
#include "./dx11/DX11DefaultInitialize.h"
#include "./dx11/DirectXShaderLevelDefine.h"

int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE,
	LPTSTR,
	INT
) {
	::CoInitialize(NULL); // �Ȃ��Ă��������ǏI������O��f��
	
	// �E�B���h�E����
	auto modeDesc = DX11ThinWrapper::gi::GetOptDisplayMode(800, 600);
	win::Window window(
		TEXT("window"), TEXT("window"), {0, 0, modeDesc.Width, modeDesc.Height}, WS_OVERLAPPEDWINDOW, win::DefaultProcedure
	);

	MSG msg = {};

	{
		// ������
		dx11::DX11DeviceSharedGuard guard;
		auto device = dx11::AccessDX11Device();

		auto context = DX11ThinWrapper::d3::AccessD3Context(device);
		auto swapChain = DX11ThinWrapper::d3::CreateSwapChain(&modeDesc, window.getHWnd(), device, true);
		
		dx11::SetDefaultRenderTarget(swapChain.get());
		dx11::SetDefaultViewport(swapChain.get());

		// View Projection�p �萔�o�b�t�@
		// �萔�o�b�t�@�� float*4�̔{���̃T�C�Y�ł���K�v������
		// ���̑��͂܂肻���ȂƂ��� �F https://twitter.com/43x2/status/144821841977028608
		struct CameraParam {
			DirectX::XMFLOAT4X4	mtxView;
			DirectX::XMFLOAT4X4	mtxProj;
		};
		auto cameraBuffer = DX11ThinWrapper::d3::CreateConstantBuffer(
			device, nullptr, sizeof(CameraParam), D3D11_CPU_ACCESS_WRITE
		);

		// World�p �萔�o�b�t�@
		struct ModelParam {
			DirectX::XMFLOAT4X4	mtxWorld;
		};
		auto modelBuffer = DX11ThinWrapper::d3::CreateConstantBuffer(
			device, nullptr, sizeof(ModelParam), D3D11_CPU_ACCESS_WRITE
		);

		// ���_��񐶐�
		// �T�C�Y�����ɂ���ĈႤ�^�͗p���Ȃ��B������int32_t����p�����ق������S�B
		// �C���f�b�N�X�o�b�t�@�����l�B
		struct Vertex{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
		};
		Vertex vertices[] =
		{
			{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
			{ DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
		};
		auto vertexBuffer = DX11ThinWrapper::d3::CreateVertexBuffer(device, (void*)vertices, sizeof(vertices));
	
		// �V�F�[�_�[����
		D3D11_INPUT_ELEMENT_DESC layout_desc[] = {
			{ "IN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "IN_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		auto vertex_blob = DX11ThinWrapper::d3::CompileShader(_T("Sample.hlsl"), "RenderVS", VERTEX_SHADER_PROFILE);
		auto vertexShader = DX11ThinWrapper::d3::CreateVertexShader(device, vertex_blob.get());
		auto inputLayout = DX11ThinWrapper::d3::CreateInputLayout(device, layout_desc, _countof(layout_desc), vertex_blob.get());
		
		auto pixelShader = DX11ThinWrapper::d3::CreatePixelShader(
			device, DX11ThinWrapper::d3::CompileShader(_T("Sample.hlsl"), "RenderPS", PIXEL_SHADER_PROFILE).get()
		);


		// �^�[�Q�b�g�r���[
		ID3D11RenderTargetView * rv;
		ID3D11DepthStencilView * dv;
		context->OMGetRenderTargets(1, &rv, &dv);

		// �E�B���h�E�\��
		::ShowWindow(window.getHWnd(), SW_SHOW);
		::UpdateWindow(window.getHWnd());

		// ���C�����[�v
		do {
			if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			} else {
				// �o�b�N�o�b�t�@���N���A����B
				static float ClearColor[4] = { 0.3f, 0.3f, 1.0f, 1.0f };
				context->ClearRenderTargetView(rv, ClearColor);

				// �[�x�o�b�t�@���N���A����B 
				context->ClearDepthStencilView(dv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

				// �J������VP�ݒ�
				DX11ThinWrapper::d3::mapping(cameraBuffer.get(), context.get(), [&](D3D11_MAPPED_SUBRESOURCE resource) {
					auto param = static_cast<CameraParam*>(resource.pData);
					auto aspectRatio = modeDesc.Width / (float)modeDesc.Height;
					auto mtxProj = DirectX::XMMatrixPerspectiveFovLH(3.1415926f / 2.0f, aspectRatio, 1.0f, 1000.0f);
					XMStoreFloat4x4(&param->mtxProj, DirectX::XMMatrixTranspose(mtxProj));
					auto mtxView = DirectX::XMMatrixLookAtLH(
						DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1),
						DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1),
						DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1)
					);
					XMStoreFloat4x4(&param->mtxView, DirectX::XMMatrixTranspose(mtxView));
				});
				ID3D11Buffer * cameraBuffers[] = { cameraBuffer.get() };
				context->VSSetConstantBuffers(0, 1, cameraBuffers);

				// ���f���̕`��
				{
					// ���f���̃��[���h�ϊ��s��ݒ�
					DX11ThinWrapper::d3::mapping(modelBuffer.get(), context.get(), [&](D3D11_MAPPED_SUBRESOURCE resource) {
						auto param = static_cast<ModelParam*>(resource.pData);
						XMStoreFloat4x4(&param->mtxWorld, DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
					});
					ID3D11Buffer * modelBuffers[] = { modelBuffer.get() };
					context->VSSetConstantBuffers(1, 1, modelBuffers);

					// ���_����ݒ�
					ID3D11Buffer* vertexBuffers[] = { vertexBuffer.get() };
					auto stride = sizeof(Vertex);
					UINT offset = 0;
					context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

					// InputLayout��ݒ�
					context->IASetInputLayout(inputLayout.get());
				
					// ���_�̕��ѕ���ݒ�
					context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					// �V�F�[�_�[��ݒ�
					context->VSSetShader(vertexShader.get(), NULL, 0);
					context->PSSetShader(pixelShader.get(), NULL, 0);

					// �`��
					context->Draw(_countof(vertices), 0);
				}

				if (FAILED(swapChain->Present(0, 0))) break;
			}
		} while (msg.message != WM_QUIT);
	}

	::CoUninitialize(); // �Ȃ��Ă��������ǏI������O��f��
	return msg.wParam;
}