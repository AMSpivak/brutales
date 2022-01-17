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
		Math::Matrix M = Math::MatrixRotationRollPitchYaw(Math::Vector{ angle_x, angle_y, 0.0f , 0.0f });
		m_FocusPoint = Math::MatrixVectorMul(M, m_FocusPoint);

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

	Math::Matrix Camera::GetViewProjection()
	{
		return Math::Matrix();
	}
}