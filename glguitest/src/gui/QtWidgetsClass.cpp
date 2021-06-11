#include "QtWidgetsClass.h"

#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <QVector3D>

extern "C" {
#include <libavutil/time.h>
}

#include <iostream>

#include "media/RTMPInput.h"

QtWidgetsClass::QtWidgetsClass(QWidget *parent) : QWidget(parent) {
    ui.setupUi(this);

    connect(ui.btn_rec, &QPushButton::clicked, [this]() {
        QString url = ui.lineEdit->text();
        RTMPInput *input = new RTMPInput;
        input->StreamOpen(url.toLocal8Bit().constData());

        {
            std::lock_guard<std::mutex> lk(rask_mtx_);
            rask_.push([this, input]() { ui.widget->AddInput(input, -1.0f, 0.0f); });
        }
    });

    connect(ui.btn_rec_2, &QPushButton::clicked, [this]() {
        QString url = ui.lineEdit_2->text();
        RTMPInput *input = new RTMPInput;
        input->StreamOpen(url.toLocal8Bit().constData());

        {
            std::lock_guard<std::mutex> lk(rask_mtx_);
            rask_.push([this, input]() { ui.widget->AddInput(input, 0, 0.0f); });
        }
    });

    render_thread_ = new std::thread(&QtWidgetsClass::RenderThread, this);
}

QtWidgetsClass::~QtWidgetsClass() {}

void QtWidgetsClass::RenderThread() {
    ui.widget->Init();

    frame_timer_ = av_gettime_relative() / 1000000.0;

    while (true) {

        double rematime = 0.01;

        Refresh(rematime);

        if (rematime > 0) av_usleep((int64_t)(rematime * 1000000.0));

        DoRask();
    }
}

void QtWidgetsClass::Refresh(double &rematime) {
    double time = av_gettime_relative() / 1000000.0;

    double deta = time - frame_timer_;
    if (deta > 0.04) {
        std::cout << "do_time:" << deta << std::endl;
        ui.widget->Refresh(ui.horizontalSlider->value(), ui.horizontalSlider_2->value(),
                           QVector3D(ui.horizontalSlider_3->value() / 255.0f,
                                     ui.horizontalSlider_4->value() / 255.0f,
                                     ui.horizontalSlider_5->value() / 255.0f));
        frame_timer_ += 0.04;
        rematime = 0;
    }
}

void QtWidgetsClass::DoRask() {
    std::lock_guard<std::mutex> lk(rask_mtx_);
    while (rask_.size()) {
        std::function<void(void)> rask = rask_.front();
        rask_.pop();
        rask();
    }
}
