#pragma once

#include <QWidget>
#include <mutex>
#include <queue>
#include <thread>

#include "ui_QtWidgetsClass.h"

class QtWidgetsClass : public QWidget {
    Q_OBJECT

  public:
    QtWidgetsClass(QWidget *parent = Q_NULLPTR);
    ~QtWidgetsClass();

  private:
    void RenderThread();
    void Refresh(double &rematime);
    void DoRask();

  private:
    Ui::QtWidgetsClass ui;

    double frame_timer_;
    std::thread *render_thread_{nullptr};

    std::queue<std::function<void(void)>> rask_;
    std::mutex rask_mtx_;
};
