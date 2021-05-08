#include "QtWidgetsClass.h"

#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <QVector3D>
#include <iostream>

#include "media/RTMPInput.h"

QtWidgetsClass::QtWidgetsClass(QWidget *parent) : QWidget(parent) {
    ui.setupUi(this);

    connect(ui.btn_rec, &QPushButton::clicked, [this]() {
        QString url = ui.lineEdit->text();
        ui.widget->AddInput(url);
    });

    render_thread_ = new std::thread(&QtWidgetsClass::RenderThread, this);
}

QtWidgetsClass::~QtWidgetsClass() {}

void QtWidgetsClass::RenderThread() {
    ui.widget->Init();

    int64_t time = clock();
    while (true) {
        int64_t cur_time = clock();
        int64_t deta = cur_time - time;
        std::cout << "do_time:" << deta << std::endl;

        if (deta <= 40) {
            std::this_thread::sleep_for(std::chrono::milliseconds(40 - deta));
        }

        time += 40;

        ui.widget->Refresh(ui.horizontalSlider->value(), ui.horizontalSlider_2->value(),
                           QVector3D(ui.horizontalSlider_3->value() / 255.0f,
                                     ui.horizontalSlider_4->value() / 255.0f,
                                     ui.horizontalSlider_5->value() / 255.0f));
    }
}
