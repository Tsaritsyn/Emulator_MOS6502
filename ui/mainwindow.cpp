//
// Created by Mikhail on 07/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_MainWindow.h" resolved

#include "mainwindow.hpp"


MainWindow::MainWindow(QWidget* parent): QMainWindow(parent) {
//    count = 0;
//    button = new QPushButton("Push me", this);
//    button->setGeometry(QRect(QPoint(100, 100), QSize(200, 50)));
//
//    connect(button, &QPushButton::released, this, &MainWindow::increaseCount);
//
//    label = new QLabel("Button never pressed", this);
//    label->setGeometry(QRect(QPoint(100, 200), QSize(200, 50)));

    mainWidget = std::make_unique<QWidget>(this);

    pageViewsLayout = std::make_unique<QHBoxLayout>(mainWidget.get());
    mainWidget->setLayout(pageViewsLayout.get());
    setCentralWidget(mainWidget.get());

    addPageView(0x10);
    addPageView(0x20);
}

MainWindow::~MainWindow() {
//    delete button;
//    delete label;
}

void MainWindow::increaseCount() {
//    count++;
//
//    std::stringstream ss;
//    ss << "Button pressed " << count << " times";
//
//    label->setText(ss.str().c_str());
//    label->repaint();
}

void MainWindow::addPageView(Byte page) {
    auto scrollArea = std::make_unique<QScrollArea>(mainWidget.get());

    auto pageView = std::make_unique<PageView>(page, scrollArea.get());
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(pageView.get());
//    scrollArea->setFixedWidth(pageView->width());
//    scrollArea->setFixedHeight(this->height());

    pageViewsLayout->addWidget(scrollArea.get());

    scrollAreas.push_back(std::move(scrollArea));
    pageViews.push_back(std::move(pageView));
};
