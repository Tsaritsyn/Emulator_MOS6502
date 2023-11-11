//
// Created by Mikhail on 07/11/2023.
//

#ifndef EMULATOR_MOS6502_CONSTANTSVIEW_HPP
#define EMULATOR_MOS6502_CONSTANTSVIEW_HPP

#include <QWidget>

#include "MOS6502.hpp"

using namespace Emulator;

using ConstantValue = std::variant<Byte, Word>;

std::string to_string(ConstantValue value);


class ConstantsView : public QWidget {
Q_OBJECT

public:
    struct Constant { std::string name; ConstantValue value; };

    explicit ConstantsView(QWidget *parent = nullptr);

private:

    std::vector<Constant> constants;

};

const std::array<ConstantsView::Constant, 4> BUILTIN_CONSTANTS {
        ConstantsView::Constant{"INTERRUPT HANDLER", ROM::INTERRUPT_HANDLER},
        ConstantsView::Constant{"RESET LOCATION", ROM::RESET_LOCATION},
        ConstantsView::Constant{"BRK HANDLER", ROM::BRK_HANDLER},
        ConstantsView::Constant{"STACK BOTTOM", ROM::STACK_BOTTOM}
};


#endif //EMULATOR_MOS6502_CONSTANTSVIEW_HPP
