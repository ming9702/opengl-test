#pragma once
#include "media/RTMPInput.h"

class RTMPInput;
class AVStreamRender {

  public:
    AVStreamRender();
    ~AVStreamRender();

    void AddInput(RTMPInput *input);

    void Init(float x, float y);
    void Draw();
    unsigned int GetTextureID();

  private:
    bool UpdateTexture();

  private:
    unsigned int texture_ = -1;
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;

    RTMPInput *input_{nullptr};
};