#ifndef GL_ReSOURSES
#define GL_ReSOURSES

#include <GL/glew.h>
#include <string>
#include <SOIL/SOIL.h>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glcamera.h"



template <class T>
//T FitRing(T value, T min, T max)
T FitRing(const T &value,const T &min, const T &max)
{
	T ret_value = value;
	if(ret_value > max)
	{
		ret_value -=  max;
	}
	if(ret_value < min)
	{
		ret_value +=  max;
	}
	return ret_value;
}

inline glm::vec2 VectorToPlane(glm::vec3 value)
{
	return glm::vec2(value[0],-value[2]);
}

inline glm::vec3 VectorFromPlane(glm::vec2 vec_value, float value)
{
	return glm::vec3(vec_value[0],value,-vec_value[1]);
}

struct Bone {
	std::string name;
	std::string parent;
	glm::mat4 matrix;
};

struct AnimationFrame
{
	std::vector <glm::mat4> bones;
	GLfloat * UnsafeReturnDataPtr()
	{
		return reinterpret_cast<GLfloat*>(glm::value_ptr(bones[0]));
	}
};

class Animation
{
public:
	size_t framescount;
	std::vector <AnimationFrame> frames;
	void LoadAnimation(const std::string &file_name, std::vector <Bone> &bones);
	void LoadAnimation(const std::string &file_name);
	void CalculateCache(const std::vector <Bone> &bones,size_t frame);
	Animation():framescount(0),m_precalculated(false), m_cache_frame(0)
	{}
	Animation(const std::string &file_name);
	GLfloat * GetDrawValues(size_t frame,const std::vector <Bone> &bones);
	const glm::mat4 & GetBoneMatrix(size_t frame,size_t bone,const std::vector <Bone> &bones);
	const glm::mat4 & GetRotationMatrix(size_t frame,const std::vector <Bone> &bones);
	
private:
	bool m_precalculated;
	size_t m_cache_frame;
	AnimationFrame m_cashe_animation;
	glm::mat4 rotation_matrix;

};



std::istream& operator>> ( std::istream& is, Bone& bone) ;

std::istream& operator >> (std::istream& is, glm::vec4& glm_vector);


std::istream& operator >> ( std::istream& is, glm::vec3 & glm_vector);
std::ostream& operator << ( std::ostream& os, const glm::vec3 & glm_vector);

std::istream& operator >> ( std::istream& is, glm::vec2 & glm_vector);
std::ostream& operator << ( std::ostream& os, const glm::vec2 & glm_vector);

std::istream& operator >> ( std::istream& is, glm::mat4& mat);
std::ostream& operator << ( std::ostream& os, const glm::mat4 & mat);

std::ostream& operator << ( std::ostream& os, const glm::vec2 & glm_vector);

template <typename T1, typename T2>
std::ostream& operator << (std::ostream& os, const std::pair<T1, T2>& value)
{
	os << value.first << " " << value.second;
	return os;
}

template <typename T1, typename T2>
std::istream& operator>> (std::istream& is, std::pair<T1, T2>& value)
{
	is >> value.first >> value.second;
	return is;
}

std::string readShaderFile(std::string FileName);

GLuint LoadShader(std::string FileName,GLenum shaderType);
GLuint LinkShaderProgram(GLuint * shaders, int shaders_count);
GLuint LoadshaderProgram(std::string FileNameVS,std::string FileNameFS);
GLuint LoadshaderProgram(std::string FileNameVS,std::string FileNameFS,std::string FileNameGS);

void EmptyShaders(GLuint * shaders, int shaders_count);

void LoadVertexArray(std::string FileName,GLuint &VBO, GLuint &VBO_BONES, GLuint &VBO_BONES_IDX, int &vertex_count);
void LoadBonesArray(std::string FileName,std::vector <Bone> &bones,int &bones_count);
void LoadTexture(std::string FileName,GLuint &texture);
void LoadCubemap(const std::string FileName,GLuint &texture);

void renderQuad();

void Fit_Matrix(glm::mat4 &matrix,float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3);

void renderSprite(GLuint current_shader,
	float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3,
	const glm::vec4 & corrector_v,
	const GLuint * texture,
	float t_sprite_w = 1.0f,float t_sprite_h = 1.0f,
	float t_sprite_offset_x = 0.0f,
	float t_sprite_offset_y = 0.0f);

void renderSpriteDepth(GLuint current_shader, GLuint depthmap, float sprite_depth,
	float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3,
	const glm::vec4 & corrector_v,
	const GLuint * texture 
);

void renderBillBoardDepth(GLuint current_shader, GLuint depthmap,const GLuint * texture,
						 float width, float height,const glm::vec4 & corrector,
						 const glm::vec3 & position, const glm::vec3 & offset, 
						 GlScene::glCamera & camera);

void RenderSingleTriangle(GLuint current_shader, GLuint depthmap, 
	const glm::vec3 & p1, const glm::vec2 & t1,
	const glm::vec3 & p2, const glm::vec2 & t2,
	const glm::vec3 & p3, const glm::vec2 & t3,
	const glm::vec4 & corrector_v,
	GlScene::glCamera & camera,
	const GLuint * texture);

void RenderHeightMap();

unsigned char * LoadHeightMap(std::string FileName,int * tex_width, int * tex_height);

void DeleteHeightMap(unsigned char * image);

const glm::mat4 SlerpMatrix(const glm::mat4 & m1,const glm::mat4 & m2,float approximation);

#endif
