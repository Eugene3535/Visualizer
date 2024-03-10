#include <glad/glad.h>

#include "Texture2D.hpp"

Texture2D::Texture2D() noexcept:
    m_texture(0u),
    m_width(0),
    m_height(0),
    m_isSmooth(false),
    m_isRepeated(false)
{
}

Texture2D::~Texture2D()
{
    if(m_texture)
        glDeleteTextures(1, &m_texture);
}

bool Texture2D::loadFromFile(const std::filesystem::path& fPath) noexcept
{
    Image image;

    return image.loadFromFile(fPath) && loadFromImage(image);
}

bool Texture2D::loadFromImage(const Image& image) noexcept
{
    if(image.pixels() == nullptr)
        return false;

    m_width  = image.width();
    m_height = image.height();

	glGenTextures(1, &m_texture);
    Texture2D::bind(this);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.pixels());
    setSmooth(false);
    setRepeated(false);
	glGenerateMipmap(GL_TEXTURE_2D);
    Texture2D::bind(nullptr);

    return true;
}

void Texture2D::setSmooth(bool smooth) noexcept
{
    if(m_isSmooth != smooth)
    {
        m_isSmooth = smooth;
        Texture2D::bind(this);

        if (m_isSmooth)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }

        Texture2D::bind(nullptr);
    }
}

void Texture2D::setRepeated(bool repeate) noexcept
{
    if(m_isRepeated != repeate)
    {
        m_isRepeated = repeate;
        Texture2D::bind(this);

        if (m_isRepeated)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            const static float border_color[] { 0.0f, 0.0f, 0.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
        }

        Texture2D::bind(nullptr);
    }
}

bool Texture2D::isSmooth() const noexcept
{
    return m_isSmooth;
}

bool Texture2D::isRepeated() const noexcept
{
    return m_isRepeated;
}

unsigned Texture2D::getNativeHandle() const noexcept
{
    return m_texture;
}

int Texture2D::width() const noexcept
{
    return m_width;
}

int Texture2D::height() const noexcept
{
    return m_height;
}

void Texture2D::bind(const Texture2D* texture) noexcept
{
    if(texture)
        glBindTexture(GL_TEXTURE_2D, texture->m_texture);
    else
        glBindTexture(GL_TEXTURE_2D, 0);
}