#include <iostream>
#include "glm/gtx/string_cast.hpp"

#include "glcamera.h"
#include "glresourses.h"

constexpr float similar_edge = 0.01f;

namespace GlScene
{

    const glm::mat4 &glCamera::CameraMatrix() const
		{
			//full_matrix =  projection * view;
			//return projection * view;
			return full_matrix;
			
		}

		const glm::mat4 &glCamera::CameraViewMatrix() const
		{
			return view;
		}

		const glm::mat4 &glCamera::CameraProjectionMatrix() const
		{
			return projection;
		}

		const glm::vec4 &glCamera::GetFrustrumPoint(FrustrumPoints point) const
        {
            return Frustrum[static_cast<int>(point)];
        }
        
		void glCamera::RecalculateFrustrum()
        {
            glm::mat4 inverse = glm::inverse(full_matrix);

            Frustrum[static_cast<int>(FrustrumPoints::NearLD)] = inverse * glm::vec4(-1.0f,-1.0f,-1.0f,1.0f);
	        Frustrum[static_cast<int>(FrustrumPoints::NearLU)] = inverse * glm::vec4(-1.0f,1.0f,-1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::NearRU)] = inverse * glm::vec4(1.0f,1.0f,-1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::NearRD)] = inverse * glm::vec4(1.0f,-1.0f,-1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarLD)] = inverse * glm::vec4(-1.0f,-1.0f,1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarLU)] = inverse * glm::vec4(-1.0f,1.0f,1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarRU)] = inverse * glm::vec4(1.0f,1.0f,1.0f,1.0f);
            Frustrum[static_cast<int>(FrustrumPoints::FarRD)] = inverse * glm::vec4(1.0f,-1.0f,1.0f,1.0f);


			for(int i = 0; i < 8; i++)
			{
				Frustrum[i]/=(Frustrum[i])[3];
			}
			
			Frustrum_2d.clear();
			Frustrum_2d.push_back(VectorToPlane(m_position));
			Frustrum_2d.push_back(VectorToPlane(Frustrum[static_cast<int>(FrustrumPoints::FarRU)]));
			Frustrum_2d.push_back(VectorToPlane(Frustrum[static_cast<int>(FrustrumPoints::FarRD)]));

			float lu = glm::dot(Frustrum_2d[1],m_map_direction);
			float ld = glm::dot(Frustrum_2d[2],m_map_direction);

			if(lu*ld< 0)
			{
				Frustrum_2d[0] = Frustrum_2d[1];
				Frustrum_2d[1] = Frustrum_2d[2];
				Frustrum_2d[2] = (VectorToPlane(Frustrum[static_cast<int>(FrustrumPoints::FarLD)]));
				Frustrum_2d.push_back(VectorToPlane(Frustrum[static_cast<int>(FrustrumPoints::FarLU)]));

				if(lu > ld)
				{
					std::swap(Frustrum_2d[0],Frustrum_2d[1]);
					std::swap(Frustrum_2d[2],Frustrum_2d[3]);
				}

			}
			else
			{
				if(abs(lu-ld) < similar_edge)
				{
					Frustrum_2d[2] = VectorToPlane(Frustrum[static_cast<int>(FrustrumPoints::FarLU)]);
				}
				else
				{
					Frustrum_2d.push_back(VectorToPlane(Frustrum[static_cast<int>(FrustrumPoints::FarLD)]));
					Frustrum_2d.push_back(VectorToPlane(Frustrum[static_cast<int>(FrustrumPoints::FarLU)]));

					if(lu > ld)
					{
						std::swap(Frustrum_2d[1],Frustrum_2d[2]);
						std::swap(Frustrum_2d[3],Frustrum_2d[4]);
					}
				}
			}


			// std::cout<<"\n<frustrum_dir>\n{\n";
			// std::cout<<m_map_direction<<"\n<frustrum>\n";

			// for(int i = 0; i < Frustrum_2d.size(); i++)
			// {
			// 	std::cout<<Frustrum_2d[i]<<"\n";
			// }
			// std::cout<<"\n}\n<frustrum>\n";
        }

		const std::vector<glm::vec2> &glCamera::GetFrustrum2d() const
		{
			return Frustrum_2d;
		}

        

		void glCamera::SetCameraLocation(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
		{
			glm::vec3 direction = target - position;
			direction[1]= 0;
			direction = glm::normalize(direction);
			m_map_direction = VectorToPlane(direction);

			m_position = position;
			view = glm::lookAt(position,target,up);
			full_matrix =  projection * view;
			RecalculateFrustrum();
		}

		void glCamera::SetCameraLens(float FOV, float aspect, float near, float far)
		{
			projection = glm::perspective(glm::radians(FOV), aspect, near, far);
			full_matrix =  projection * view;
		}

		void glCamera::SetCameraLens_Orto(float x1, float x2,float y1, float y2, float near, float far)
		{
			projection = glm::ortho(x1, x2, y1, y2, near, far);
			full_matrix =  projection * view;
		}

}
