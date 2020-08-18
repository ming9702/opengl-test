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
  LoadShader();
  LoadShaderLight();
  CreateLight();
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

void Render::LoadShader() {
  std::ifstream in;
  std::istreambuf_iterator<char> beg(in), end;
  in.open("shader.v");
  std::string shader_v(beg, end);
  in.close();
  in.open("shader_light.f");
  std::string shader_f(beg, end);
  in.close();
  pro_item_ = new QOpenGLShaderProgram();
  pro_item_->addShaderFromSourceCode(QOpenGLShader::Vertex, shader_v.c_str());
  pro_item_->addShaderFromSourceCode(QOpenGLShader::Fragment, shader_f.c_str());
  pro_item_->link();
  projection_ = pro_item_->uniformLocation("projection");
  view_ = pro_item_->uniformLocation("view");
  model_ = pro_item_->uniformLocation("model");
}

void Render::LoadShaderLight() {
  std::ifstream in;
  std::istreambuf_iterator<char> beg(in), end;
  in.open("shader.v");
  std::string shader_v(beg, end);
  in.close();
  in.open("shader.f");
  std::string shader_f(beg, end);
  in.close();
  pro_light_ = new QOpenGLShaderProgram();
  pro_light_->addShaderFromSourceCode(QOpenGLShader::Vertex, shader_v.c_str());
  pro_light_->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                      shader_f.c_str());
  pro_light_->link();
}

void Render::CreateLight() {
  float vertices[] = {
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
      0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

      -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
      0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

      -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
      0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
      -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};
  glGenVertexArrays(1, &light_vao_);
  glGenBuffers(1, &light_vbo_);
  // 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
  glBindVertexArray(light_vao_);
  glBindBuffer(GL_ARRAY_BUFFER, light_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  // 4. 设定顶点属性指针
  // xyz
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  // uv
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));
}

void Render::render(QWindow* win, float a, float b, QVector3D pos) {
  static float offset = 0.01f;
  offset = -offset;
  pos.setY(pos.y() + offset);
  pos *= 10;

  const qreal retinaScale = win->devicePixelRatio();
  glViewport(0, 0, win->width() * retinaScale, win->height() * retinaScale);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  QMatrix4x4 mat_projection;
  mat_projection.perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
  QMatrix4x4 mat_view;
  QVector3D view_pos{0.0f, 0.0f, -25.0f};
  mat_view.translate(view_pos);

  pro_light_->bind();

  QMatrix4x4 mat_light_model;
  mat_light_model.translate(pos);
  pro_light_->setUniformValue("model", mat_light_model);
  pro_light_->setUniformValue("projection", mat_projection);
  pro_light_->setUniformValue("view", mat_view);

  glBindVertexArray(light_vao_);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glDrawArrays(GL_TRIANGLES, 0, 36);

  glDisableVertexAttribArray(1);
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
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
  mat_item_model.scale(0.08f);
  pro_item_->setUniformValue(model_, mat_item_model);
  pro_item_->setUniformValue(projection_, mat_projection);
  pro_item_->setUniformValue(view_, mat_view);

  mymodel_->Draw();

  pro_item_->release();
}
