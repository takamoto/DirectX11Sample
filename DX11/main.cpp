#include <DirectXMath.h>
#include <memory>
#include "./win/Window.h"
#include "./win/WindowProcedure.h"
#include "./dx11/DX11ThinWrapper.h"
#include "./dx11/DX11GlobalDevice.h"
#include "./dx11/DX11DefaultInitialize.h"
#include "./dx11/DX11DefaultSetting.h"
#include "./dx11/DirectXShaderLevelDefine.h"
#include "./dx11/DX11WrappedPrimitive.h"
#include "./dx11/DX11Canvas.h"

#include "./Camera.h"
#include "./Viewport.h"

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
		using namespace DirectX;

		// ������
		dx11::DX11DeviceSharedGuard guard;
		auto device = dx11::AccessDX11Device();

		auto context = DX11ThinWrapper::d3::AccessD3ImmediateContext(device);
		auto swapChain = dx11::CreateDefaultSwapChain(&modeDesc, window.getHWnd(), device, true);
		auto canvas = dx11::Canvas(context, swapChain);
		
		std::vector<std::shared_ptr<Viewport>> viewports({
			std::make_shared<Viewport>(XMFLOAT2{ 400, 300 }, XMFLOAT2{ 0, 0 }),
			std::make_shared<Viewport>(XMFLOAT2{ 400, 300 }, XMFLOAT2{ 400, 0 }),
			std::make_shared<Viewport>(XMFLOAT2{ 400, 300 }, XMFLOAT2{ 0, 300 })
		});
		
		std::vector<std::shared_ptr<Camera>> cameras({
			std::make_shared<Camera>(XMFLOAT3(0, 0, -5.f), XMFLOAT3(0, 0, 0)),
			std::make_shared<Camera>(XMFLOAT3(0, 0, -5.f), XMFLOAT3(0, 0, 0)),
			std::make_shared<Camera>(XMFLOAT3(0, 0, -20.f), XMFLOAT3(0, 0, 0))
		});

		//// �萔�o�b�t�@�� float*4�̔{���̃T�C�Y�ł���K�v������
		//// ���̑��͂܂肻���ȂƂ��� �F https://twitter.com/43x2/status/144821841977028608
		// World�p �萔�o�b�t�@
		struct WorldParam {
			DirectX::XMFLOAT4X4	matWorld;
		};

		using WorldBuffer = dx11::primitive::ConstantBuffer<WorldParam>;
		auto worldBuffer = WorldBuffer(device, nullptr, D3D11_CPU_ACCESS_WRITE);

		// �e�N�X�`���ǂݍ���
		auto tex = DX11ThinWrapper::d3::CreateWICTextureFromFile(device, L"./texture.jpg");
		auto sampler = DX11ThinWrapper::d3::CreateSampler(device, dx11::default_setting::SamplerState(
			D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP)
		);

		// ���_��񐶐�
		// �T�C�Y�����ɂ���ĈႤ�^�͗p���Ȃ��B������int32_t����p�����ق������S�B
		// �C���f�b�N�X�o�b�t�@�����l�B
		struct Vertex {
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
			DirectX::XMFLOAT3 normal;
			DirectX::XMFLOAT2 uv;
		};
		Vertex vertices[] =
		{
			{ DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), DirectX::XMFLOAT4(), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(0.0f, 0.0f) },
			{ DirectX::XMFLOAT3(1.0f, 1.0f, -1.0f), DirectX::XMFLOAT4(), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1.0f, 0.0f) },
			{ DirectX::XMFLOAT3(1.0f, -2.0f, -1.0f), DirectX::XMFLOAT4(), DirectX::XMFLOAT3(), DirectX::XMFLOAT2(1.0f, 1.0f) }
		};
		auto vertexBuffer = DX11ThinWrapper::d3::CreateVertexBuffer(device, static_cast<void*>(vertices), sizeof(vertices));
	
		// �V�F�[�_�[����
		D3D11_INPUT_ELEMENT_DESC layout_desc[] = {
			{ "IN_POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "IN_COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "IN_NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "IN_TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		auto vertex_blob = DX11ThinWrapper::d3::CompileShader(_T("vs.hlsl"), "RenderVS", VERTEX_SHADER_PROFILE);
		auto vertexShader = DX11ThinWrapper::d3::CreateVertexShader(device, vertex_blob.get());
		auto inputLayout = DX11ThinWrapper::d3::CreateInputLayout(device, layout_desc, _countof(layout_desc), vertex_blob.get());

		auto geometryShader = DX11ThinWrapper::d3::CreateGeometryShader(
			device, DX11ThinWrapper::d3::CompileShader(_T("gs.hlsl"), "RenderGS", GEOMETORY_SHADER_PROFILE).get()
		);

		auto pixelShader = DX11ThinWrapper::d3::CreatePixelShader(
			device, DX11ThinWrapper::d3::CompileShader(_T("ps.hlsl"), "RenderPS", PIXEL_SHADER_PROFILE).get()
		);

		// �E�B���h�E�\��
		::ShowWindow(window.getHWnd(), SW_SHOW);
		::UpdateWindow(window.getHWnd());

		// ���C�����[�v
		do {
			if (::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			} else {
				canvas.begin(viewports, cameras);

				// ���f���̕`��
				{
					canvas.setDrawable();

					// ���f���̃��[���h�ϊ��s��ݒ�
					worldBuffer.mapping(context.get(), [](WorldBuffer::ResourceType resource) {
						auto&& data = *resource.pData;
						XMStoreFloat4x4(&data.matWorld, DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity()));
					});
					ID3D11Buffer * worldBuffers[] = { worldBuffer.get() };
					context->GSSetConstantBuffers(0, 1, worldBuffers); // ����slot���ߑł�

					// ���_����ݒ�
					ID3D11Buffer* vertexBuffers[] = { vertexBuffer.get() };
					auto stride = sizeof(Vertex);
					UINT offset = 0;
					context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);

					// InputLayout��ݒ�
					context->IASetInputLayout(inputLayout.get());
				
					// ���_�̕��ѕ���ݒ�
					// �ڍ� http://msdn.microsoft.com/ja-jp/library/ee415716(v=vs.85).aspx
					context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					// �V�F�[�_�[��ݒ�
					context->VSSetShader(vertexShader.get(), NULL, 0);
					context->GSSetShader(geometryShader.get(), NULL, 0);
					context->PSSetShader(pixelShader.get(), NULL, 0);

					// �e�N�X�`�����Z�b�g
					ID3D11ShaderResourceView * views[] = { tex.get() };
					context->PSSetShaderResources(0, 1, views);

					ID3D11SamplerState * samplers[] = { sampler.get() };
					context->PSSetSamplers(0, 1, samplers);

					// �`��
					context->Draw(_countof(vertices), 0);
				}

				if (!canvas.end()) break;
			}

		} while (msg.message != WM_QUIT);
	}

	::CoUninitialize(); // �Ȃ��Ă��������ǏI������O��f��
	return msg.wParam;
}