//
// Created by Mikhail on 11/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ByteView.h" resolved

#include <iostream>
#include "byteview.hpp"
#include "MOS6502_helpers.hpp"


ByteView::ByteView(ROM &memory, Word address, QWidget *parent): QWidget(parent), memory{memory}, address{address} {
    mainLayout = std::make_unique<QHBoxLayout>(this);
    setLayout(mainLayout.get());
    layout()->setSizeConstraint(QLayout::SetMinimumSize);

    indexLabel = std::make_unique<QLabel>(std::format("0x{:04x}", address).c_str(), this);
    valueLabel = std::make_unique<QTextEdit>(std::format("{:d}", memory[address]).c_str(), this);
    assemblyDecodingLabel = std::make_unique<QLabel>(this);
    commentLabel = std::make_unique<QTextEdit>("", this);

    valueLabel->setFixedHeight(indexLabel->height());
    valueLabel->setFixedWidth(indexLabel->width());
    commentLabel->setFixedHeight(indexLabel->height());

    update_decoding();

    layout()->addWidget(indexLabel.get());
    layout()->addWidget(valueLabel.get());
    layout()->addWidget(assemblyDecodingLabel.get());
    layout()->addWidget(commentLabel.get());

    connect(valueLabel.get(), &QTextEdit::textChanged, this, &ByteView::change_value);
}

void ByteView::change_value() {
    std::string labelText = valueLabel->toPlainText().toStdString();
    int newValue;
    try {
        size_t start;
        if (labelText.starts_with("0x")) {
            start = 2;
            newValue = std::stoi(labelText, &start, 16);
        }
        else newValue = std::stoi(labelText, &start, 10);
    }
    catch (const std::invalid_argument &e) {
        return;
    }

    if (newValue <= UINT8_MAX) {
        memory[address] = newValue;
        update_decoding();
    }
}

void ByteView::update_decoding() {
    assemblyDecodingLabel->setText(byte_description(memory[address]).c_str());
}
