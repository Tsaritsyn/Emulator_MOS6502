//
// Created by Mikhail on 11/10/2023.
//

#ifndef EMULATOR_MOS6502_BYTEVIEW_HPP
#define EMULATOR_MOS6502_BYTEVIEW_HPP

#include <iostream>

#include <QWidget>
#include <QLabel>
#include <QTextEdit>
#include <QHBoxLayout>
#include <QComboBox>
#include <QKeyEvent>


#include "MOS6502_definitions.hpp"
#include "ROM.hpp"
#include "definitions.hpp"

using namespace Emulator;

// https://stackoverflow.com/questions/64298466/how-to-emit-qtexttexidtextchanged-signal-only-in-certain-cases-selectively
class MyTextEdit: public QTextEdit {
    Q_OBJECT

public:
    MyTextEdit(QWidget *parent = nullptr): QTextEdit(parent) {}

    template<typename ... Args>
    MyTextEdit(Args... args): QTextEdit(args...) {}

    signals:
            void textEntered();

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) emit textEntered();
        else QTextEdit::keyPressEvent(event);
    }
};

class ByteView : public QWidget {
    Q_OBJECT

public:
    explicit ByteView(ROM &memory, Word address, QWidget *parent = nullptr);

    ~ByteView() override;

private:
    void set_memory(int value);
    void set_memory(const std::string& text);

    void update_assembly();
    void update_value_text();
    void update_color();

    QHBoxLayout* mainLayout;

    MyTextEdit* valueEditor;
    QComboBox* assemblySelector;

    QPalette palette;

    ROM &memory;
    Word address;
};


#endif //EMULATOR_MOS6502_BYTEVIEW_HPP
