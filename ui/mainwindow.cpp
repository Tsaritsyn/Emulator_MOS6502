//
// Created by Mikhail on 07/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.hpp"


MainWindow::MainWindow(ROM &memory, QWidget* parent): QMainWindow(parent), memory(memory) {
    mainWidget = std::make_unique<QWidget>(this);

    pageViewsLayout = std::make_unique<QHBoxLayout>(mainWidget.get());
    mainWidget->setLayout(pageViewsLayout.get());
    setCentralWidget(mainWidget.get());

    addPageView(0x10);
    addPageView(0x20);
}


void MainWindow::addPageView(Byte page) {
    auto scrollArea = std::make_unique<QScrollArea>(mainWidget.get());

    auto pageView = std::make_unique<PageView>(page, memory, scrollArea.get());
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(pageView.get());

    pageViewsLayout->addWidget(scrollArea.get());

    scrollAreas.push_back(std::move(scrollArea));
    pageViews.push_back(std::move(pageView));
};
