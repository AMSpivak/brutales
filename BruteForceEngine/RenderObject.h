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
    }
}


#endif
