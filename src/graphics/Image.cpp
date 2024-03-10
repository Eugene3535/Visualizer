#include <iostream>
#include <algorithm>
#include <cctype>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "Image.hpp"

Image::Image() noexcept:
    m_width(0),
    m_height(0)
{
}

Image::~Image()
{
}

bool Image::loadFromFile(const std::filesystem::path& fPath) noexcept
{
    m_pixels.clear();

    std::int32_t bytePerPixel = 0;
    std::uint8_t* data = stbi_load(fPath.generic_string().c_str(), &m_width, &m_height, &bytePerPixel, STBI_rgb_alpha);

    if (!data)
        return false;

    m_pixels.resize(static_cast<size_t>((m_width * m_height) << 2));
    std::memcpy(m_pixels.data(), data, m_pixels.size());
    stbi_image_free(data);

    return true;
}

const std::uint8_t* Image::pixels() const noexcept
{
    return m_pixels.data();
}

std::int32_t Image::width() const noexcept
{
    return m_width;
}

std::int32_t Image::height() const noexcept
{
    return m_height;
}