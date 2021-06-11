#include "AVStreamRender.h"
#include <glad/glad.h>

AVStreamRender::AVStreamRender() {}

AVStreamRender::~AVStreamRender() {}

void AVStreamRender::AddInput(RTMPInput *input) { input_ = input; }

void AVStreamRender::Init(float x, float y) {
    glGenTextures(1, &texture_);

    //      --位置--  --纹理--
    float vertices[] = {// 右上
                        x + 1.0f, y, 0.0f, 1.0f, 1.0f,
                        // 右下
                        x + 1.0f, y + 1.0f, 0.0f, 1.0f, 0.0f,
                        // 左下
                        x, y + 1.0f, 0.0f, 0.0f, 0.0f,
                        // 左上
                        x, y, 0.0f, 0.0f, 1.0f};

    unsigned int indices[] = {
        // 注意索引从0开始!
        0, 1, 3, // 第一个三角形
        1, 2, 3  // 第二个三角形
    };

    //
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    // 初始化
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                          (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void AVStreamRender::Draw() {
    if (!input_) return;

    UpdateTexture();

    glBindVertexArray(vao_);

    // glDrawArrays(GL_TRIANGLES, 0, 3);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}

unsigned int AVStreamRender::GetTextureID() { return texture_; }

bool AVStreamRender::UpdateTexture() {
    AVFrame *frame = input_->BeginUsedFrame();
    if (!frame) return false;

    glActiveTexture(GL_TEXTURE0 + texture_);
    //     // 在绑定之前激活相应的纹理单元
    //     // 获取纹理序号（diffuse_textureN 中的 N）
    //     std::string number;
    //     std::string name = textures[i].type;
    //     if (name == "texture_diffuse")
    //       number = std::to_string(diffuseNr++);
    //     else if (name == "texture_specular")
    //       number = std::to_string(specularNr++);
    //
    //     shader.setFloat(("material." + name + number).c_str(), i);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame->width, frame->height, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 frame->data[0]);

    glGenerateMipmap(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);

    input_->EndUsedFrame(frame);
    return true;
}
