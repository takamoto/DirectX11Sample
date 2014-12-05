#include "../DX11ThinWrapper.h"
#include <stdexcept>

#pragma comment(lib, "d3dcompiler.lib")

namespace {
	template<typename S> std::shared_ptr<S> CreateShader(
		ID3D11Device* device, BYTE* shader, size_t size,
		HRESULT(__stdcall ID3D11Device::*func)(const void *, SIZE_T, ID3D11ClassLinkage *, S **)
	) {
		S * shaderObject = nullptr;
		auto hr = (device->*func)(shader, size, nullptr, &shaderObject);
		if (FAILED(hr)) throw std::runtime_error("�V�F�[�_�[�̐����Ɏ��s���܂���.");
		return std::shared_ptr<S>(shaderObject, DX11ThinWrapper::ReleaseIUnknown);
	}
}

namespace DX11ThinWrapper {
	namespace d3 {
		std::shared_ptr<ID3DBlob> CompileShader(TCHAR source[], CHAR entryPoint[], CHAR target[]) {
			// �s����D��Őݒ肵�A�Â��`���̋L�q�������Ȃ��悤�ɂ���
			UINT flag1 = D3D10_SHADER_PACK_MATRIX_COLUMN_MAJOR | D3D10_SHADER_ENABLE_STRICTNESS;

			// �œK�����x����ݒ肷��
			#if defined(DEBUG) || defined(_DEBUG)
				flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL0;
			#else
				flag1 |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
			#endif

			ID3DBlob* blob = nullptr;
			auto hr = ::D3DCompileFromFile(source, nullptr, nullptr, entryPoint, target, flag1, 0, &blob, nullptr);
			if (FAILED(hr)) throw std::runtime_error("�V�F�[�_�[�t�@�C���̃R���p�C���Ɏ��s���܂���.");
			return std::shared_ptr<ID3DBlob>(blob, ReleaseIUnknown);
		}

		std::shared_ptr<ID3D11PixelShader> CreatePixelShader(ID3D11Device* device, ID3DBlob * blob) {
			return CreateShader(device, (BYTE*)blob->GetBufferPointer(), blob->GetBufferSize(), &ID3D11Device::CreatePixelShader);
		}
		std::shared_ptr<ID3D11VertexShader> CreateVertexShader(ID3D11Device* device, ID3DBlob * blob) {
			return CreateShader(device, (BYTE*)blob->GetBufferPointer(), blob->GetBufferSize(), &ID3D11Device::CreateVertexShader);
		}
		std::shared_ptr<ID3D11GeometryShader> CreateGeometryShader(ID3D11Device* device, ID3DBlob * blob) {
			return CreateShader(device, (BYTE*)blob->GetBufferPointer(), blob->GetBufferSize(), &ID3D11Device::CreateGeometryShader);
		}
		std::shared_ptr<ID3D11InputLayout> CreateInputLayout(
			ID3D11Device* device, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numOfLayout, ID3DBlob * blob
		) {
			ID3D11InputLayout* layout;
			auto hr = device->CreateInputLayout(
				layoutDesc, numOfLayout, (BYTE*)blob->GetBufferPointer(), blob->GetBufferSize(), &layout
			);
			if (FAILED(hr)) throw std::runtime_error("���̓��C�A�E�g�̐����Ɏ��s���܂���.");
			return std::shared_ptr<ID3D11InputLayout>(layout, DX11ThinWrapper::ReleaseIUnknown);
		}
	}
}