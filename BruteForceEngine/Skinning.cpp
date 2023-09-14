#include "Skinning.h"

namespace BruteForce
{
    namespace Skinning
    {
        void Armature::ReserveBones(size_t count)
        {
            m_Bones.reserve(count);
            m_InverseBoneMatrixes.reserve(count);
            m_BonesCount = count;
        }
        bool Armature::PushBone(const Bone& bone, const Math::Matrix& matrix)
        {
            if (m_Bones.size() < m_BonesCount)
            {
                m_Bones.push_back(bone);
                m_InverseBoneMatrixes.push_back(matrix);
                return true;
            }
            return false;
        }

        bool Armature::PushBone(const Bone& bone, const float* matrix)
        {
            if (m_Bones.size() < m_BonesCount)
            {
                m_Bones.push_back(bone);
                //m_InverseBoneMatrixes.push_back(matrix); // todo: add correct matrix init
                return true;
            }
            return false;
        }

        void Armature::ClearBones(bool shrink)
        {
            m_Bones.clear();
            m_InverseBoneMatrixes.clear();
            m_BonesCount = 0;
            if (shrink)
            {
                m_Bones.shrink_to_fit();
                m_InverseBoneMatrixes.shrink_to_fit();
            }
        }
    }
}