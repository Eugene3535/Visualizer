#ifndef TEXTURED_MODEL_HPP
#define TEXTURED_MODEL_HPP

#include <filesystem>
#include <vector>

#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "graphics/Vertex.hpp"

class ObjectModel final
{
public:
	ObjectModel() noexcept;
	~ObjectModel();

	bool load_from_file(const std::filesystem::path& fpath) noexcept; 
	void draw() noexcept;

private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ibo;
	GLint  m_indexCount;
};

#endif // !TEXTURED_MODEL_HPP