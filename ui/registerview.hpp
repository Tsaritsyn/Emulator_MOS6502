//
// Created by Mikhail on 06/11/2023.
//

#ifndef EMULATOR_MOS6502_REGISTERVIEW_HPP
#define EMULATOR_MOS6502_REGISTERVIEW_HPP

#include "MOS6502.hpp"

#include <QWidget>

using namespace Emulator;

class RegisterView : public QWidget {
Q_OBJECT

public:
    explicit RegisterView(MOS6502& cpu, QWidget *parent = nullptr);

    void update() noexcept;

private:
    MOS6502 &m_cpu;
    std::array<QLabel*, 7> valueLabels;
};


#endif //EMULATOR_MOS6502_REGISTERVIEW_HPP
