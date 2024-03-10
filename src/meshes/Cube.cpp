#include "Meshes/Cube.hpp"

Cube::Cube() noexcept:
    m_vao(0),
    m_vbo(0),
    m_ibo(0),
    m_indexCount(0)
{
}

Cube::~Cube()
{
    if(m_vao) glDeleteVertexArrays(1, &m_vao);
	if(m_vbo) glDeleteBuffers(1, &m_vbo);
    if(m_ibo) glDeleteBuffers(1, &m_ibo);
}

void Cube::init(const glm::vec3& min_point, const glm::vec3& max_point, const glm::mat4& transform) noexcept
{
    GLuint vertexCount = 8;

    std::vector<float> vertices(vertexCount * 3, 0.0f);
    float* data = vertices.data();

    for (unsigned i = 0; i < cz; ++i)
    {
        for (unsigned j = 0; j < cx; ++j)
        {
            data[0] = static_cast<float>(j);
            data[2] = static_cast<float>(i);
            data += 3;
        }
    }

    std::vector<GLuint> indices;
    indices.reserve(cellCount * 6);

    for (GLuint i = 0; i < depth; ++i)
    {
        for (GLuint j = 0; j < width; ++j)
        {
            GLuint pos = i * cx + j;

            indices.push_back(pos);
            indices.push_back(pos + 1);
            indices.push_back(pos + cx);

            indices.push_back(pos + 1);
            indices.push_back(pos + cx);
            indices.push_back(pos + cx + 1);
        }
    }
    m_indexCount = static_cast<GLuint>(indices.size());

    glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ibo);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), indices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Cube::draw() const noexcept
{
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}