#pragma once
#include "media/RTMPInput.h"

class RTMPInput;
class AVStreamRender {

  public:
    AVStreamRender();
    ~AVStreamRender();

    void AddInput(const std::string &url);

    void Init();
    void Draw();

  private:
    unsigned int texture_ = -1;
    unsigned int vao_ = 0;
    unsigned int vbo_ = 0;
    unsigned int ebo_ = 0;

    RTMPInput *input_{nullptr};
};