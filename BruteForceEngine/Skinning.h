#ifndef SKINNING_H
#define SKINNING_H
#include "BruteForceMath.h"
#include "PlatformDefine.h"
namespace BruteForce
{
    namespace Skinning
    {

        struct Bone
        {
            Math::Vec4Float Rotation;
            Math::Vec3Float Scaling;
            Math::Vec3Float Translation;
            uint8_t ParentIndex;
        };

        class Armature
        {
            std::vector<Math::Matrix> m_InverseBoneMatrixes;
            std::vector<Bone> m_Bones;
            size_t m_BonesCount;
        public:
            void GenerateSkinningMatrixes(size_t frame1, size_t frame2, float lerp_factor);
            void ReserveBones(size_t count);
            bool PushBone(const Bone& bone, const Math::Matrix& matrix);
            bool PushBone(const Bone& bone, const float * matrix);
            void ClearBones(bool shrink);
            Bone* GetBonePtr(int i) { return i < m_Bones.size() ? &m_Bones[i] : nullptr; };


        };
    }
}

#endif // !SKINNING_H

