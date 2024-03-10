#ifndef CUBE_HPP
#define CUBE_HPP

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class Cube
{
public:
    Cube() noexcept;
    ~Cube();

    bool init(const glm::vec3& min_point, const glm::vec3& max_point, const glm::mat4& transform) noexcept; 
    void draw() const noexcept;

private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ibo;
    GLuint m_indexCount;
};

#endif // !CUBE_HPP