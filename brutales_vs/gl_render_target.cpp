#include "gl_render_target.h"

void glRenderTargetSimple::InitBuffer(unsigned int WIDTH, unsigned int HEIGHT,float buffer_scale)
{
	width = static_cast<unsigned int>(buffer_scale * WIDTH);
	height = static_cast<unsigned int>(buffer_scale * HEIGHT);

	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	GenerateBuffers();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl<<std::hex<<glCheckFramebufferStatus(GL_FRAMEBUFFER)<<std::dec<<std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void glRenderTargetSimple::set()
{
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

glRenderTargetSimple::~glRenderTargetSimple()
{
	glDeleteTextures(1,&AlbedoMap);
	glDeleteBuffers(1, &FBO);
	std::cout << "RenderTargetSimple cleared!" << std::endl;
}

void glRenderTargetSimple::GenerateBuffers()
{
	glGenTextures(1, &AlbedoMap);
	glBindTexture(GL_TEXTURE_2D, AlbedoMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, AlbedoMap, 0);

	unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, attachments);
}

glRenderTarget::~glRenderTarget()
{
	glDeleteTextures(1,&depthMap);
	glDeleteTextures(1,&NormalMap);
	glDeleteBuffers(1, &StencilBuffer);
	std::cout << "RenderTarget cleared!" << std::endl;
}

void glRenderTarget::GenerateBuffers()
{
	glRenderTargetSimple::GenerateBuffers();
	glGenRenderbuffers(1, &StencilBuffer);
		  

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_STENCIL, width, height, 0, GL_DEPTH_STENCIL, GL_FLOAT_32_UNSIGNED_INT_24_8_REV, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

	glGenTextures(1, &NormalMap);
	glBindTexture(GL_TEXTURE_2D, NormalMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	// glGenTextures(1, &AlbedoMap);
	// glBindTexture(GL_TEXTURE_2D, AlbedoMap);

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, AlbedoMap, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, NormalMap, 0);


	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
	glDrawBuffers(2, attachments);



}




void glRenderTargetDeffered ::GenerateBuffers()
{
	glRenderTarget::GenerateBuffers();

	glGenTextures(1, &PositionMap);
	glBindTexture(GL_TEXTURE_2D,PositionMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, PositionMap, 0);

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2};
	glDrawBuffers(3, attachments);

}

glRenderTargetDeffered::~glRenderTargetDeffered()
{
	glDeleteTextures(1, &PositionMap);
}

void glRenderTargetCubicSimple::InitBuffer(unsigned int size)
{
	m_size = size;
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	GenerateBuffers();

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl << std::hex << glCheckFramebufferStatus(GL_FRAMEBUFFER) << std::dec << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glRenderTargetCubicSimple::~glRenderTargetCubicSimple()
{
	glDeleteTextures(1, &AlbedoMap);
	glDeleteBuffers(1, &FBO);
	std::cout << "RenderTargetCubicSimple cleared!" << std::endl;
}

const glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
const glm::mat4 captureViews[] =
{
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
};

void glRenderTargetCubicSimple::set()
{
	glViewport(0, 0, m_size, m_size);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
}

const glm::mat4* const glRenderTargetCubicSimple::SwitchSide(int side_num)
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_CUBE_MAP_POSITIVE_X + side_num, AlbedoMap, 0);
	return &captureViews[side_num];
}

void glRenderTargetCubicSimple::GenerateBuffers()
{
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glGenTextures(1, &AlbedoMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, AlbedoMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			m_size, m_size, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}
