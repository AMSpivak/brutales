#ifndef GL_ENGINE_CAMERA
#define GL_ENGINE_CAMERA

#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace GlScene
{
	enum class FrustrumPoints { NearLD = 0, NearLU, NearRU, NearRD, FarLD, FarLU, FarRU, FarRD, FirstLD, FirstLU, FirstRU, FirstRD, FrustrumPointsCount};
	enum class FrustrumNormals {Near = 0, Far, Up, Down, Left, Right};
	class glCamera
	{
		glm::mat4 view;
		glm::mat4 projection;
		glm::mat4 full_matrix;
		glm::vec4 Frustrum[static_cast<unsigned int>(FrustrumPoints::FrustrumPointsCount)];
		glm::vec3 Normals[6];
		glm::vec2 m_map_direction;
		std::vector<glm::vec2> Frustrum_2d;

	public:
		glm::vec3 m_position;

		const glm::mat4 &CameraMatrix() const;
		const glm::mat4 &CameraViewMatrix() const;
		const glm::mat4 &CameraProjectionMatrix() const;
		const glm::vec4 &GetFrustrumPoint(FrustrumPoints point) const;
		const glm::vec3 &GetFrustrumNormal(FrustrumNormals normal) const;
		const std::vector<glm::vec2> &GetFrustrum2d() const;
		virtual void RecalculateFrustrum();
		void SetCameraLocation(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up);
		void SetCameraLens(float FOV, float aspect, float near, float far);
		void SetCameraLens_Orto(float x1, float x2,float y1, float y2, float near, float far);
	};
}

#endif