#include "loaders/MeshLoader.hpp"

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
        char buffer[64]{};

        while (!feof(file_ptr))
        {
            if(fgets(buffer, sizeof(buffer), file_ptr) != nullptr)
            {
                char* token = strtok(buffer, " ");
                token = strtok(nullptr, " ");

                if (buffer[0] == 'v')
                {
                    auto& vertex = vertices.emplace_back();
                    int i = 0;

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