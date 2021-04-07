#include "glresourses.h"
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <fstream>
#include <array>




void Fit_Matrix(glm::mat4 &matrix,float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3)
{
	float mScale_Matrix[16] = {0};

	//Matrix.setIdentityM(mScale_Matrix,0);
	mScale_Matrix[0]=x0;
	mScale_Matrix[1]=y0;
	mScale_Matrix[2]=0.0f;
	mScale_Matrix[3]=1.0f;
	mScale_Matrix[4]=x1;
	mScale_Matrix[5]=y1;
	mScale_Matrix[6]=0.0f;
	mScale_Matrix[7]=1.0f;
	mScale_Matrix[8]=x3;
	mScale_Matrix[9]=y3;
	mScale_Matrix[10]=0.0f;
	mScale_Matrix[11]=1.0f;
	mScale_Matrix[12]=x2;
	mScale_Matrix[13]=y2;
	mScale_Matrix[14]=0.0f;
	mScale_Matrix[15]=1.0f;

	matrix = glm::make_mat4(mScale_Matrix);

}

void Fit_Tile_Matrix(glm::mat4 &matrix,float x,float y,float width,float height)
{
	float mScale_Matrix[16] = {0};

	//Matrix.setIdentityM(mScale_Matrix,0);
	mScale_Matrix[0]=width;
	mScale_Matrix[1]=0.0f;
	mScale_Matrix[2]=0.0f;
	mScale_Matrix[3]=0.0f;
	mScale_Matrix[4]=0.0f;
	mScale_Matrix[5]=height;
	mScale_Matrix[6]=0.0f;
	mScale_Matrix[7]=0.0f;
	mScale_Matrix[8]=y;//0.0f;
	mScale_Matrix[9]=0.0f;
	mScale_Matrix[10]=0.0f;
	mScale_Matrix[11]=0.0f;
	mScale_Matrix[12]=x;//0.0f;
	mScale_Matrix[13]=y;//0.0f;
	mScale_Matrix[14]=0.0f;
	mScale_Matrix[15]=1.0f;
	// 	mScale_Matrix[0]=width;
	// mScale_Matrix[1]=0.0f;
	// mScale_Matrix[2]=0.0f;
	// mScale_Matrix[3]=1.0f;
	// mScale_Matrix[4]=0.0f;
	// mScale_Matrix[5]=height;
	// mScale_Matrix[6]=0.0f;
	// mScale_Matrix[7]=1.0f;
	// mScale_Matrix[8]=0.0f;
	// mScale_Matrix[9]=0.0f;
	// mScale_Matrix[10]=0.0f;
	// mScale_Matrix[11]=1.0f;
	// mScale_Matrix[12]=x;
	// mScale_Matrix[13]=y;
	// mScale_Matrix[14]=0.0f;
	// mScale_Matrix[15]=1.0f;

	matrix = glm::make_mat4(mScale_Matrix);

}




void renderSprite(GLuint current_shader,
	float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3,
	const glm::vec4 & corrector_v,
	const GLuint * texture,
	float t_sprite_w,float t_sprite_h,
	float t_sprite_offset_x,
	float t_sprite_offset_y
	)
{


    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
             // R, G, B, A

			 1.0f, 0.0f, 0.0f, 0.0f,
			 0.0f, 1.0f,

			 0.0f, 1.0f, 0.0f, 0.0f,
			 1.0f, 1.0f,

			 0.0f, 0.0f, 1.0f, 0.0f,
			 0.0f, 0.0f,

			 0.0f, 0.0f, 0.0f, 1.0f,
			 1.0f, 0.0f
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
	}
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(current_shader);

	glm::mat4 texture_m = glm::mat4(1.0f);
	//texture_m = glm::scale(model_m,glm::vec3(1.0f,(float)width/height,1.0f));
	glm::mat4 position_m = glm::mat4(1.0f);
	Fit_Matrix(position_m,
		x0,y0,x1,y1,x2,y2,x3,y3
	);
	Fit_Tile_Matrix(texture_m,t_sprite_offset_x,t_sprite_offset_y,t_sprite_w,t_sprite_h);
	GLuint position_u  = glGetUniformLocation(current_shader, "DrawMatrix");
	glUniformMatrix4fv(position_u, 1, GL_FALSE, glm::value_ptr(position_m));

	GLuint texture_u  = glGetUniformLocation(current_shader, "SpriteMatrix");
	glUniformMatrix4fv(texture_u, 1, GL_FALSE, glm::value_ptr(texture_m));

	
	GLuint corrector_u  = glGetUniformLocation(current_shader, "corrector");
	glUniform4fv(corrector_u, 1, glm::value_ptr(corrector_v));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);

    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderSingleTriangle(GLuint current_shader, GLuint depthmap, 
	const glm::vec3 & p1, const glm::vec2 & t1,
	const glm::vec3 & p2, const glm::vec2 & t2,
	const glm::vec3 & p3, const glm::vec2 & t3,
	const glm::vec4 & corrector_v,
	GlScene::glCamera & camera,
	const GLuint * texture)
{
	unsigned int trisVAO = 0;
    unsigned int trisVBO;
	float trisVertices[] = {
            // positions        // texture Coords

			p1[0], p1[1], p1[2], 1.0f,
			t1[0], t1[1],

			p2[0], p2[1], p2[2], 1.0f,
			t2[0], t2[1],

			p3[0], p3[1], p3[2], 1.0f,
			t3[0], t3[1],

			p1[0], p1[1], p1[2], 1.0f,
			t1[0], t1[1]

        };

		// float trisVertices[] = {
        //     // positions        // texture Coords

		// 	0.0f, 0.0f, 0.0f, 1.0f,
		// 	t1[0], t1[1],

		// 	1.0f, 0.0f, 0.0f, 1.0f,
		// 	t2[0], t2[1],

		// 	0.0f, 1.0f, 0.0f, 1.0f,
		// 	t3[0], t3[1]

        // };
        // setup plane VAO
	glGenVertexArrays(1, &trisVAO);
	glGenBuffers(1, &trisVBO);
	glBindVertexArray(trisVAO);
	glBindBuffer(GL_ARRAY_BUFFER, trisVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(trisVertices), &trisVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(0); 

	glBindVertexArray(0);


	glUseProgram(current_shader);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glUniform1i(glGetUniformLocation(current_shader, "DepthMap"), 1);
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, depthmap);

	glActiveTexture(GL_TEXTURE0);
	
	GLuint corrector_u  = glGetUniformLocation(current_shader, "corrector");
	glUniform4fv(corrector_u, 1, glm::value_ptr(corrector_v));

	GLuint camera_u  = glGetUniformLocation(current_shader, "camera");
	glUniformMatrix4fv(camera_u, 1, GL_FALSE, glm::value_ptr(camera.CameraMatrix()));

    glBindVertexArray(trisVAO);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

    glBindVertexArray(0);
	
    glDeleteVertexArrays(1, &trisVAO);
	glDeleteBuffers(1, &trisVBO);


}


void renderSpriteDepth(GLuint current_shader, GLuint depthmap, float sprite_depth,
	float x0,float y0,float x1,float y1,float x2,float y2,float x3,float y3,
	const glm::vec4 & corrector_v,
	const GLuint * texture 
)
{


    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
             // R, G, B, A

			 1.0f, 0.0f, 0.0f, 0.0f,
			 0.0f, 1.0f,

			 0.0f, 1.0f, 0.0f, 0.0f,
			 1.0f, 1.0f,

			 0.0f, 0.0f, 1.0f, 0.0f,
			 0.0f, 0.0f,

			 0.0f, 0.0f, 0.0f, 1.0f,
			 1.0f, 0.0f
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(4 * sizeof(float)));
	}
	
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(current_shader);

	glm::mat4 texture_m = glm::mat4(1.0f);
	//texture_m = glm::scale(model_m,glm::vec3(1.0f,(float)width/height,1.0f));
	glm::mat4 position_m = glm::mat4(1.0f);
	Fit_Matrix(position_m,
		x0,y0,x1,y1,x2,y2,x3,y3
	);
	GLuint position_u  = glGetUniformLocation(current_shader, "DrawMatrix");
	glUniformMatrix4fv(position_u, 1, GL_FALSE, glm::value_ptr(position_m));

	GLuint texture_u  = glGetUniformLocation(current_shader, "SpriteMatrix");
	glUniformMatrix4fv(texture_u, 1, GL_FALSE, glm::value_ptr(texture_m));

	
	GLuint corrector_u  = glGetUniformLocation(current_shader, "corrector");
	glUniform4fv(corrector_u, 1, glm::value_ptr(corrector_v));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *texture);

	glUniform1i(glGetUniformLocation(current_shader, "DepthMap"), 1);
	glActiveTexture(GL_TEXTURE0+1);
	glBindTexture(GL_TEXTURE_2D, depthmap);

	glActiveTexture(GL_TEXTURE0);


	GLint shader_depth = glGetUniformLocation(current_shader, "Depth");
   	glUniform1f(shader_depth, sprite_depth);


    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void renderBillBoardDepth(GLuint current_shader, GLuint depthmap,const GLuint * texture,
						 float width, float height,const glm::vec4 & corrector,
						 const glm::vec3 & position, const glm::vec3 & offset, 
						 GlScene::glCamera & camera)
{
	glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        

        const glm::mat4 m_camera = camera.CameraMatrix();
        const glm::mat4 m_projection = camera.CameraProjectionMatrix();
        

        

        glm::vec3 vector3d = position - offset;
        glm::vec4 BillboardPos_worldspace(vector3d.x,vector3d.y,vector3d.z, 1.0f);
        glm::vec4 BillboardPos_screenspace = m_camera * BillboardPos_worldspace;
        BillboardPos_screenspace /= BillboardPos_screenspace.w;

        
        
        float z = BillboardPos_screenspace.z *0.5f + 0.5f;
        
        
        float radius_screen_x = width * m_projection[0].x;
        float radius_screen_y = height * m_projection[1].y;

        if (z <= 0.0f){
            // Object is behind the camera, don't display it.
        }
        else
        {

            float scaler = (BillboardPos_screenspace.z + m_projection[2].z)/m_projection[3].z;
            //std::cout<<BillboardPos_screenspace.w<<" 2 "<<scaler<<"\n";
            

            radius_screen_x *= scaler;
            radius_screen_y *= scaler;
            renderSpriteDepth(current_shader,depthmap, z,
                    BillboardPos_screenspace.x-radius_screen_x,BillboardPos_screenspace.y-radius_screen_y,
                    BillboardPos_screenspace.x-radius_screen_x,BillboardPos_screenspace.y+radius_screen_y,
                    BillboardPos_screenspace.x+radius_screen_x,BillboardPos_screenspace.y+radius_screen_y,
                    BillboardPos_screenspace.x+radius_screen_x,BillboardPos_screenspace.y-radius_screen_y,
                    corrector, texture);
        }
        

        glDisable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
}

void renderQuad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderHeightMap()
{
	static unsigned int quadVAO = 0;
    static unsigned int quadVBO;
    static unsigned int quadIBO;
	static GLsizei vert_count = 0;

    if (quadVAO == 0)
    {
        // float quadVertices[] = {
        //     // positions        // texture Coords
        //      1.0f,  0.0f, -1.0f, 
        //     -1.0f,  0.0f, 1.0f, 
        //      1.0f,  0.0f, 1.0f, 
        //      0.5f, -0.5f, 0.0f, 
        // };
		std::vector<float> quadVertices;

		size_t map_vertex_size = 100;
		size_t map_size = map_vertex_size - 1;
		const float tile_size = 1.0f;
		float offset = 0.5f * tile_size * map_size;

		for(size_t i_z = 0; i_z < map_vertex_size; i_z++)
		{
			for(size_t i_x = 0; i_x < map_vertex_size;i_x++)
			{
				quadVertices.push_back(-offset + tile_size * i_x);
				quadVertices.push_back(0.0f);
				quadVertices.push_back(offset - tile_size * i_z);
			}
		}

	 	std::vector<unsigned int> indices;
		
		for(size_t i_z = 0; i_z < map_size; i_z++)
		{
			for(size_t i_x = 0; i_x < map_size;i_x++)
			{
				size_t current = i_z * map_vertex_size + i_x;
				indices.push_back(current + map_vertex_size);
				indices.push_back(current);
				indices.push_back(current + 1);

				indices.push_back(current + 1);
				indices.push_back(current + map_vertex_size + 1);
				indices.push_back(current + map_vertex_size);

			}
		}
		vert_count =  indices.size();
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
		glGenBuffers(1, &quadIBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*quadVertices.size(), quadVertices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadIBO);
	    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
		
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }
	
    glBindVertexArray(quadVAO);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
	glDrawElements(
     GL_TRIANGLES,      // mode
     vert_count,    // count
     GL_UNSIGNED_INT,   // type
     0           // element array buffer offset
 	);
    glBindVertexArray(0);
}


std::istream& operator>> ( std::istream& is, glm::mat4& mat)
{
	float marray[16];
	//for(int i = 0; i< 16; i++)
	//is >> marray[i];
	is >> marray[0]	>> marray[4]>> marray[8]>> marray[12]
	>> marray[1]	>> marray[5]>> marray[9]>> marray[13]
	>> marray[2]	>> marray[6]>> marray[10]>> marray[14]
	>> marray[3]	>> marray[7]>> marray[11]>> marray[15]	;
	mat = glm::make_mat4(marray);
	return is;
}

std::ostream& operator << ( std::ostream& os, const glm::mat4 & mat)
{
	const float *p_mat = static_cast<const float*> (glm::value_ptr(mat));
	os 	 << p_mat[0] 	<<" "<< p_mat[4]<<" "<< p_mat[8]	<<" "<< p_mat[12]
	<<" "<< p_mat[1]	<<" "<< p_mat[5]<<" "<< p_mat[9] 	<<" "<< p_mat[13]
	<<" "<< p_mat[2]	<<" "<< p_mat[6]<<" "<< p_mat[10]	<<" "<< p_mat[14]
	<<" "<< p_mat[3]	<<" "<< p_mat[7]<<" "<< p_mat[11]	<<" "<< p_mat[15];

	return os;
}


std::istream& operator >> ( std::istream& is, Bone& bone)
{
	is >> bone.name >> bone.parent >> bone.matrix;
    return is;
}


std::string readShaderFile(std::string file_name)
{
	std::string ShaderString = "";
	std::ifstream shaderFile;
	shaderFile.open(file_name);
	if (!shaderFile)
        throw std::runtime_error("Could not open file "+ file_name);
		
	while(!shaderFile.eof())
	{
		std::string tempholder;
		getline(shaderFile, tempholder);
		ShaderString.append(tempholder);
		ShaderString.append("\n");
	}
	shaderFile.close();

	return ShaderString;
}

GLuint LoadShader(std::string FileName,GLenum shaderType)
{
	std::cout<<FileName<<"\n";
	std::string shader_text = readShaderFile(FileName);
	GLchar const * ShaderSource = shader_text.c_str();

    GLuint Shader = glCreateShader(shaderType);
    glShaderSource(Shader, 1, &ShaderSource, NULL);
    glCompileShader(Shader);
    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(Shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::"
		<<"COMPILATION_FAILED\n"
		<< infoLog << std::endl;
		return -1;
    }
	return Shader;
}

GLuint LoadshaderProgram(std::string FileNameVS,std::string FileNameFS)
{
	GLuint Shaders[2];
    /*GLuint vertexShader*/
	Shaders[0] = LoadShader(FileNameVS,GL_VERTEX_SHADER);
    /*GLuint fragmentShader*/
	Shaders[1] =  LoadShader(FileNameFS,GL_FRAGMENT_SHADER);//glCreateShader(GL_FRAGMENT_SHADER);
    // Link shaders
    GLuint shaderProgram = LinkShaderProgram(Shaders,2);
    EmptyShaders(Shaders,2);
	return shaderProgram;
}

GLuint LoadshaderProgram(std::string FileNameVS,std::string FileNameFS,std::string FileNameGS)
{
	GLuint Shaders[3];
    /*GLuint vertexShader*/
	Shaders[0] = LoadShader(FileNameVS,GL_VERTEX_SHADER);
    /*GLuint fragmentShader*/
	Shaders[1] =  LoadShader(FileNameGS,GL_GEOMETRY_SHADER );//glCreateShader(GL_FRAGMENT_SHADER);
	Shaders[2] =  LoadShader(FileNameFS,GL_FRAGMENT_SHADER);//glCreateShader(GL_FRAGMENT_SHADER);
    // Link shaders
    GLuint shaderProgram = LinkShaderProgram(Shaders,3);
    EmptyShaders(Shaders,3);
	return shaderProgram;
}

glm::vec3 CalculateTangent(const std::array<glm::vec3 , 3> &pos,const std::array<glm::vec2 , 3> &uv)
{
	glm::vec3 edge1 = pos[1] - pos[0];
	glm::vec3 edge2 = pos[2] - pos[0];
	glm::vec2 deltaUV1 = uv[1] - uv[0];
	glm::vec2 deltaUV2 = uv[2] - uv[0];

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	glm::vec3 tangent = glm::vec3(f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
									f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
									f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z));


	return  glm::normalize(tangent);
}

void LoadVertexArray(std::string file_name,GLuint &VBO, GLuint &VBO_BONES, GLuint &VBO_BONES_IDX, int &vertex_count)
{

	std::string tmp_string = "";
	std::ifstream ModelFile;
	ModelFile.open(file_name);
	if (!ModelFile)
        throw std::runtime_error("Could not open file "+file_name);
	if(ModelFile.is_open())
	{
		//ModelFile >> tmp_string;
		getline(ModelFile, tmp_string);
		//std::cout << tmp_string;
		getline(ModelFile, tmp_string);
		//std::cout << tmp_string;
		ModelFile >> tmp_string;
		//std::cout << tmp_string;
		ModelFile >> vertex_count;

		int faces_count = vertex_count;
		vertex_count *= 3;
		//std::cout << vertex_count;
		//std::cout << "\n";
		getline(ModelFile, tmp_string);
		getline(ModelFile, tmp_string);
		//std::cout << tmp_string<< "\n";

		GLfloat * vertices = new GLfloat[vertex_count*11];
		GLfloat * bone_weight = new GLfloat[vertex_count*4];
		GLint * bone_indexes = new GLint[vertex_count*4];
		int i_v = 0;

		std::array<glm::vec3 , 3> pos;
		std::array<glm::vec2 , 3> uv;

		for(int face_i =0; face_i<faces_count; face_i++)
		{
			for(int i =0; i<3; i++)
			{
				
				for(int ip = 0; ip < 8; ip++)
				{
					ModelFile >> vertices[ip + i_v * 11];
				}
				pos[i] = glm::vec3(vertices[i_v * 11 + 0],  vertices[i_v * 11 + 1], vertices[i_v * 11 + 2]);
				uv[i]  = glm::vec2(vertices[i_v * 11 + 6],  vertices[i_v * 11 + 7]);

				getline(ModelFile, tmp_string);

				getline(ModelFile, tmp_string);


				std::istringstream iss(tmp_string);

				iss.width(10);
				iss.precision(8);

				for(int ib = 0; ib < 4; ib++)
					{
						bone_indexes[i_v * 4 + ib] = 0;
						bone_weight[ib + i_v * 4] = 0.0f;
					}

				int ndx = 0;
				while (!(iss.eof()))
				{
					iss >> bone_indexes[i_v * 4 + ndx] >> bone_weight[(ndx) + i_v * 4];
					ndx++;
				}
				
				i_v++;
			}

			glm::vec3 tangent = CalculateTangent(pos,uv);
			//std::cout <<tangent.x <<":"<<tangent.y<<":"<<tangent.z<<"\n";

			for(int ip = 1; ip < 4; ip++)
			{
				int index = 8 + (i_v-ip) * 11;
				vertices[index] = tangent.x;
				vertices[index+1] = tangent.y;
				vertices[index+2] = tangent.z;
			}
		}

		ModelFile.close();





		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * 11* sizeof(GLfloat), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_BONES);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * 4* sizeof(GLfloat), bone_weight, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, VBO_BONES_IDX);
		glBufferData(GL_ARRAY_BUFFER, vertex_count * 4* sizeof(GLint), bone_indexes, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		delete [] vertices;
		delete [] bone_weight;
		delete [] bone_indexes;
	}
}



glm::mat4 LoadMatrix(std::ifstream &source)
{
	glm::mat4 mat;
	if(source.is_open()) source >> mat;
	return  mat;
}

Bone LoadBone(std::ifstream &source)
{
	Bone bone;
	if(source.is_open()) source >> bone;
	return  bone;
}


void LoadBonesArray(std::string file_name,std::vector <Bone> &bones,int &bones_count)
{
	std::string tmp_string = "";
	std::ifstream ModelFile;
	ModelFile.open(file_name);
	if (!ModelFile)
        throw std::runtime_error("Could not open file " + file_name);	
	getline(ModelFile, tmp_string);
	ModelFile>>tmp_string>>bones_count>>tmp_string;

	Bone bone;
	bones.clear();

	for(int i = 0; i < bones_count; i++)
	{
		ModelFile>>bone;
		bones.push_back(bone);
	}
	ModelFile.close();
}
GLuint LinkShaderProgram(GLuint * shaders, int shaders_count)
{
	GLuint shaderProgram = glCreateProgram();
	for(int i = 0; i <shaders_count; i++) glAttachShader(shaderProgram, shaders[i]);

    glLinkProgram(shaderProgram);
    // Check for linking errors
	GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		return -1;
    }
	return shaderProgram;
}

void EmptyShaders(GLuint * shaders, int shaders_count)
{
	for(int i = 0; i <shaders_count; i++) glDeleteShader(shaders[i]);
}

void LoadTexture(std::string FileName,GLuint &texture)
{
	std::string extention = FileName.substr(FileName.find_last_of(".")+1);
	if(extention == "cub")
	{
		LoadCubemap(FileName,texture);
		return;
	}

	if(extention == "tex")
	{
		std::ifstream tex_file;
		tex_file.open(FileName);
		if (!tex_file)
        	throw std::runtime_error("Could not open file");
	}
	glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture); // All upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT (usually basic wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Load image, create texture and generate mipmaps
    int tex_width, tex_height;
    unsigned char* image = SOIL_load_image(FileName.c_str(), &tex_width, &tex_height, 0, SOIL_LOAD_RGBA);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned char * LoadHeightMap(std::string FileName,int * tex_width, int * tex_height)
{
	std::string extention = FileName.substr(FileName.find_last_of(".")+1);

	// if(extention == "tex")
	// {
	// 	std::ifstream tex_file;
	// 	tex_file.open(FileName);
	// 	if (!tex_file)
    //     	throw std::runtime_error("Could not open file");
	// }
    unsigned char* image = SOIL_load_image(FileName.c_str(), tex_width, tex_height, 0, SOIL_LOAD_RGBA);
    //SOIL_free_image_data(image);
	return image;
}

void DeleteHeightMap(unsigned char * image)
{
	SOIL_free_image_data(image);
}

void LoadCubemap(const std::string file_name,GLuint &texture)
{
	size_t pos = file_name.rfind("/");
	
	/*std::ifstream modelfile;
	modelfile.open(FileName);
	if (!modelfile)
        throw std::runtime_error("Could not open file");*/
	std::string path = pos == std::string::npos ? "" : file_name;
	if(pos != std::string::npos)
	{
		path.erase(pos+1,std::string::npos);
	}
	std::string tmp_string = "";
	std::vector<std::string> faces;
	std::ifstream texture_descriptor_file;
	texture_descriptor_file.open(file_name);
	if (!texture_descriptor_file)
        throw std::runtime_error("Could not open file " + file_name);
	while(faces.size()<6 && !texture_descriptor_file.eof())
	{
		getline(texture_descriptor_file, tmp_string);
		faces.push_back(tmp_string);
	}
	if(faces.size()<6)
	{
		std::cout << "Wrong cubemap descriptor: " << file_name;
	}

	texture_descriptor_file.close();


	glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_CUBE_MAP, texture); 

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);  

    int tex_width, tex_height;

 	for (unsigned int i = 0; i < faces.size(); i++)
    {
		std::string file = path+faces[i];
        unsigned char *data = SOIL_load_image(file.c_str(), &tex_width, &tex_height, 0, SOIL_LOAD_RGBA);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data
            );
            SOIL_free_image_data(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << file << std::endl;
            SOIL_free_image_data(data);
        }
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Animation::Animation(const std::string &file_name)
{
	LoadAnimation(file_name);
}


void Animation::LoadAnimation(const std::string &file_name, std::vector <Bone> &bones)
{
	int bon_count = 0;
	std::string tmp_string = "";
	std::ifstream ModelFile;
	ModelFile.open(file_name);
	if (!ModelFile)
        throw std::runtime_error("Could not open file " + file_name);
	getline(ModelFile, tmp_string);
	ModelFile>>tmp_string>>bon_count>>tmp_string>>tmp_string>>framescount;
	frames.clear();
	//vector<AnimationFrame>().swap( frames );
	int current_frame;
	glm::mat4 tmp_matrix;
	for(size_t i_frame = 0; i_frame < framescount; i_frame++)
	{
		ModelFile>>tmp_string>>current_frame;
		AnimationFrame a_frame;
		for(decltype(bon_count) i = 0; i < bon_count; i++)
		{

			ModelFile>>tmp_string>>tmp_matrix;
			//std::cout<<tmp_string<<"\n";
			a_frame.bones.push_back( tmp_matrix  * glm::inverse(bones[i].matrix));

		}
		frames.push_back(a_frame);
	}
	ModelFile.close();
	m_precalculated = true;	
}

void Animation::LoadAnimation(const std::string & file_name)
{
	size_t bon_count = 0;
	std::string tmp_string = "";
	std::ifstream ModelFile;
	ModelFile.open(file_name);
	if (!ModelFile)
        throw std::runtime_error("Could not open file "+ file_name);
	getline(ModelFile, tmp_string);
	ModelFile>>tmp_string>>bon_count>>tmp_string>>tmp_string>>framescount;
	frames.clear();
	int current_frame;
	glm::mat4 tmp_matrix;
	for(size_t i_frame = 0; i_frame < framescount; i_frame++)
	{
		ModelFile>>tmp_string>>current_frame;
		AnimationFrame a_frame;
		for(decltype(bon_count) i = 0; i < bon_count; i++)
		{

			ModelFile>>tmp_string>>tmp_matrix;
			a_frame.bones.push_back( tmp_matrix);

		}
		frames.push_back(a_frame);
	}
	ModelFile.close();
	m_cashe_animation.bones.resize(bon_count);
	m_precalculated = false;
}

GLfloat * Animation::GetDrawValues(size_t frame,const std::vector <Bone> &bones)
{
	size_t size = frames.size();
	frame = frame < size ? frame : size - 1; 

	if(m_precalculated) 
	{
		return glm::value_ptr(frames[frame].bones[0]);
	}

	CalculateCache(bones, frame);
	return glm::value_ptr(m_cashe_animation.bones[0]);
}

const glm::mat4 & Animation::GetBoneMatrix(size_t frame,size_t bone,const std::vector <Bone> &bones)
{
	size_t size = frames.size();
	frame = frame < size ? frame : size - 1; 

	if(m_precalculated) 
	{
		return frames[frame].bones[bone];
	}

	CalculateCache(bones, frame);
	return m_cashe_animation.bones[bone];
}


void Animation::CalculateCache(const std::vector <Bone> &bones,size_t frame)
{
	size_t size = frames.size();
	frame = frame < size ? frame : size - 1; 

	float approx = 1.0f;
	if(m_cache_frame == frame) 
		return;
	size_t bon_count = m_cashe_animation.bones.size();
	m_cashe_animation.bones[0] =glm::mat4(1.0f);
	rotation_matrix = frame == 0 ? glm::mat4(1.0f) : glm::inverse(frames[frame - 1].bones[0] )*frames[frame].bones[0] ;
	glm::mat4 base = glm::inverse(frames[frame].bones[0] );
	//glm::mat4 rot = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	if(bon_count>1)
	{
		for(decltype(bon_count) i = 1; i < bon_count; i++)
		{
			//m_cashe_animation.bones[i] =  SlerpMatrix(m_cashe_animation.bones[i],base * frames[frame].bones[i] *  glm::inverse(bones[i].matrix),approx);
			m_cashe_animation.bones[i] =  base * frames[frame].bones[i] *  glm::inverse(bones[i].matrix);
		}
	}
	m_cache_frame = frame;
}

const glm::mat4 & Animation::GetRotationMatrix(size_t frame,const std::vector <Bone> &bones)
{
	CalculateCache(bones, frame);
	return rotation_matrix;
}


const glm::mat4 SlerpMatrix(const glm::mat4 & m1,const glm::mat4 & m2,float approximation)
{
	glm::vec4 transformComp1 = glm::vec4(m1[0][3],m1[1][3],m1[2][3],m1[3][3]);
	glm::vec4 transformComp2 = glm::vec4(m2[0][3],m2[1][3],m2[2][3],m2[3][3]);

	glm::vec4 finalTrans = (float)(1.0-approximation)*transformComp1+transformComp2*approximation;


	glm::quat firstQuat = glm::quat_cast(m1);
	glm::quat secondQuat = glm::quat_cast(m2);
	glm::quat finalQuat = glm::slerp(firstQuat, secondQuat, approximation);
	glm::mat4 return_matrix = glm::mat4_cast(finalQuat);

	return_matrix[0][3] = finalTrans.x;
	return_matrix[1][3] = finalTrans.y;
	return_matrix[2][3] = finalTrans.z;
	return_matrix[3][3] = finalTrans.w;
	return return_matrix;
}


//<size_t >

std::istream& operator>> (std::istream& is, glm::vec4& glm_vector)
{

	for (size_t i = 0; i < 4; i++)
	{
		is >> glm_vector[i];
	}
	return is;
}
std::istream& operator>> ( std::istream& is, glm::vec3 & glm_vector)
{
	
	//float tmp[3];
	for(size_t i =0; i<3; i++)
	{
		is>>glm_vector[i];
	}
	return is;
}


//void(std::ostream& os,)

std::ostream& operator << ( std::ostream& os, const glm::vec3 & glm_vector)
{
	//float tmp[3];
	const size_t max = 3;
	for(size_t i =0; i<max; i++)
	{
		os<<glm_vector[i];
		if(i!=max-1) os<<" ";
	}
	return os;
}

std::ostream& operator << ( std::ostream& os, const glm::vec2 & glm_vector)
{
	const size_t max = 2;
	for(size_t i =0; i<max; i++)
	{
		os<<glm_vector[i];
		if(i!=max-1) os<<" ";
	}
	return os;
}


std::ostream& operator << ( std::ostream& os, const glm::vec4 & glm_vector)
{
	const size_t max = 2;
	for(size_t i =0; i<max; i++)
	{
		os<<glm_vector[i];
		if(i!=max-1) os<<" ";
	}
	return os;
}