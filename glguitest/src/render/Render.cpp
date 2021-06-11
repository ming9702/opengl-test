#include "Render.h"

#include <qmath.h>

#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QPixmap>
#include <QWindow>
#include <fstream>
#include <iostream>
#include <string>

Render::Render() : QOpenGLFunctions_3_2_Core() {}

Render::~Render() {}

void Render::initialize() {
    initializeOpenGLFunctions();

    mymodel_ = new Model{"res/Banana_OBJ/Banana.obj"};

    light_ = new Light;
    light_->Init();

    LoadShaderItem("shader/item.vs", "shader/item.fs");
    LoadShaderLight("shader/item.vs", "shader/light.fs");
    LoadShaderStream("shader/stream.vs", "shader/stream.fs");

    glEnable(GL_DEPTH_TEST);
    /*  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/

    // 纹理
    //   unsigned int texture;
    //   QImage pic("res/awesomeface.png");
    //   qDebug() << pic;
    //   pic.format();
    //   glGenTextures(1, &texture);
    //   glBindTexture(GL_TEXTURE_2D, texture);
    //   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pic.width(), pic.height(), 0,
    //   GL_BGRA,
    //                GL_UNSIGNED_BYTE, pic.bits());
    //   glGenerateMipmap(GL_TEXTURE_2D);
}

void Render::LoadShaderStream(const std::string &file_vs, const std::string &file_fs) {
    pro_stream_ = CreateShader(file_vs, file_fs);
}

void Render::LoadShaderItem(const std::string &file_vs, const std::string &file_fs) {
    pro_item_ = CreateShader(file_vs, file_fs);

    projection_ = pro_item_->uniformLocation("projection");
    view_ = pro_item_->uniformLocation("view");
    model_ = pro_item_->uniformLocation("model");
}

void Render::LoadShaderLight(const std::string &file_vs, const std::string &file_fs) {
    pro_light_ = CreateShader(file_vs, file_fs);
}

QOpenGLShaderProgram *Render::CreateShader(const std::string &file_vs, const std::string &file_fs) {
    std::ifstream in;
    std::istreambuf_iterator<char> beg(in), end;
    in.open(file_vs);
    std::string shader_v(beg, end);
    in.close();
    in.open(file_fs);
    std::string shader_f(beg, end);
    in.close();
    QOpenGLShaderProgram *shader_pro = new QOpenGLShaderProgram();
    shader_pro->addShaderFromSourceCode(QOpenGLShader::Vertex, shader_v.c_str());
    shader_pro->addShaderFromSourceCode(QOpenGLShader::Fragment, shader_f.c_str());
    shader_pro->link();
    return shader_pro;
}

void Render::render(QWindow *win, float a, float b, QVector3D pos) {
    static float offset = 0.01f;
    offset = -offset;
    pos.setY(pos.y() + offset);

    const qreal retinaScale = win->devicePixelRatio();
    int width = win->width();
    int height = width / 16 * 9;

    if (height > win->height()) {
        height = win->height();
        width = height / 9 * 16;
    }

    int x = (win->width() - width) / 2;
    int y = (win->height() - height) / 2;

    glViewport(x, y, width * retinaScale, height * retinaScale);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 mat_projection;
    mat_projection.perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
    QMatrix4x4 mat_view;
    QVector3D view_pos{0.0f, 0.0f, -3.0f};
    mat_view.translate(view_pos);

    pro_light_->bind();

    QMatrix4x4 mat_light_model;
    mat_light_model.translate(pos);
    mat_light_model.scale(0.5f);
    pro_light_->setUniformValue("model", mat_light_model);
    pro_light_->setUniformValue("projection", mat_projection);
    pro_light_->setUniformValue("view", mat_view);

    light_->Drow();
    pro_light_->release();

    //
    pro_item_->bind();
    pro_item_->setUniformValue("viewPos", view_pos);
    //
    pro_item_->setUniformValue("light.ambient", 0.2f, 0.2f, 0.2f);
    pro_item_->setUniformValue("light.diffuse", 0.5f, 0.5f, 0.5f);
    pro_item_->setUniformValue("light.specular", 1.0f, 1.0f, 1.0f);
    pro_item_->setUniformValue("light.position", pos);
    // item
    pro_item_->setUniformValue("material.ambient", 0);
    pro_item_->setUniformValue("material.diffuse", 1);
    pro_item_->setUniformValue("material.specular", 2);
    pro_item_->setUniformValue("material.shininess", 0.5f);

    QMatrix4x4 mat_item_model;
    mat_item_model.rotate(a, 1.0f, 0.0f, 0.0f);
    mat_item_model.rotate(b, 0.0f, 1.0f, 0.0f);
    mat_item_model.scale(0.01f);
    pro_item_->setUniformValue(model_, mat_item_model);
    pro_item_->setUniformValue(projection_, mat_projection);
    pro_item_->setUniformValue(view_, mat_view);

    mymodel_->Draw();
    pro_item_->release();

    pro_stream_->bind();
    for (auto av : avs_) {
        pro_stream_->setUniformValue("ourTexture", av->GetTextureID());
        av->Draw();
    }
    pro_stream_->release();
}

void Render::AddInput(RTMPInput *input, float x, float y) {
    AVStreamRender *avsr = new AVStreamRender;
    avsr->Init(x, y);
    avsr->AddInput(input);

    avs_.push_back(avsr);
}
