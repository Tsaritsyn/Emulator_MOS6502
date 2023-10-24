//
// Created by Mikhail on 11/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ByteView.h" resolved

#include <iostream>

#include <QMessageBox>

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

    std::cout << "Text changed to " << labelText << '\n';

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

    std::cout << "New value extracted: " << newValue << '\n';

    if (newValue <= UINT8_MAX) {
        std::cout << "Setting memory...\n";
        auto result = memory.set_byte(address, newValue);
        std::cout << "Result obtained\n";
        if (!result.has_value()) {
            std::cout << "Result is an error\n";
            QMessageBox::warning(this,
                                 "Attempt to override stack",
                                 QString::fromStdString(std::vformat("Writing to address 0x{:04x} that belongs to stack", std::make_format_args(result.error().address)))
            );
        }
        else {
            std::cout << "Result is successful\n";
            update_decoding();
        }
    }
}

void ByteView::update_decoding() {
    assemblyDecodingLabel->setText(byte_description(memory[address]).c_str());
}
