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
        Math::Vector m_Position;
        Math::Vector m_FocusPoint;
        Math::Vector m_UpDirection;
        Math::Vector m_RightDirection;
        Math::Vector m_Angles;
        float m_Fov;
        float m_AspectRatio;
        float m_Near;
        float m_Far;
        float m_JitterX;
        float m_JitterY;
        void RecalculateView();
        //void RecalculateView(const BruteForce::Math::Vector& focusPoint, const BruteForce::Math::Vector& upDirection);
        void RecalculateProjection();
    public:
        Camera() :m_Fov{ 45.0 }, m_AspectRatio{ 1.0f }, m_Near{ 0.1f }, m_Far{ 1000.0f }, m_JitterX{ 0.0f }, m_JitterY{ 0.0f },
            m_Position{ 0.0f, 0.0f, -10.0f, 0.0 }, m_FocusPoint{ 0.0f, 0.0f, 1.0f, 0.0 }, m_UpDirection{ 0.0f, 1.0f, 0.0f, 0.0 }, m_RightDirection{ 1.0f, 0.0f, 0.0f, 0.0 },
            m_Angles{ 0.0f, 0.0f, 0.0f, 0.0f}
        {
            m_Projection = BruteForce::Math::MatrixPerspectiveFovLH(BruteForce::Math::DegToRad(m_Fov), m_AspectRatio, m_Near, m_Far);
            RecalculateView();
            m_View = BruteForce::Math::MatrixLookAtLH(m_Position, m_FocusPoint, m_UpDirection);
            m_ViewProjection = BruteForce::Math::Multiply(m_View, m_Projection);
            //RotateView({0,0,1,0},30.0f);
        }
        void RecalculateView(const BruteForce::Math::Vector& focusPoint, const BruteForce::Math::Vector& upDirection);

        void SetFov(float fov, bool renew_matrixes);
        void SetPosition(const Math::Vec3Float& position, bool renew_matrixes);
        void SetAspectRatio(float aspect, bool renew_matrixes);
        void SetJitter(float jx, float jy, bool renew_matrixes);
        const Math::Matrix* GetCameraMatrixPointer()  const{ return &m_ViewProjection; }
        void RotateView(const BruteForce::Math::Vector& rotationAxis, float angle);
        void RotateView(float angle_x, float angle_y, float angle_z);
        void MoveView(float x, float y, float z);
        Math::Matrix GetViewProjection();
        const Math::Vector& GetPosition() const { return  m_Position; };
    };
}


#endif
