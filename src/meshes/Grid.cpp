#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "loaders/MeshLoader.hpp"
#include "meshes/Grid.hpp"

Grid::Grid() noexcept:
    m_vao(0),
    m_vbo(0),
    m_ibo(0),
    m_index_count(0)
{
}

Grid::~Grid()
{
    if(m_vao) glDeleteVertexArrays(1, &m_vao);
	if(m_vbo) glDeleteBuffers(1, &m_vbo);
    if(m_ibo) glDeleteBuffers(1, &m_ibo);
}

bool Grid::load_from_file(const std::filesystem::path& fpath) noexcept
{
    std::vector<glm::vec3> vertices;
    std::vector<GLuint> indices;

    if(!MeshLoader().load_mesh_from_file(fpath, vertices, indices))
        return false;

    m_index_count = static_cast<GLuint>(indices.size());

    glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

    return true;
}

void Grid::draw() const noexcept
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}