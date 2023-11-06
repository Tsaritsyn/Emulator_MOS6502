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


MainWindow::MainWindow(ROM &memory_, QWidget* parent): QMainWindow(parent), /*pageViews(), scrollAreas(),*/ memory(memory_) {
    mainWidget = new QWidget(this);

    auto pageViewsLayout = new QHBoxLayout(mainWidget);
    mainWidget->setLayout(pageViewsLayout);
    setCentralWidget(mainWidget);

    auto execute = new QAction("Execute", this);
    connect(execute, &QAction::triggered, this, &MainWindow::execute_program);

    auto addPageView = new QAction("Add page", this);
    connect(addPageView, &QAction::triggered, this, &MainWindow::add_page_view);

    auto fileMenu = menuBar()->addMenu("File");
    fileMenu->addAction(execute);
    fileMenu->addAction(addPageView);
}


void MainWindow::add_page_view() {
    Byte page = static_cast<Byte>(QInputDialog::getInt(this, "Page", "Page index", 0, 0, UINT8_MAX));

    auto scrollArea = new QScrollArea(mainWidget);

    auto pageView = new PageView(page, memory, scrollArea);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(pageView);

    mainWidget->layout()->addWidget(scrollArea);
}

void MainWindow::execute_program() {
    MOS6502 cpu{};
    cpu.burn(memory);
    cpu.reset();
    cpu.stop_on_break(true);

    auto registerView = new RegisterView(cpu, this);
    mainWidget->layout()->addWidget(registerView);

    auto executionStatus = cpu.execute();

    if (!executionStatus.has_value()) {
        std::visit(Overload{
                [this](MOS6502::ParseError error) {
                    QMessageBox::warning(this,
                                         "Execution terminated unexpectedly",
                                         QString::fromStdString(std::vformat("Could not parse operation at 0x{:04x}",std::make_format_args(error.address)))
                                         );
                },
                [this](MOS6502::OperationError errorInOperation) {
                    std::visit(Overload{
                            [this](ROM::StackOverride error) {
                                QMessageBox::warning(this,
                                                     "Execution terminated unexpectedly",
                                                     QString::fromStdString(std::vformat("Attempt to override stack value at 0x{:04x}",std::make_format_args(error.address)))
                                );
                            },
                            [this](MOS6502::AddressOverflow) {
                                QMessageBox::warning(this,
                                                     "Execution terminated unexpectedly",
                                                     QString::fromStdString("Attempt to create address outside of memory bounds")
                                );
                            },
                            [this](MOS6502::StackOverflow error) {
                                switch (error.type) {
                                    case MOS6502::StackOverflow::Type::STACK_EMPTY:
                                        QMessageBox::warning(this,
                                                             "Execution terminated unexpectedly",
                                                             QString::fromStdString("Attempt to pull from empty stack")
                                        );
                                        break;
                                    case MOS6502::StackOverflow::Type::STACK_FULL:
                                        QMessageBox::warning(this,
                                                             "Execution terminated unexpectedly",
                                                             QString::fromStdString("Attempt to push to full stack")
                                        );
                                        break;
                                }
                            }
                        },
                               errorInOperation);
                }
        },
                executionStatus.error()
                );
    }

    registerView->update();
}

MainWindow::~MainWindow() {
    delete mainWidget;
};
