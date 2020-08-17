#include <QApplication>

#include "QtWidgetsClass.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  QtWidgetsClass w;
  w.show();
  return a.exec();
}
