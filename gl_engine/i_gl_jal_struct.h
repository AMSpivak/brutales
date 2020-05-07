#ifndef GL_JAL_STRUCT
#define GL_JAL_STRUCT
#include <GL/glew.h>
#include <string>
#include "glresourses.h"
struct IGlJalStruct
{
    GLuint VBO, VBO_BONES, VBO_BONES_IDX, VAO;
	int vertexcount;

    IGlJalStruct(const std::string & filename)
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &VBO_BONES);
        glGenBuffers(1, &VBO_BONES_IDX);

        LoadVertexArray(filename, VBO, VBO_BONES, VBO_BONES_IDX, vertexcount);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_BONES_IDX);

        glVertexAttribIPointer(4, 4, GL_INT, 4 * sizeof(GL_INT), (GLvoid*)0);
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_BONES);

        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(5);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
        glBindVertexArray(0);
    }
    
    ~IGlJalStruct()
    {
        glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &VBO_BONES);
		glDeleteBuffers(1, &VBO_BONES_IDX);
    }
};
#endif