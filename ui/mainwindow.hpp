//
// Created by Mikhail on 07/10/2023.
//

#ifndef EMULATOR_MOS6502_MAINWINDOW_HPP
#define EMULATOR_MOS6502_MAINWINDOW_HPP

#include <QPushButton>
#include <QLabel>
#include <QMainWindow>

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override;

private slots:
    void increaseCount();

private:
    int count;
    QPushButton* button;
    QLabel* label;
};


#endif //EMULATOR_MOS6502_MAINWINDOW_HPP
