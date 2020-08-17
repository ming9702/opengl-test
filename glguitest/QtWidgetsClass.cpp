#include "QtWidgetsClass.h"

#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <QVector3D>

QtWidgetsClass::QtWidgetsClass(QWidget *parent) : QWidget(parent) {
  ui.setupUi(this);
  ui.widget->Init();
  QTimer *timer = new QTimer(this);
  connect(timer, &QTimer::timeout, [this]() {
    ui.widget->Refresh(ui.horizontalSlider->value(),
                       ui.horizontalSlider_2->value(),
                       QVector3D(ui.horizontalSlider_3->value() / 255.0f,
                                 ui.horizontalSlider_4->value() / 255.0f,
                                 ui.horizontalSlider_5->value() / 255.0f));
  });
  timer->start(100);
}

QtWidgetsClass::~QtWidgetsClass() {}
