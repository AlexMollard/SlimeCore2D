#include <glew.h>
#include <iostream>

class RenderTarget
{
public:
    RenderTarget(unsigned int width, unsigned int height);

    ~RenderTarget();

    void Bind();

    void Unbind();

    unsigned int GetID() const;
    unsigned int GetTextureID() const;

private:
    unsigned int m_fbo = 0;
    unsigned int m_texture = 0;
    unsigned int m_rbo = 0;

    unsigned int m_width = 0;
    unsigned int m_height = 0;

    void Create();

    void Destroy();
};