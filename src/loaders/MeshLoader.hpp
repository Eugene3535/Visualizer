#ifndef MESH_LOADER_HPP
#define MESH_LOADER_HPP

#include <filesystem>
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

class MeshLoader
{
public:
    bool create_plane(const std::filesystem::path& fpath, GLuint width, GLuint depth) noexcept;
	bool load_mesh_from_file(const std::filesystem::path& fpath, std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices) noexcept;
};

#endif // !MESH_LOADER_HPP