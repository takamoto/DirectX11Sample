#pragma once
#include <d3d11.h>

namespace dx11{
	// ������������
	D3D11_SAMPLER_DESC SamplerState(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE addressMode);
}