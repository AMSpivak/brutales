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
            std::vector<Bone> Bones;
        public:
            void GenerateSkinningMatrixes(size_t frame1, size_t frame2, float lerp_factor);

        };
    }
}

#endif // !SKINNING_H

