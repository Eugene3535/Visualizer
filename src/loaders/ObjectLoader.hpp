#ifndef OBJECT_LOADER_HPP
#define OBJECT_LOADER_HPP

#include <cstdint>
#include <filesystem>
#include <vector>

#include "Vertex.hpp"

class ObjectLoader
{
public:
	bool load_model_from_file(const std::filesystem::path& fpath, std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices) noexcept;
};

#endif // !OBJECT_LOADER_HPP