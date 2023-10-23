//
// Created by Mikhail on 11/10/2023.
//

#ifndef EMULATOR_MOS6502_BYTEVIEW_HPP
#define EMULATOR_MOS6502_BYTEVIEW_HPP

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QHBoxLayout>

#include "MOS6502_definitions.hpp"
#include "ROM.hpp"

using namespace Emulator;

class ByteView : public QWidget {
Q_OBJECT

public:
    explicit ByteView(ROM &memory, Word address, QWidget *parent = nullptr);

private slots:
    void change_value();

private:
    void update_decoding();

    std::unique_ptr<QLabel> indexLabel;
    std::unique_ptr<QTextEdit> valueLabel;
    std::unique_ptr<QLabel> assemblyDecodingLabel;
    std::unique_ptr<QTextEdit> commentLabel;

    std::unique_ptr<QHBoxLayout> mainLayout;

    Word address;
    ROM &memory;
};


#endif //EMULATOR_MOS6502_BYTEVIEW_HPP
