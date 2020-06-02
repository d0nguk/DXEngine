#pragma once

#include "iComponent.h"

class Transform : public iComponent
{
public:
	Transform();
	virtual ~Transform();

public:
	virtual BOOL Init() override;
	BOOL Init(XMFLOAT3 vPos, XMFLOAT3 vRot, XMFLOAT3 vScale);
	virtual void Update(float dTime = 0.0f) override;
	virtual void LateUpdate(float dTime = 0.0f) override;
	virtual void Release() override;

public:
	XMFLOAT4X4 GetTM() { return m_mTM; }

public:
	void Translate(float _Factor)
	{
		Position.x += _Factor;
		Position.y += _Factor;
		Position.z += _Factor;
	}
	void Translate(float _xFactor, float _yFactor, float _zFactor)
	{
		Position.x += _xFactor;
		Position.y += _yFactor;
		Position.z += _zFactor;
	}
	void Translate(XMFLOAT3 _Factor)
	{
		Position.x += _Factor.x;
		Position.y += _Factor.y;
		Position.z += _Factor.z;
	}

	// Radian Angle
	void Rotate(float _Angle)
	{
		Rotation.x += _Angle;
		Rotation.y += _Angle;
		Rotation.z += _Angle;
	}
	// Radian Angle
	void Rotate(float _xAngle, float _yAngle, float _zAngle)
	{
		Rotation.x += _xAngle;
		Rotation.y += _yAngle;
		Rotation.z += _zAngle;
	}

	void Scaling(float _Factor)
	{
		Scale.x *= _Factor;
		Scale.y *= _Factor;
		Scale.z *= _Factor;
	}
	void Scaling(float _xFactor, float _yFactor, float _zFactor)
	{
		Scale.x *= _xFactor;
		Scale.y *= _yFactor;
		Scale.z *= _zFactor;
	}

private:
	XMFLOAT4X4 m_mTrans, m_mRot, m_mScale, m_mTM;
	XMFLOAT3 &m_vPos, &m_vRot, &m_vScale;
public:
	XMFLOAT3 Position, Rotation, Scale;
};