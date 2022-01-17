#ifndef INDEXED_GEOMETRY_GENERATOR_H
#define INDEXED_GEOMETRY_GENERATOR_H
#include "IndexedGeometry.h"

namespace BruteForce
{
    namespace Geometry
    {
        void CreateCube(Device& device, IndexedGeometry& geometry);
        void CreatePlane(Device& device, IndexedGeometry& geometry, size_t cells_x, size_t cells_y, float scale_x, float scale_y);
    }
}
#endif
