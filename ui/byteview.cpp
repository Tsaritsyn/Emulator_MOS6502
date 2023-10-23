//
// Created by Mikhail on 11/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ByteView.h" resolved

#include <iostream>

#include <QMessageBox>

#include "byteview.hpp"
#include "helpers.hpp"

constexpr int DEFAULT_DATA = -1;


ByteView::ByteView(ROM &memory_, Word address_, QWidget *parent): QWidget(parent), memory{memory_}, address{address_} {
    mainLayout = new QHBoxLayout(this);
    setLayout(mainLayout);
    layout()->setSizeConstraint(QLayout::SetMinimumSize);

    auto addressLabel = new QLabel(std::format("0x{:04x}", address).c_str(), this);
    valueEditor = new MyTextEdit(std::format("{:d}", memory[address]).c_str(), this);
    assemblySelector = new QComboBox(this);
    auto commentEditor = new QTextEdit("", this);

    valueEditor->setFixedHeight(addressLabel->height());
    valueEditor->setFixedWidth(addressLabel->width());
    commentEditor->setFixedHeight(addressLabel->height());

    for (auto opcode: allOpcodes) assemblySelector->addItem(byte_description(opcode).c_str(), opcode);
    assemblySelector->addItem("Raw", DEFAULT_DATA);
    update_assembly();

    layout()->addWidget(addressLabel);
    layout()->addWidget(valueEditor);
    layout()->addWidget(assemblySelector);
    layout()->addWidget(commentEditor);

    connect(valueEditor, &MyTextEdit::textEntered, this, [this](){
        auto text = valueEditor->toPlainText().toStdString();
        set_memory(text);
        update_assembly();
    });
    connect(assemblySelector, &QComboBox::currentIndexChanged, this, [this](){
        auto data = assemblySelector->currentData().toInt();
        if (data != DEFAULT_DATA) set_memory(data);
        update_value_text();
    });
}

void ByteView::update_assembly() {
    const auto index = assemblySelector->findData(memory[address]);
    if (index == -1) assemblySelector->setCurrentIndex(assemblySelector->findData(DEFAULT_DATA));
    else assemblySelector->setCurrentIndex(index);
}

void ByteView::set_memory(const std::string& text) {
    int newValue;
    try {
        size_t start;
        if (text.starts_with("0x")) {
            start = 2;
            newValue = std::stoi(text, &start, 16);
        }
        else newValue = std::stoi(text, &start, 10);
    }
    catch (const std::invalid_argument &e) {
        return;
    }

    set_memory(newValue);
}

void ByteView::set_memory(int value) {
    if (value >= 0 && value <= UINT8_MAX) {
        auto result = memory.set_byte(address, static_cast<Byte>(value));
        if (!result.has_value()) {
            QMessageBox::warning(this,
                                 "Attempt to override stack",
                                 QString::fromStdString(std::vformat("Writing to address 0x{:04x} that belongs to stack", std::make_format_args(result.error().address)))
            );
        }
        else update_assembly();
    }
}

void ByteView::update_value_text() {
    valueEditor->setText(QString::fromStdString(std::vformat("0x{:02x}", std::make_format_args(memory[address]))));

    auto cursor = valueEditor->textCursor();
    cursor.movePosition(QTextCursor::End);
    valueEditor->setTextCursor(cursor);
}

ByteView::~ByteView() {
    delete valueEditor;
    delete assemblySelector;
    delete mainLayout;
}
