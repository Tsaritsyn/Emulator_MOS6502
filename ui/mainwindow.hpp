//
// Created by Mikhail on 07/10/2023.
//

#ifndef EMULATOR_MOS6502_MAINWINDOW_HPP
#define EMULATOR_MOS6502_MAINWINDOW_HPP

#include <QPushButton>
#include <QLabel>
#include <QMainWindow>
#include <QScrollArea>
#include <QMenu>
#include <QMenuBar>

#include "pageview.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(ROM &memory, QWidget* parent = nullptr);

    void add_page_view();

private slots:
    void execute_program();

private:
    std::vector<std::unique_ptr<PageView>> pageViews;
    std::vector<std::unique_ptr<QScrollArea>> scrollAreas;

    std::unique_ptr<QWidget> mainWidget;
    std::unique_ptr<QHBoxLayout> pageViewsLayout;

    std::unique_ptr<QMenu> fileMenu;
    std::unique_ptr<QAction> execute;
    std::unique_ptr<QAction> addPageView;

    ROM &memory;
};


#endif //EMULATOR_MOS6502_MAINWINDOW_HPP
