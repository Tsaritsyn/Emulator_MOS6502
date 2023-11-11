//
// Created by Mikhail on 06/11/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_RegisterView.h" resolved

#include <QGridLayout>
#include <QLabel>
#include "registerview.hpp"

inline constexpr int AC_ROW = 0;
inline constexpr int X_ROW = 1;
inline constexpr int Y_ROW = 2;
inline constexpr int SP_ROW = 3;
inline constexpr int SR_ROW = 4;
inline constexpr int PC_ROW = 5;
inline constexpr int CYCLE_ROW = 6;


RegisterView::RegisterView(MOS6502 &cpu, QWidget *parent): QWidget(parent), m_cpu(cpu) {
    auto newLayout = new QGridLayout(this);
    newLayout->setSizeConstraint(QLayout::SetMinimumSize);

    newLayout->addWidget(new QLabel("AC", this), AC_ROW, 0);
    newLayout->addWidget(new QLabel("X", this), X_ROW, 0);
    newLayout->addWidget(new QLabel("Y", this), Y_ROW, 0);
    newLayout->addWidget(new QLabel("SP", this), SP_ROW, 0);
    newLayout->addWidget(new QLabel("SR", this), SR_ROW, 0);
    newLayout->addWidget(new QLabel("PC", this), PC_ROW, 0);
    newLayout->addWidget(new QLabel("cycle", this), CYCLE_ROW, 0);

    for (size_t i = 0; i < valueLabels.size(); i++) {
        valueLabels[i] = new QLabel("undefined");
        newLayout->addWidget(valueLabels[i], (int)i, 1);
    }
    update();

    setLayout(newLayout);
}

void RegisterView::update() noexcept {
    valueLabels[AC_ROW]->setText(std::format("{:02d} (0x{:02x})", m_cpu.get_AC(), m_cpu.get_AC()).c_str());
    valueLabels[X_ROW]->setText(std::format("{:02d} (0x{:02x})", m_cpu.get_X(), m_cpu.get_X()).c_str());
    valueLabels[Y_ROW]->setText(std::format("{:02d} (0x{:02x})", m_cpu.get_Y(), m_cpu.get_Y()).c_str());
    valueLabels[SP_ROW]->setText(std::format("{:02d} (0x{:02x})", m_cpu.get_SP(), m_cpu.get_SP()).c_str());
    valueLabels[SR_ROW]->setText(std::format("{}", m_cpu.get_SR().verbose_description()).c_str());
    valueLabels[PC_ROW]->setText(std::format("0x{:04x}", m_cpu.get_PC()).c_str());
    valueLabels[CYCLE_ROW]->setText(std::format("{:d}", m_cpu.get_cycle()).c_str());
}
