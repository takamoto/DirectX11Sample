#pragma once
#include <d3d11.h>
#include <memory>
#include <functional>

namespace dx11{
	namespace primitive{
		template<class T> struct MappedSubResource {
			T * pData;
			UINT RowPitch;
			UINT DepthPitch;

			MappedSubResource(const D3D11_MAPPED_SUBRESOURCE & r)
				: pData(static_cast<T*>(r.pData)), RowPitch(r.RowPitch), DepthPitch(r.DepthPitch) {}
		};

		template<class T> class ConstantBuffer {
		private:
			std::shared_ptr<ID3D11Buffer> buffer;
		public:
			using ResourceType = MappedSubResource<T>;

			ConstantBuffer() = default;
			ConstantBuffer(ID3D11Device * device, void * data, UINT cpuAccessFlag = 0U) {
				buffer = DX11ThinWrapper::d3::CreateConstantBuffer(device, data, sizeof T, cpuAccessFlag);
			}
			void mapping(ID3D11DeviceContext * context, std::function<void(ResourceType)> f) {
				DX11ThinWrapper::d3::mapping(buffer.get(), context, [&](D3D11_MAPPED_SUBRESOURCE resource) {
					f(ResourceType(resource));
				});
			}
			ID3D11Buffer * get() { return buffer.get(); }
		};
	}
}