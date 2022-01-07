#include "Camera.h"

namespace BruteForce
{
	void Camera::RecalculateView()
	{
		const Math::Vector eyePosition = BruteForce::Math::VectorSet(m_Position.x, m_Position.y, m_Position.z, 1);
		const Math::Vector focusPoint = BruteForce::Math::VectorSet(0, 0, 0, 1);
		const Math::Vector upDirection = BruteForce::Math::VectorSet(0, 1, 0, 0);
		m_View = Math::MatrixLookAtLH(eyePosition, focusPoint, upDirection);
	}
	void Camera::RecalculateView(const BruteForce::Math::Vector& focusPoint, const BruteForce::Math::Vector& upDirection)
	{
		const Math::Vector eyePosition = BruteForce::Math::VectorSet(m_Position.x, m_Position.y, m_Position.z, 1);
		m_View = Math::MatrixLookAtLH(eyePosition, focusPoint, upDirection);
	}

	void Camera::RotateView(const BruteForce::Math::Vector& rotationAxis, float angle)
	{
		Math::Matrix tmp = Math::MatrixRotationAxis(rotationAxis, BruteForce::Math::DegToRad(angle));
		m_View = Math::Multiply(m_View, tmp);

		
		m_ViewProjection = Math::Multiply(m_View, m_Projection);
	}

	void Camera::MoveView(float x, float y, float z)
	{
		Math::Matrix tmp = Math::MatrixTranslation(x, y, z);
		m_View = Math::Multiply(m_View, tmp);
		m_ViewProjection = Math::Multiply(m_View, m_Projection);
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