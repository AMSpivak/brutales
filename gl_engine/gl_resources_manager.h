#ifndef GL_RESOURCES_MANAGER
#define GL_RESOURCES_MANAGER

#include "glresourses.h"

#include "i_gl_jub_struct.h"
#include "i_gl_jal_struct.h"
#include "i_gl_texture_struct.h"
#include "gl_atlas.h"

#include <string>
class GLResourcesManager
{
public:
GLAtlas<IGlTextureStruct> m_texture_atlas;
GLAtlas<IGlJalStruct> m_mesh_atlas;
GLAtlas<Animation> m_animation_atlas;
GLAtlas<IGlJubStruct> m_bones_atlas;

std::map<const std::string,GLuint> m_shader_map;

GLResourcesManager(const std::string & textures_folder,
                    const std::string & meshes_folder,
                    const std::string & animation_folder,
                    const std::string & bones_folder
                    )
                    :m_texture_atlas(textures_folder)
                    ,m_mesh_atlas(meshes_folder)
                    ,m_animation_atlas(animation_folder)
                    ,m_bones_atlas(bones_folder)
{}
~GLResourcesManager(){}

GLuint GetShader(const std::string &shader_name);
void Clean();
};

GLResourcesManager * GetResourceManager();
void SetResourceManager(GLResourcesManager * resource_manager);

#endif