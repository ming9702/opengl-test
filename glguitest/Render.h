#pragma once
#include <QOpenGLFunctions_3_2_Core>

#include "Model.h"

class QWindow;
class QOpenGLShaderProgram;
class Render : public QOpenGLFunctions_3_2_Core {
 public:
  Render();
  ~Render();
  void initialize();
  void render(QWindow* win, float a, float b, QVector3D pos);

 private:
  void LoadModel();
  void LoadShader();
  void LoadShaderLight();
  void CreateLight();
  unsigned int TextureFromFile(const char* path, const std::string& directory,
                               bool gamma);
 private:
  GLuint projection_;
  GLuint view_;
  GLuint model_;

  QOpenGLShaderProgram* pro_item_;
  QOpenGLShaderProgram* pro_light_;
  Model* mymodel_;
  unsigned int light_vao_, light_vbo_;
};
