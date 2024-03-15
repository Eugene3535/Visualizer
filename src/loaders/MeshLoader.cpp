#include <iostream>
#include <string>
#include <array>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "loaders/MeshLoader.hpp"

#define STACK_BUFFER_SIZE 64

bool MeshLoader::create_plane(const std::filesystem::path& fpath, GLuint width, GLuint depth) noexcept
{
    GLuint cellCount = width * depth;
    GLuint vertexCount = cellCount << 2;

    GLuint cx = width + 1; 
    GLuint cz = depth + 1;

    std::vector<glm::vec3> vertices(vertexCount);
    glm::vec3* data = vertices.data();

    for (GLuint i = 0; i < cz; ++i)
    {
        for (GLuint j = 0; j < cx; ++j)
        {
            data->x = static_cast<float>(j);
            data->z = static_cast<float>(i);
            ++data;
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

    FILE* file_ptr = fopen(fpath.string().c_str(), "w");

    if(file_ptr)
    {
        for (const auto& vertex : vertices)  
            fprintf(file_ptr, "v %f %f %f\n", vertex.x, vertex.y, vertex.z);    

        const GLuint* data = indices.data();
        const GLuint size = static_cast<GLuint>(indices.size());

        for (GLuint i = 0; i < size; i += 6, data += 6)    
            fprintf(file_ptr, "i %u %u %u %u %u %u\n", data[0], data[1], data[2], data[3], data[4], data[5]);    
        
        fclose(file_ptr);

        return true;
    }

    return false;
}

bool MeshLoader::load_mesh_from_file(const std::filesystem::path& fpath, std::vector<glm::vec3>& vertices, std::vector<GLuint>& indices) noexcept
{
    FILE* file_ptr = fopen(fpath.string().c_str(), "r");

    if(file_ptr)
    {
        char buffer[STACK_BUFFER_SIZE]{};

        while(!feof(file_ptr))
        {
            if(fgets(buffer, sizeof(buffer), file_ptr) != nullptr)
            {
                char* token = strtok(buffer, " ");
                token = strtok(nullptr, " ");

                if (buffer[0] == 'v')
                {
                    auto& vertex = vertices.emplace_back();
                    std::int32_t i = 0;

                    while (token != nullptr)
                    {
                        vertex[i] = static_cast<float>(atof(token));
                        ++i;
                        token = strtok(nullptr, " ");
                    }
                }
                else if(buffer[0] == 'i')
                {
                    while (token != nullptr)
                    {
                        indices.push_back(strtol(token, NULL, 10));
                        token = strtok(nullptr, " ");
                    }
                }
            }
        }
           
        fclose(file_ptr);

        return true;
    }

    return false;
}

bool MeshLoader::load_model_from_file(const std::filesystem::path& fpath, std::vector<Vertex>& vertices, std::vector<GLuint>& indices) noexcept
{
    auto split = [](char* src, const char* delimeter)
    {
        std::array<const char*, 3> token_ptrs{};
        char* token = strtok(src, delimeter);
        token = strtok(nullptr, delimeter);
        std::size_t n = 0;

        while (token != nullptr)
        {
            token_ptrs[n] = token;
            ++n;
            token = strtok(nullptr, delimeter);
        }

        return token_ptrs;
    };

    auto process_vertices = [&split](char* current_line, std::vector<glm::vec3>& positions)
    {
        auto string_array = split(current_line, " ");

        float x = static_cast<float>(atof(string_array[0]));
        float y = static_cast<float>(atof(string_array[1]));
        float z = static_cast<float>(atof(string_array[2]));

        positions.emplace_back(x, y, z);
    };

    auto process_uv_coords = [&split](char* current_line, std::vector<glm::vec2>& tex_coords)
    {
        auto string_array = split(current_line, " ");

        float u = static_cast<float>(atof(string_array[0]));
        float v = static_cast<float>(atof(string_array[1]));

        tex_coords.emplace_back(u, v);
    };

    auto process_normals = [&split](char* current_line, std::vector<glm::vec3>& normals)
    {
        auto string_array = split(current_line, " ");

        float x = static_cast<float>(atof(string_array[0]));
        float y = static_cast<float>(atof(string_array[1]));
        float z = static_cast<float>(atof(string_array[2]));

        normals.emplace_back(x, y, z);
    };

    auto process_polygon_indices = [&split](char* current_line, std::vector<GLuint>& vertex_indices, std::vector<GLuint>& uv_indices, std::vector<GLuint>& normal_indices)
    {
        auto face_triples = split(current_line, " ");

        for(const char* ptr : face_triples)
        {
            char* face = const_cast<char*>(ptr);
            auto tokens = split(face, "/");

            GLuint index  = strtol(tokens[0], NULL, 10) - 1;
            GLuint uv     = strtol(tokens[0], NULL, 10) - 1;
            GLuint normal = strtol(tokens[0], NULL, 10) - 1;

            vertex_indices.push_back(index);
            uv_indices.push_back(uv);
            normal_indices.push_back(normal);
        }
    };

    std::vector<glm::vec3> positions;
	std::vector<glm::vec2> unordered_uvs;
	std::vector<glm::vec3> unordered_normals;

	std::vector<std::uint32_t> uv_indices;
	std::vector<std::uint32_t> normal_indices;

	std::vector<glm::vec2> ordered_uvs;
	std::vector<glm::vec3> ordered_normals;

    FILE* file_ptr = fopen(fpath.string().c_str(), "r");

    if(file_ptr)
    {
        char buffer[STACK_BUFFER_SIZE]{};

        while(!feof(file_ptr))
        {
            if(fgets(buffer, sizeof(buffer), file_ptr) != nullptr)
            {
                if (buffer[0] == 'v' && buffer[1] == ' ')
                {
                    process_vertices(buffer, positions);
                }
                else if (buffer[0] == 'v' && buffer[1] == 't')
                {
                    process_uv_coords(buffer, unordered_uvs);
                }
                else if (buffer[0] == 'v' && buffer[1] == 'n')
                {
                    process_normals(buffer, unordered_normals);
                }
                else if (buffer[0] == 'f' && buffer[1] == ' ')
                {
                    process_polygon_indices(buffer, indices, uv_indices, normal_indices);
                }
            }
        }    

        fclose(file_ptr);
    }

	ordered_uvs.resize(unordered_uvs.size());

	for (std::size_t i = 0; i < uv_indices.size(); ++i)
		ordered_uvs[indices[i]] = unordered_uvs[uv_indices[i]];
	
	ordered_normals.resize(unordered_normals.size());

	for (std::size_t i = 0; i < normal_indices.size(); ++i)
		ordered_normals[indices[i]] = unordered_normals[normal_indices[i]];
	
	for (std::size_t i = 0; i < positions.size(); ++i)
		vertices.emplace_back(positions[i], ordered_uvs[i], ordered_normals[i], glm::vec4(1.0f));

	return true;
}