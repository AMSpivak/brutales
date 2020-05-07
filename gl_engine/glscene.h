#ifndef GL_ENGINE_SCENE
#define GL_ENGINE_SCENE

#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <vector>
#include <memory>
#include "glresourses.h"
#include "gl_material.h"
#include "glcamera.h"
#include <iostream>

namespace GlScene
{
    struct Scene
    {
        GLuint render_shader = 0;
        glm::vec3 zero_offset;
        const glCamera * render_camera = nullptr;
        std::weak_ptr<GameResource::GlMaterial> material;
    };
}

#endif
