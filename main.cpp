#include <QApplication>

#include "MOS6502.hpp"

#include "ui/mainwindow.hpp"

using namespace Emulator;

int main(int argc, char *argv[]) {
    ROM memory{};

    QApplication a(argc, argv);

    MainWindow mainWindow{memory};
    mainWindow.showMaximized();

    return QApplication::exec();
}
