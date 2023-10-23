//
// Created by Mikhail on 07/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include <QInputDialog>
#include <iostream>
#include <QMessageBox>
#include "mainwindow.hpp"
#include "MOS6502.hpp"

using namespace Emulator;


MainWindow::MainWindow(ROM &memory, QWidget* parent): QMainWindow(parent), memory(memory) {
    mainWidget = std::make_unique<QWidget>(this);

    pageViewsLayout = std::make_unique<QHBoxLayout>(mainWidget.get());
    mainWidget->setLayout(pageViewsLayout.get());
    setCentralWidget(mainWidget.get());

    execute = std::make_unique<QAction>("Execute", this);
    connect(execute.get(), &QAction::triggered, this, &MainWindow::execute_program);

    addPageView = std::make_unique<QAction>("Add page", this);
    connect(addPageView.get(), &QAction::triggered, this, &MainWindow::add_page_view);

    fileMenu = std::unique_ptr<QMenu>(menuBar()->addMenu("File"));
    fileMenu->addAction(execute.get());
    fileMenu->addAction(addPageView.get());
}


void MainWindow::add_page_view() {
    Byte page = QInputDialog::getInt(this, "Page", "Page index", 0, 0, UINT8_MAX);

    auto scrollArea = std::make_unique<QScrollArea>(mainWidget.get());

    auto pageView = std::make_unique<PageView>(page, memory, scrollArea.get());
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(pageView.get());

    pageViewsLayout->addWidget(scrollArea.get());

    scrollAreas.push_back(std::move(scrollArea));
    pageViews.push_back(std::move(pageView));
}

void MainWindow::execute_program() {
    MOS6502 cpu{};
    cpu.burn(memory);
    cpu.reset();
    cpu.stop_on_break(true);

    auto executionStatus = cpu.execute();

    if (!executionStatus.has_value()) {
        std::visit(Overload{
                [this](MOS6502::UnknownOperation error) {
                    QMessageBox::warning(this,
                                         "Execution terminated unexpectedly",
                                         QString::fromStdString(std::vformat("Could not parse operation at 0x{:04x}",std::make_format_args(error.address)))
                                         );
                },
        },
                executionStatus.error()
                );
    }

    std::cout << cpu.dump(false) << '\n';
};
