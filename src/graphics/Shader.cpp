#include <iostream>
#include <string>
#include <cstdio>

#include <glad/glad.h>

#include "graphics/Shader.hpp"

Shader::Shader() noexcept: 
    m_program(0u)
{
    m_program = glCreateProgram();
}

Shader::~Shader()
{
    if(m_program)
        glDeleteProgram(m_program);
}

bool Shader::compile(const std::filesystem::path& fPath, unsigned type) noexcept
{
    if(!m_program)
        return false;

    if(fPath.empty())
        return false;

    std::string source = readShaderSourceFromFile(fPath);

    if(source.empty())
        return false;

    unsigned shader = compileShaderFromSource(source, type);

    if(!shader)
        return false;

    return linkToProgram(shader);
}

bool Shader::compile(const std::filesystem::path& vert, const std::filesystem::path& frag) noexcept
{
    return compile(vert, GL_VERTEX_SHADER) && compile(frag, GL_FRAGMENT_SHADER);
}

bool Shader::compile(const std::filesystem::path& vert, const std::filesystem::path& frag, const std::filesystem::path& geom) noexcept
{
    return compile(vert, GL_VERTEX_SHADER) && compile(frag, GL_FRAGMENT_SHADER) && compile(geom, GL_GEOMETRY_SHADER);
}

int Shader::getUniformLocation(const char* name) const noexcept
{
    if(!m_program)
        return -1;

    return glGetUniformLocation(m_program, name);
}

void Shader::bind(const Shader* shader) noexcept
{
    if(shader)
        glUseProgram(shader->m_program);
    else
        glUseProgram(0);
}

std::string Shader::readShaderSourceFromFile(const std::filesystem::path& fPath)
{
    std::string source;

    FILE* f = fopen(fPath.generic_string().c_str(), "r");

    if (!f)
    {
        std::cerr << "Error: shader file not succesfully read\n";

        return source;
    }

    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);

    source.resize(length + 1);

    fseek(f, 0, SEEK_SET);
    fread(source.data(), sizeof(char), length, f);
    fclose(f);

    return source;
}

unsigned Shader::compileShaderFromSource(const std::string& source, unsigned type)
{
    unsigned shader = glCreateShader(type);
    const char* c_str = source.c_str();

    glShaderSource(shader, 1, &c_str, 0);
    glCompileShader(shader);

    int success = 0;
    
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        char infoLog[1024]{};

        glGetShaderInfoLog(shader, 1024, NULL, infoLog);

        std::cerr << "Error: shader compilation error\n"
            << infoLog << "\n -- --------------------------------------------------- -- \n";

        return 0;
    }

    return shader;
}

bool Shader::linkToProgram(unsigned shader)
{
    glAttachShader(m_program, shader);
    glLinkProgram(m_program); 

    int success = 0;
    char infoLog[1024]{};
  
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(shader, 1024, NULL, infoLog);

        std::cerr << "Error: shader program link error\n"
            << infoLog << "\n -- --------------------------------------------------- -- \n";

        return false;
    }

    glDeleteShader(shader);

    return true;
}