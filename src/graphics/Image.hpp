#ifndef IMAGE_HPP
#define IMAGE_HPP

#include <filesystem>
#include <vector>

class Image
{
public:
    Image() noexcept;
    ~Image();

    bool loadFromFile(const std::filesystem::path& fPath) noexcept;

    const std::uint8_t* pixels() const noexcept;
    std::int32_t width()  const noexcept;
    std::int32_t height() const noexcept;

private:
    std::vector<std::uint8_t> m_pixels;
    std::int32_t m_width;
    std::int32_t m_height;
};

#endif // !IMAGE_HPP