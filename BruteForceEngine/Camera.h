#ifndef _BRUTEFORCE_CAMERA_H
#define _BRUTEFORCE_CAMERA_H
#include "PlatformDefine.h"

namespace BruteForce
{
    class Camera
    {
        Math::Matrix m_Projection;
        Math::Matrix m_View;
        Math::Matrix m_ViewProjection;
        Math::Vec3Float m_Position;
        float m_Fov;
        float m_AspectRatio;
        float m_Near;
        float m_Far;
        void RecalculateView();
        void RecalculateView(const BruteForce::Math::Vector& focusPoint, const BruteForce::Math::Vector& upDirection);
        void RecalculateProjection();
    public:
        Camera() :m_Fov{ 45.0 }, m_AspectRatio{ 1.0f }, m_Near{ 0.1f }, m_Far{ 100.0f }, m_Position{ 0.0f, 0.0f, -10.0f }
        {
            m_Projection = BruteForce::Math::MatrixPerspectiveFovLH(BruteForce::Math::DegToRad(m_Fov), m_AspectRatio, m_Near, m_Far);
            const BruteForce::Math::Vector eyePosition = BruteForce::Math::VectorSet(m_Position.x, m_Position.y, m_Position.z, 1);
            const BruteForce::Math::Vector focusPoint = BruteForce::Math::VectorSet(0, 0, 0, 1);
            const BruteForce::Math::Vector upDirection = BruteForce::Math::VectorSet(0, 1, 0, 0);
            m_View = BruteForce::Math::MatrixLookAtLH(eyePosition, focusPoint, upDirection);
            m_ViewProjection = BruteForce::Math::Multiply(m_View, m_Projection);
            //RotateView({0,0,1,0},30.0f);
        }
        void SetFov(float fov, bool renew_matrixes);
        void SetPosition(const Math::Vec3Float& position, bool renew_matrixes);
        void SetAspectRatio(float aspect, bool renew_matrixes);
        const Math::Matrix* GetCameraMatrixPointer() const { return &m_ViewProjection; }
        void RotateView(const BruteForce::Math::Vector& rotationAxis, float angle);
        void MoveView(float x, float y, float z);
        Math::Matrix GetViewProjection();
    };
}


#endif
