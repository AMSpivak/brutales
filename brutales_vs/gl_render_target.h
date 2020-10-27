#ifndef GL_ENGINE_RENDER_TARGET
#define GL_ENGINE_RENDER_TARGET
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <string>
#include <vector>
#include <memory>
#include "glresourses.h"
#include <iostream>

class glRenderTargetSimple
{

public:
	GLuint AlbedoMap = 0;
	virtual void InitBuffer(unsigned int WIDTH, unsigned int HEIGHT, float buffer_scale);

	virtual ~glRenderTargetSimple();

	void set();

protected:

	virtual void GenerateBuffers();
	unsigned int width = 0;
	unsigned int height = 0;
	GLuint FBO = 0;
//private:

};

class glRenderTarget : public glRenderTargetSimple
{

public:
	GLuint depthMap;
	GLuint NormalMap;
	GLuint StencilBuffer;

	//void InitBuffer(unsigned int WIDTH, unsigned int HEIGHT, float buffer_scale);

	virtual ~glRenderTarget();


protected:

	virtual void GenerateBuffers();
//private:



};

class glRenderTargetDeffered : public glRenderTarget
{

public:
	GLuint PositionMap;

	~glRenderTargetDeffered();

private:
	void GenerateBuffers();

};

class glRenderTargetCubicSimple
{

public:
	GLuint AlbedoMap = 0;
	virtual void InitBuffer(unsigned int size);

	virtual ~glRenderTargetCubicSimple();

	void set();
	const glm::mat4* const SwitchSide(int side_num);

protected:

	virtual void GenerateBuffers();
	unsigned int m_size = 0;
	GLuint FBO = 0;
	//private:

};
#endif
