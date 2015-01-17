#pragma once
#include <DirectXMath.h>

class Viewport {
private:
	DirectX::XMFLOAT2 screenSize;
	DirectX::XMFLOAT2 screenPosition;
	float fovAngleY = 3.1415926f / 2.0f;
	float nearZ = 1.0f;
	float farZ = 1000.0f;
public:
	Viewport(
		const DirectX::XMFLOAT2 & screenSize,
		const DirectX::XMFLOAT2 & screenPosition
	) : screenSize(screenSize), screenPosition(screenPosition) {}
	float getFovAngleY() const { return fovAngleY; }
	float getAspectRatio() const { return screenSize.x / screenSize.y; }
	float getNearZ() const { return nearZ; }
	float getFarZ() const { return farZ; }
	DirectX::XMFLOAT2 getScreenSize() const { return screenSize; }
	DirectX::XMFLOAT2 getScreenPosition() const { return screenPosition; }
};