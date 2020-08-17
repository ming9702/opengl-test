#include "Render.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
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
  LoadModel();
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
//   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pic.width(), pic.height(), 0, GL_BGRA,
//                GL_UNSIGNED_BYTE, pic.bits());
//   glGenerateMipmap(GL_TEXTURE_2D);
}

void Render::LoadModel() {
  mymodel_ = new Model{"res/Banana_OBJ/Banana.obj"};
  for (auto& mesh : mymodel_->meshs()) {
    glGenVertexArrays(1, &mesh.VAO);
    glGenBuffers(1, &mesh.VBO);
    glGenBuffers(1, &mesh.EBO);

    glBindVertexArray(mesh.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
                 &mesh.vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0],
                 GL_STATIC_DRAW);

    // 顶点位置
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // 顶点法线
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));
    // 顶点纹理坐标
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
  }
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

unsigned int Render::TextureFromFile(const char* path, const std::string& directory,
                                     bool gamma) {
  std::string filename = std::string(path);
  filename = directory + '/' + filename;

  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char* data =
      stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
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
  mat_view.translate(0.0f, 0.0f, -25.0f);

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
  pro_item_->setUniformValue("objectColor", QVector3D{1.0f, 0.5f, 0.31f});
  pro_item_->setUniformValue("lightColor", QVector3D{1.0f, 1.0f, 1.0f});
  pro_item_->setUniformValue("lightPos", pos);
  QMatrix4x4 mat_item_model;
  mat_item_model.rotate(a, 1.0f, 0.0f, 0.0f);
  mat_item_model.rotate(b, 0.0f, 1.0f, 0.0f);
  mat_item_model.scale(0.08f);
  pro_item_->setUniformValue(model_, mat_item_model);
  pro_item_->setUniformValue(projection_, mat_projection);
  pro_item_->setUniformValue(view_, mat_view);

  for (auto& mesh : mymodel_->meshs()) {
    // 绘制网格
    glBindVertexArray(mesh.VAO);
    glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
  }

  pro_item_->release();
}
