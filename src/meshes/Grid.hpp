#ifndef GRID_HPP
#define GRID_HPP

#include <filesystem>

class Grid
{
public:
    Grid() noexcept;
    ~Grid();

    bool load_from_file(const std::filesystem::path& fpath) noexcept; 
    void draw() const noexcept;

private:
    unsigned m_vao;
    unsigned m_vbo;
    unsigned m_ibo;
    unsigned m_indexCount;
};

#endif // !GRID_HPP