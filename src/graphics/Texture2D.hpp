#ifndef TEXTURE2D_HPP
#define TEXTURE2D_HPP

#include "graphics/Image.hpp"

class Texture2D
{
public:
	Texture2D() noexcept;
	~Texture2D();

	bool loadFromFile(const std::filesystem::path& fPath) noexcept;
	bool loadFromImage(const Image& image)                noexcept;

	void setSmooth(bool smooth)    noexcept;
	void setRepeated(bool repeate) noexcept;

	bool isSmooth()   const noexcept;
	bool isRepeated() const noexcept;

	unsigned getNativeHandle() const noexcept;
	int width()  const noexcept;
    int height() const noexcept;

	static void bind(const Texture2D* texture) noexcept;

private:
	unsigned m_texture;
	int m_width;
    int m_height;

	bool m_isSmooth;
	bool m_isRepeated;
};

#endif // !TEXTURE2D_HPP