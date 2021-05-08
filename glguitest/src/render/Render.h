#pragma once
#include <QOpenGLFunctions_3_2_Core>

#include "AVStreamRender.h"
#include "Model.h"

class QWindow;
class QOpenGLShaderProgram;
class Render : public QOpenGLFunctions_3_2_Core {
  public:
    Render();
    ~Render();
    void initialize();
    void render(QWindow *win, float a, float b, QVector3D pos);
    void AddInput(const QString &url);

  private:
    void LoadShaderStream(const std::string &file_vs, const std::string &file_fs);
    void LoadShaderItem(const std::string &file_vs, const std::string &file_fs);
    void LoadShaderLight(const std::string &file_vs, const std::string &file_fs);
    void CreateLight();
    QOpenGLShaderProgram *CreateShader(const std::string &file_vs, const std::string &file_fs);

  private:
    GLuint projection_;
    GLuint view_;
    GLuint model_;

    QOpenGLShaderProgram *pro_item_{nullptr};
    QOpenGLShaderProgram *pro_light_{nullptr};
    QOpenGLShaderProgram *pro_stream_{nullptr};

    Model *mymodel_{nullptr};
    AVStreamRender *av_render_{nullptr};
    unsigned int light_vao_, light_vbo_;
};
