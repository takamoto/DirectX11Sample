#pragma once
#include <memory>

// ���܂�ǂ��Ȃ��݌v���Ǝv�����炠�ƂŒ���

struct ID3D11Device;

namespace dx11 {
	class DX11DeviceSharedGuard {
	private:
		std::shared_ptr<ID3D11Device> _device;
	public:
		DX11DeviceSharedGuard();
	};

	::ID3D11Device * AccessDX11Device();
}