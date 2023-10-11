//
// Created by Mikhail on 07/10/2023.
//

#ifndef EMULATOR_MOS6502_MAINWINDOW_HPP
#define EMULATOR_MOS6502_MAINWINDOW_HPP

#include <QPushButton>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>

#include "pageview.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override;

    void addPageView(Byte page);

private slots:
    void increaseCount();

private:
    std::vector<std::unique_ptr<PageView>> pageViews;
    std::vector<std::unique_ptr<QScrollArea>> scrollAreas;

    std::unique_ptr<QWidget> mainWidget;
    std::unique_ptr<QHBoxLayout> pageViewsLayout;
//    int count;
//    QPushButton* button;
//    QLabel* label;
};


#endif //EMULATOR_MOS6502_MAINWINDOW_HPP
