#include "QtWidgetsClass.h"

#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <QVector3D>

#include "media/RTMPInput.h"

QtWidgetsClass::QtWidgetsClass(QWidget *parent) : QWidget(parent) {
    ui.setupUi(this);
    ui.widget->Init();
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]() {
        ui.widget->Refresh(ui.horizontalSlider->value(), ui.horizontalSlider_2->value(),
                           QVector3D(ui.horizontalSlider_3->value() / 255.0f,
                                     ui.horizontalSlider_4->value() / 255.0f,
                                     ui.horizontalSlider_5->value() / 255.0f));
    });

    timer->start(40);

    connect(ui.btn_rec, &QPushButton::clicked, [this]() {
        QString url = ui.lineEdit->text();
        ui.widget->AddInput(url);
    });
}

QtWidgetsClass::~QtWidgetsClass() {}
