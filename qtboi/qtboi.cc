#include <QtWidgets/QApplication>
#include "QtBoiMainWindow.h"
#include "../core/GameBoy.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QtBoiMainWindow mainwindow;
  mainwindow.show();
  return app.exec();
}
