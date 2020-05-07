#ifndef GL_JUB_STRUCT
#define GL_JUB_STRUCT
#include <GL/glew.h>
#include <string>
#include "glresourses.h"

struct IGlJubStruct
{
    std::vector <Bone> bones;
	int bonescount;

    IGlJubStruct(const std::string & filename)
    {
        LoadBonesArray(filename,bones,bonescount);
    }
    
    ~IGlJubStruct()
    {
    }
};
#endif