#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>

#include "ObjectLoader.hpp"

static std::vector<std::string> split(const std::string& src, char delimeter)
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
}

static void processVertexPos(const std::string& currentLine, std::vector<glm::vec3>& list)
{
	std::vector<std::string> floats = split(currentLine, ' ');
	list.push_back(glm::vec3(stof(floats[1]), stof(floats[2]), stof(floats[3])));
}

static void processVertexUV(const std::string& currentLine, std::vector<glm::vec2>& list)
{
	std::vector<std::string> floats = split(currentLine, ' ');
	list.push_back(glm::vec2(stof(floats[1]), stof(floats[2])));
}

static void processVertexNormal(const std::string& currentLine, std::vector<glm::vec3>& list)
{
	std::vector<std::string> floats = split(currentLine, ' ');
	list.push_back(glm::vec3(stof(floats[1]), stof(floats[2]), stof(floats[3])));
}

static void processPolygonIndices(const std::string& currentLine, std::vector<std::uint32_t>& indices, std::vector<std::uint32_t>& uvIndices, std::vector<std::uint32_t>& normalIndices)
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
}

bool ObjectLoader::load_model_from_file(const std::filesystem::path& fpath, std::vector<Vertex>& vertices, std::vector<std::uint32_t>& indices) noexcept
{
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
