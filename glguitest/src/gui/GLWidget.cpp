#include "GLWidget.h"

#include <QDebug>
#include <QOpenGLContext>
#include <QWindow>

#include "render/Render.h"

GLWidget::GLWidget(QWidget *parent) : ctx_{nullptr}, QWidget(parent) {
    winId();
    sur_ = windowHandle();
    sur_->setSurfaceType(QWindow::OpenGLSurface);
}

GLWidget::~GLWidget() {}

void GLWidget::Init() {
    ctx_ = new QOpenGLContext(this);
    ctx_->setFormat(sur_->requestedFormat());
    qDebug() << ctx_->create();
    qDebug() << ctx_->makeCurrent(sur_);
    render_ = new Render();
    render_->initialize();
}

void GLWidget::Refresh(float a, float b, QVector3D pos) {
    render_->render(sur_, a, b, pos);
    if (sur_->isExposed()) {
        ctx_->makeCurrent(sur_);
        ctx_->swapBuffers(sur_);
    }
}

void GLWidget::AddInput(RTMPInput *input, float x, float y) { render_->AddInput(input, x, y); }
