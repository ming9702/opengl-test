#pragma once
#include <QOffscreenSurface>
#include <QVector3D>
#include <QWidget>

class Render;
class GLWidget : public QWidget {
    Q_OBJECT

  public:
    GLWidget(QWidget *parent);
    ~GLWidget();
    void Init();
    void Refresh(float a, float b, QVector3D pos);

  private:
    QOpenGLContext *ctx_;
    Render *render_;
    QWindow *sur_;
};
