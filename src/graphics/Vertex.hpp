#ifndef VERTEX_HPP
#define VERTEX_HPP

#include <glm/glm.hpp>

class Vertex
{
public:
	Vertex() noexcept;
	Vertex(const glm::vec3& arg_position, const glm::vec2& arg_uv, const glm::vec3& arg_normal, const glm::vec4& arg_color) noexcept;

	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	glm::vec4 color;
};

#endif // !VERTEX_HPP