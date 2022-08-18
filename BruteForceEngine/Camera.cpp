#include "Camera.h"

namespace BruteForce
{
	void Camera::RecalculateView()
	{
		m_View = Math::MatrixLookAtLH(m_Position, m_FocusPoint, m_UpDirection);
	}
	void Camera::RecalculateView(const BruteForce::Math::Vector& focusPoint, const BruteForce::Math::Vector& upDirection)
	{
		m_FocusPoint = focusPoint;
		m_UpDirection = upDirection;
		RecalculateView();
	}

	void Camera::RotateView(const BruteForce::Math::Vector& rotationAxis, float angle)
	{
		Math::Matrix M = Math::MatrixRotationAxis(rotationAxis, BruteForce::Math::DegToRad(angle));
		m_FocusPoint = Math::MatrixVectorMul(M, m_FocusPoint);

		m_View = Math::MatrixLookAtLH(m_Position, Math::VectorAdd(m_Position, m_FocusPoint), m_UpDirection);
		m_ViewProjection = Math::Multiply(m_View, m_Projection);
	}

	void Camera::RotateView(float angle_x, float angle_y, float angle_z)
	{
		Math::Matrix M = Math::MatrixRotationAxis({ 0.0f,1.0f,0.0f,0.0f }, angle_y);
		m_FocusPoint = Math::MatrixVectorMul(M, m_FocusPoint);
		m_RightDirection = Math::MatrixVector3Cross(m_UpDirection, m_FocusPoint);
		M = Math::MatrixRotationAxis(m_RightDirection, angle_x);
		m_FocusPoint = Math::MatrixVectorMul(M, m_FocusPoint);
		/*Math::Matrix M = Math::MatrixRotationRollPitchYaw(Math::Vector{ angle_x, angle_y, 0.0f , 0.0f });
		m_FocusPoint = Math::MatrixVectorMul(M, m_FocusPoint);*/

		m_View = Math::MatrixLookAtLH(m_Position, Math::VectorAdd(m_Position, m_FocusPoint), m_UpDirection);
		m_ViewProjection = Math::Multiply(m_View, m_Projection);
	}

	void Camera::MoveView(float x, float y, float z)
	{
		if (z)
		{
			m_Position = Math::VectorAdd(m_Position, Math::MatrixVectorScale(m_FocusPoint, z));
		}

		if (y)
		{
			m_Position = Math::VectorAdd(m_Position, Math::Vector{ 0.0f, -y, 0.0f, 0.0f });
		}

		if (x)
		{
			m_Position = Math::VectorAdd(m_Position, Math::MatrixVectorScale(Math::MatrixVector3Cross(m_FocusPoint, m_UpDirection) , -x));
		}
	}

	void Camera::RecalculateProjection()
	{
		m_Projection = Math::MatrixPerspectiveFovLH(BruteForce::Math::DegToRad(m_Fov), m_AspectRatio, m_Near, m_Far);
		m_Projection.r[2] = Math::VectorAdd(m_Projection.r[2],{m_JitterX, m_JitterY, 0.f,0.f});
		//m_Projection//.m[2][0] = 0.0f;
		//m_Projection//.m[2][1] = 0.0f;
	}

	void BruteForce::Camera::SetFov(float fov, bool renew_matrixes)
	{
		m_Fov = fov;
		if (renew_matrixes)
		{
			RecalculateProjection();
			m_ViewProjection = Math::Multiply(m_View, m_Projection);
		}
	}

	void Camera::SetPosition(const Math::Vec3Float& position, bool renew_matrixes)
	{
		m_Position = { position.x, position.y, position.z, 0.0f};
		if (renew_matrixes)
		{
			RecalculateProjection();
			m_ViewProjection = Math::Multiply(m_View, m_Projection);
		}
	}

	void Camera::SetAspectRatio(float aspect, bool renew_matrixes)
	{
		m_AspectRatio = aspect;
		if (renew_matrixes)
		{
			RecalculateProjection();
			m_ViewProjection = Math::Multiply(m_View, m_Projection);
		}
	}

	void Camera::SetJitter(float jx, float jy, bool renew_matrixes)
	{
		m_JitterX = jx;
		m_JitterY = jy;
		if (renew_matrixes)
		{
			RecalculateProjection();
			m_ViewProjection = Math::Multiply(m_View, m_Projection);
		}
	}

	Math::Matrix Camera::GetViewProjection()
	{
		return Math::Matrix();
	}
}