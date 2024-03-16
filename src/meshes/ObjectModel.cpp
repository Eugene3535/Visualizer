#include <glm/ext.hpp>

#include "loaders/MeshLoader.hpp"
#include "meshes/ObjectModel.hpp"

ObjectModel::ObjectModel() noexcept: 
	m_vao(0U),
	m_vbo(0U),
	m_ibo(0U),
	m_indexCount(0)
{
}

ObjectModel::~ObjectModel()
{
	if(m_vbo) glDeleteBuffers(1, &m_vbo);
	if(m_ibo) glDeleteBuffers(1, &m_ibo);
	if(m_vao) glDeleteVertexArrays(1, &m_vao);
}

bool ObjectModel::load_from_file(const std::filesystem::path& fpath) noexcept
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	if(!MeshLoader().load_model_from_file(fpath, vertices, indices))
		return false;

	m_indexCount = indices.size();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), static_cast<const void*>(indices.data()), GL_STATIC_DRAW);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), static_cast<const void*>(vertices.data()), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), (const void*)(3 * sizeof(GLfloat)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(Vertex), (const void*)(5 * sizeof(GLfloat)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, false, sizeof(Vertex), (const void*)(8 * sizeof(GLfloat)));

	return true;
}

void ObjectModel::draw() noexcept
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
