#include "graphics/Vertex.hpp"

Vertex::Vertex() noexcept: 
    position(), 
    uv(), 
    normal(), 
    color(1.0f, 1.0f, 1.0f, 1.0f)  
{
}

Vertex::Vertex(const glm::vec3& arg_position, const glm::vec2& arg_uv, const glm::vec3& arg_normal, const glm::vec4& arg_color) noexcept: 
    position(arg_position), 
    uv(arg_uv), 
    normal(arg_normal), 
    color(arg_color) 
{
}