#pragma once
#include <memory>
#include <d3d11.h>

namespace dx11 {
	// �X���b�v�`�F�C���̐���
	std::shared_ptr<IDXGISwapChain> CreateDefaultSwapChain(
		DXGI_MODE_DESC * displayMode, HWND hWnd, ID3D11Device * device, bool useMultiSample
	);
};