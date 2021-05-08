#pragma once

#include <QWidget>
#include <thread>

#include "ui_QtWidgetsClass.h"

class QtWidgetsClass : public QWidget {
    Q_OBJECT

  public:
    QtWidgetsClass(QWidget *parent = Q_NULLPTR);
    ~QtWidgetsClass();

  private:
    void RenderThread();

  private:
    Ui::QtWidgetsClass ui;

    std::thread *render_thread_{nullptr};
};
