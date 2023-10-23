#include <QApplication>

#include "MOS6502.hpp"

#include "ui/mainwindow.hpp"

using namespace Emulator;

// TODO: Implement instruction-wise parsing of the memory (make the page view instruction-wise as well?)
// TODO: Implement instruction-wise code writing:
//  clicking on the assembly label reveals list of commands, choose from it and fill the arguments if necessary

int main(int argc, char *argv[]) {
    ROM memory{};

    QApplication a(argc, argv);

    MainWindow mainWindow{memory};
    mainWindow.showMaximized();

    return QApplication::exec();
}
