#pragma once
#include <DirectXMath.h>

class Camera {
public:
	Camera(
		const DirectX::XMFLOAT3 & pos,
		const DirectX::XMFLOAT3 & focus,
		const DirectX::XMFLOAT3 & upDirection = { 0, 1, 0 }
		) : pos(pos), focus(focus), upDirection(upDirection) {
	}
	DirectX::XMFLOAT3 getPosition() const { return pos; }
	DirectX::XMFLOAT3 getFocus() const { return focus; }
	DirectX::XMFLOAT3 getUpDirection() const { return upDirection; }
private:
	bool rockOnFlag;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 focus;
	DirectX::XMFLOAT3 upDirection;
};