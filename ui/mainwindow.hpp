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
#include "registerview.hpp"
#include "constantsview.hpp"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(ROM &memory, QWidget* parent = nullptr);

    void add_page_view();

    ~MainWindow() override;

private slots:
    void execute_program();

private:
    QWidget* mainWidget;
    QWidget* environmentWidget;

    ROM &memory;
};


#endif //EMULATOR_MOS6502_MAINWINDOW_HPP
