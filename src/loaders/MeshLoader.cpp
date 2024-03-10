#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

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















bool MeshLoader::load_model_from_file(const std::filesystem::path& fpath, std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices) noexcept
{
    auto split = [](const std::string& src, char delimeter)
    {
        std::string buffer;
        std::vector<std::string> result;

        for (std::size_t i = 0; i < src.size(); ++i)
        {
            char c = src[i];

            if (c != delimeter)
            {
                buffer += c;
            }
            else if (c == delimeter && !buffer.empty())
            { 
                result.push_back(buffer);
                buffer.clear();
            }
        }

        if (!buffer.empty())
        {
            result.push_back(buffer);
        }

        return result;
    };

    auto processVertexPos = [&split](const std::string& currentLine, std::vector<glm::vec3>& list)
    {
        std::vector<std::string> floats = split(currentLine, ' ');
        list.push_back(glm::vec3(stof(floats[1]), stof(floats[2]), stof(floats[3])));
    };

    auto processVertexUV = [&split](const std::string& currentLine, std::vector<glm::vec2>& list)
    {
        std::vector<std::string> floats = split(currentLine, ' ');
        list.push_back(glm::vec2(stof(floats[1]), stof(floats[2])));
    };

    auto processVertexNormal = [&split](const std::string& currentLine, std::vector<glm::vec3>& list)
    {
        std::vector<std::string> floats = split(currentLine, ' ');
        list.push_back(glm::vec3(stof(floats[1]), stof(floats[2]), stof(floats[3])));
    };

    auto processPolygonIndices = [&split](const std::string& currentLine, std::vector<std::uint32_t>& indices, std::vector<std::uint32_t>& uvIndices, std::vector<std::uint32_t>& normalIndices)
    {
        std::vector<std::string> faceTriples = split(currentLine, ' ');
        int vertIndex = 0;
        for (std::vector<std::string>::iterator i = faceTriples.begin(); i != faceTriples.end(); ++i)
        {
            if (vertIndex != 0)
            {
                std::vector<std::string> vertIndices = split(*i, '/');
                indices.push_back(stoi(vertIndices[0]) - 1);
                uvIndices.push_back(stoi(vertIndices[1]) - 1);
                normalIndices.push_back(stoi(vertIndices[2]) - 1);
            }
            vertIndex++;
        }
    };

    if (fpath.empty())
		return false;

	if(fpath.extension().generic_string() != ".obj")
		return false;
	
	std::fstream fs(fpath);

	if (!fs.is_open())
	{
		return false;
	}

	std::vector<glm::vec3> positions;
	std::vector<glm::vec2> unorderedUvs;
	std::vector<glm::vec3> unorderedNormals;

	std::vector<std::uint32_t> posIndices;
	std::vector<std::uint32_t> uvIndices;
	std::vector<std::uint32_t> normalIndices;

	std::vector<glm::vec2> orderedUvs;
	std::vector<glm::vec3> orderedNormals;
	std::vector<Vertex> resultVertices;

	std::string currentLine;

	while (std::getline(fs, currentLine))
	{
		if (currentLine.find("v ") == 0)
		{
			processVertexPos(currentLine, positions);
			continue;
		}

		if (currentLine.find("vt ") == 0)
		{
			processVertexUV(currentLine, unorderedUvs);
			continue;
		}

		if (currentLine.find("vn ") == 0)
		{
			processVertexNormal(currentLine, unorderedNormals);
			continue;
		}

		if (currentLine.find("f ") == 0)
		{
			processPolygonIndices(currentLine, posIndices, uvIndices, normalIndices);
			continue;
		}
	}
	fs.close();

	orderedUvs.resize(unorderedUvs.size());

	std::size_t itter = 0;
	for (auto i = uvIndices.begin(); i != uvIndices.end(); ++i)
	{
		orderedUvs[posIndices[itter]] = unorderedUvs[uvIndices[itter]];
		itter++;
	}

	orderedNormals.resize(unorderedNormals.size());

	itter = 0;
	for (auto i = normalIndices.begin(); i != normalIndices.end(); ++i)
	{
		orderedNormals[posIndices[itter]] = unorderedNormals[normalIndices[itter]];
		itter++;
	}

	itter = 0;
	for (auto i = positions.begin(); i != positions.end(); ++i)
	{
		resultVertices.push_back(Vertex(*i, orderedUvs[itter], orderedNormals[itter], {1,1,1,1}));
		itter++;
	}

	vertices.resize(resultVertices.size());
	indices.resize(posIndices.size());

	std::swap(resultVertices, vertices);
	std::swap(posIndices, indices);

	return true;
}