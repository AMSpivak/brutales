#include "map_heightmap.h"
#include "glresourses.h"
#include "collision.h"
#include <GL/glew.h>
#include <GL/glfw3.h>
namespace GameMap
{

    HeightMap::~HeightMap()
    {
        if(m_heightmap != nullptr)
        {
            DeleteHeightMap(m_heightmap);
        }

        glDeleteVertexArrays(1, &quadVAO);
		glDeleteBuffers(1, &quadVBO);
		glDeleteBuffers(1, &quadIBO);
    }

    void HeightMap::CreateMap()
    {
        std::vector<float> quadVertices;

		size_t map_vertex_size = 41;
		size_t map_size = map_vertex_size - 1;
		const float tile_size = 1.0f;
        m_mesh_size = tile_size * map_size;        
		float offset = 0.5f * m_mesh_size;

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

    void HeightMap::LoadMap(std::string FileName)
    {
        m_heightmap = LoadHeightMap(GetResourceManager()->m_texture_atlas.GetResourceFolder() + FileName,&m_width, &m_height);
        
        m_heightmap_texture = GetResourceManager()->m_texture_atlas.Assign(FileName);

        m_material[0] = std::make_shared<GameResource::GlMaterial>("DesetdSand_albedo.png","DesetdSand_normal.png","DesetdSand_roughness.png");
        m_material[1] = std::make_shared<GameResource::GlMaterial>("Desert_Rock_albedo.png","Desert_Rock_normal.png","Desert_Rock_roughness.png");
    }

    void HeightMap::SetParameters(float map_scaler,float height_scaler)
    {
        m_map_scaler = map_scaler;
        m_height_scaler = height_scaler;
    }

    size_t inline MapOffset(size_t ix,size_t iy, size_t line_width, size_t lines_count, size_t pixel_size)
    {
        if(ix < 0) ix+= line_width;
        if(iy < 0) iy+= lines_count;

        ix %= line_width;
        iy %= lines_count;

        size_t offset = iy; 
        offset = (offset * line_width + ix);
        return offset * pixel_size;
    }
    
    float HeightMap::GetHeight(float x,float y)
    {
        if(m_heightmap == nullptr)
            return 0.0f;

        float fx =(x * m_map_scaler + 0.5f) * m_width;
        float fy =(y * m_map_scaler + 0.5f) * m_height;
         
        int ix = static_cast<int>(fx);
        int iy = static_cast<int>(fy);

        ix %= m_width;
        iy %= m_height;

        float off_x = fx - ix;
        float off_y = fy - iy;

        size_t  offset = MapOffset(ix,iy,m_width,m_height,4);    
        float h0 = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        offset = MapOffset(ix+1,iy,m_width,m_height,4);    
        float hx = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        offset = MapOffset(ix,iy+1,m_width,m_height,4);    
        float hy = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        offset = MapOffset(ix+1,iy+1,m_width,m_height,4);    
        float hxy = 0.00392157f * m_heightmap[offset] * m_height_scaler;
        float sx1 = h0 + off_x * (hx - h0);
        float sx2 = hy + off_x * (hxy - hy);

        return sx1 + off_y * (sx2 - sx1);
    }

    float HeightMap::GetMapScaler()
    {
        return m_map_scaler;
    }

    float HeightMap::GetHeightScaler()
    {
        return m_height_scaler;
    }

    const glm::vec3 HeightMap::GetMapSize()
    {
        return m_map_size;
    }
    
    void HeightMap::SetMapSize(const glm::vec3 &size)
    {
        m_map_size = size;
    }

    void HeightMap::Draw(GLuint current_shader,const glm::vec3 &position,const GlScene::glCamera &camera, size_t simplify)
    {
        // glm::vec3 c = camera.m_position;
        // c[1] = 0.0f;
        // glm::vec3 e1 = camera.GetFrustrumPoint(GlScene::FrustrumPoints::FarRD);
        // e1[1] = 0.0f;        
        // glm::vec3 e2 = camera.GetFrustrumPoint(GlScene::FrustrumPoints::FarLD);
        // e2[1] = 0.0f;
        

        glUseProgram(current_shader);
        const float tile_size = 1.0f;
        float inv = 1/tile_size;
        //glm::vec3 = glm::fract(position);
        float x = position[0]-trunc(position[0]);
        float z = position[2]-trunc(position[2]);

        float offset_x = x - (tile_size * round(x*inv)); 
        float offset_z = z - (tile_size * round(z*inv)); 

        glm::vec4 offset_position_vector = glm::vec4(-offset_x,-position[1],-offset_z,tile_size);
        glm::vec4 map_position_vector = glm::vec4(position[0],position[2],GetMapScaler(),GetHeightScaler());
        
        GLuint cameraLoc  = glGetUniformLocation(current_shader, "camera");
        glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(camera.CameraMatrix()));

        GLuint offset_position  = glGetUniformLocation(current_shader, "offset_position");
        glUniform4fv(offset_position, 1, glm::value_ptr(offset_position_vector));

        GLuint map_position  = glGetUniformLocation(current_shader, "map_position");
        glUniform4fv(map_position, 1, glm::value_ptr(map_position_vector));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_heightmap_texture->m_texture);

        m_material[0]->Assign(current_shader,1,"Albedo_0","Normal_0","Utility_0");
        m_material[1]->Assign(current_shader,4,"Albedo_1","Normal_1","Utility_1");

        
        if(quadVAO == 0)
        {
            CreateMap();
        }
        
        
        glBindVertexArray(quadVAO);
        // glDrawElements(GL_TRIANGLES,      // mode
        // vert_count,    // count
        // GL_UNSIGNED_INT,   // type
        // 0           // element array buffer offset
        // );

        int index_x =0; int index_z = 0;


        bool intersects = true;
        const std::vector<glm::vec2> &frustrum = camera.GetFrustrum2d();
        Collision::C_2d_BB bbox;
        bbox.size = glm::vec2(m_mesh_size,m_mesh_size);

        for(int i_y =-15; i_y<15; i_y++)
        {
            index_z = i_y;
            for(int i_x =-15; i_x<15; i_x++)
            {
                index_x = i_x;
                offset_position_vector = glm::vec4(-offset_x + m_mesh_size*index_x,-position[1],-offset_z - m_mesh_size*index_z,tile_size);

                if(frustrum.size()>1)
                {
                    bbox.center = glm::vec2(offset_position_vector[0],-offset_position_vector[2]);
                    intersects = Collision::_2d::Intersect(frustrum,bbox);
                    //std::cout<<"\nCollision: "<<bbox.center <<" result"<<intersects<<"\n";
                }
                if(intersects)
                {
                    glUniform4fv(offset_position, 1, glm::value_ptr(offset_position_vector));
                    glDrawElements(GL_TRIANGLES,vert_count,GL_UNSIGNED_INT,0);
                }
            }
        }



        glBindVertexArray(0);


    }
    
}