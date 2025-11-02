#include "../core/GameBoy.h"
#include "QtBoiMainWindow.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QtBoiMainWindow mainwindow;
  mainwindow.show();
  return app.exec();
}
