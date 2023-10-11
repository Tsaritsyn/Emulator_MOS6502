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

using namespace Emulator;


class PageView : public QWidget {
Q_OBJECT

public:
    explicit PageView(Byte pageIndex, /*ROM &memory, */QWidget *parent = nullptr);

//private slots:
//    template <int index>
//    void changeMemoryValue() {
//        std::cout << "Memory value changed to " << memoryValueLabels[index]->toPlainText().toStdString() << '\n';
//    };

private:
    void changeMemory(Byte page, Byte index);
//    std::unique_ptr<QScrollArea> mainScroll;
//    std::unique_ptr<QGroupBox> mainFrame;
//    std::unique_ptr<QGroupBox> mainGroupBox;
    std::unique_ptr<QVBoxLayout> mainLayout;

//    std::unique_ptr<QLabel> pageIndexLabel;

    std::unique_ptr<QLabel> byteIndexLabels[256];
    std::unique_ptr<QTextEdit> memoryValueLabels[256];
    std::unique_ptr<QLabel> assemblyDecodingLabels[256];
    std::unique_ptr<QTextEdit> commentLabels[256];

    std::unique_ptr<QHBoxLayout> groupLayouts[256];
    std::unique_ptr<QGroupBox> groupBoxes[256];

//    std::unique_ptr<QListView> listView;

    Byte m_pageIndex;
//    ROM &m_memory;
};


#endif //EMULATOR_MOS6502_PAGEVIEW_HPP
