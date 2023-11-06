#include <glew.h>
#include <iostream>
#include "engine/CommonEnums.h"

class RenderTarget
{
public:
    RenderTarget(unsigned int width, unsigned int height, FlipPolicy flipPolicy = FlipPolicy::None);

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

    void Create(FlipPolicy flipPolicy);

    void Destroy();
};