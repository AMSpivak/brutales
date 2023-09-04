#ifndef RENDER_OBJECT_H
#define RENDER_OBJECT_H

#include "IndexedGeometry.h"
#include "MaterialManager.h"
#include <memory>

namespace BruteForce
{
    namespace Render
    {

        class RenderObject
        {
        public:
            std::shared_ptr <IndexedGeometry> m_geometry;
            std::shared_ptr<RenderMaterial> m_material;
            //Math::Vec3Float Position;
        };

        class RenderSkinnedObject: RenderObject
        {
            uint8_t m_BonesCount;
        public:
            RenderSkinnedObject() :m_BonesCount(0) {}
            RenderSkinnedObject(const RenderSkinnedObject&) = default;
            RenderSkinnedObject& operator=(const RenderSkinnedObject&) = default;

            RenderSkinnedObject(RenderSkinnedObject&&) = default;
            RenderSkinnedObject& operator=(RenderSkinnedObject&&) = default;
            //std::shared_ptr <IndexedGeometry> m_geometry;
            //std::shared_ptr<RenderMaterial> m_material;
            //Math::Vec3Float Position;
        };
    }
}


#endif
