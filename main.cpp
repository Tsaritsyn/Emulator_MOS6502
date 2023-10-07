#include <QApplication>

#include "MOS6502.hpp"

#include "ui/mainwindow.hpp"

using namespace Emulator;

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    MainWindow mainWindow{};
    mainWindow.showMaximized();

    return QApplication::exec();
}
