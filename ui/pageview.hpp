//
// Created by Mikhail on 07/10/2023.
//

#ifndef EMULATOR_MOS6502_PAGEVIEW_HPP
#define EMULATOR_MOS6502_PAGEVIEW_HPP

#include <QWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QListView>

#include "MOS6502_definitions.hpp"
#include "byteview.hpp"

using namespace Emulator;


class PageView : public QWidget {
Q_OBJECT

public:
    explicit PageView(Byte pageIndex, ROM &memory, QWidget *parent = nullptr);

private:
    std::unique_ptr<QVBoxLayout> mainLayout;
    std::unique_ptr<ByteView> byteViews[256];

    Byte m_pageIndex;
    ROM &m_memory;
};


#endif //EMULATOR_MOS6502_PAGEVIEW_HPP
