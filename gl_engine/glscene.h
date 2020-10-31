#ifndef GL_ENGINE_SCENE
#define GL_ENGINE_SCENE

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <vector>
#include <set>
#include <list>
#include <memory>
#include "glresourses.h"
#include "gl_material.h"
#include "i_gl_jal_struct.h"
#include "glcamera.h"
#include <iostream>

class glModel;

namespace GlScene
{
    struct Scene
    {
        bool LockedShader = false;
        GLuint render_shader = 0;
        glm::vec3 zero_offset;
        const glCamera * render_camera = nullptr;
        std::weak_ptr<GameResource::GlMaterial> material;
        std::weak_ptr<IGlJalStruct> jal_mesh;
        //std::multiset<std::shared_ptr<glModel>, [](std::shared_ptr<glModel> a, std::shared_ptr<glModel> b) { return *a < *b; } > model_list;
        std::list<std::shared_ptr<glModel>> model_list;
    };
}

#endif
