#pragma once

#include <glm/glm.hpp>

namespace Shapes {
	struct S_Sphere {
	public:
		S_Sphere() = default;
		S_Sphere(const glm::vec3& position, float radius);

		bool IsColliding(const S_Sphere& other) const;
		void Transform(const glm::mat4& matrix);

		glm::vec3	m_position;
		float		m_radius;
	};
}