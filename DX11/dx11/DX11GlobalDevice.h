#pragma once
#include <memory>
#include <d3d11.h>

// あまり良くない設計だと思うからあとで直す

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