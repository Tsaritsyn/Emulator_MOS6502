//
// Created by Mikhail on 07/11/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ConstantsView.h" resolved

#include <QGridLayout>
#include <QLabel>

#include "ROM.hpp"
#include "constantsview.hpp"


ConstantsView::ConstantsView(QWidget *parent): QWidget(parent), constants{BUILTIN_CONSTANTS.begin(), BUILTIN_CONSTANTS.end()} {
    auto newLayout = new QGridLayout(this);
    newLayout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(newLayout);

    for (size_t i = 0; i < constants.size(); i++) {
        newLayout->addWidget(new QLabel(QString::fromStdString(constants[i].name), this), (int)i, 0);
        newLayout->addWidget(new QLabel(QString::fromStdString(to_string(constants[i].value)), this), (int)i, 1);
    }
}

std::string to_string(ConstantValue value) {
    return std::visit(Overload{
        [](Byte byte) { return std::format("0x{:02x}", byte); },
        [](Word word) { return std::format("0x{:04x}", word); },
        }, value);
}
