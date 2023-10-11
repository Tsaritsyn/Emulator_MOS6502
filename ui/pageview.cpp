//
// Created by Mikhail on 07/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PageView.h" resolved

#include <iostream>

#include "pageview.hpp"


PageView::PageView(Byte pageIndex, /*ROM &memory,*/ QWidget *parent): QWidget(parent), m_pageIndex(pageIndex)/*, m_memory(memory)*/ {
////    mainScroll = std::make_unique<QScrollArea>(this);
////    mainScroll->setWidgetResizable(true);
//
////    mainFrame = std::make_unique<QGroupBox>(this);
//
//    mainLayout = std::make_unique<QVBoxLayout>(this);
//    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);
////    mainFrame->setLayout(mainLayout.get());
////    mainFrame->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
//
////    mainScroll->setWidget(mainFrame.get());
//
//    pageIndexLabel = std::make_unique<QLabel>(std::format("Page 0x{:02x}", m_pageIndex).c_str(), this);
////    mainFrame->layout()->addWidget(pageIndexLabel.get());
//
//    for (int i = 0; i < 256; i++) {
//        groupBoxes[i] = std::make_unique<QGroupBox>(this);
//
//        groupLayouts[i] = std::make_unique<QHBoxLayout>(groupBoxes[i].get());
//        groupBoxes[i]->setLayout(groupLayouts[i].get());
//
//        byteIndexLabels[i] = std::make_unique<QLabel>(std::format("0x{:02x}", i).c_str(), groupBoxes[i].get());
//        byteIndexLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
//        groupBoxes[i]->layout()->addWidget(byteIndexLabels[i].get());
//
//        memoryValueLabels[i] = std::make_unique<QTextEdit>("0xC8", groupBoxes[i].get());
//        memoryValueLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
//        connect(memoryValueLabels[i].get(), &QTextEdit::textChanged, this, [this, i]{ changeMemory(m_pageIndex, i);});
//        groupBoxes[i]->layout()->addWidget(memoryValueLabels[i].get());
//
//        assemblyDecodingLabels[i] = std::make_unique<QLabel>("INY", groupBoxes[i].get());
//        assemblyDecodingLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
//        groupBoxes[i]->layout()->addWidget(assemblyDecodingLabels[i].get());
//
//        commentLabels[i] = std::make_unique<QTextEdit>("increment Y register", groupBoxes[i].get());
//        commentLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
//        groupBoxes[i]->layout()->addWidget(commentLabels[i].get());
//
//
////        mainFrame->layout()->addWidget(groupBoxes[i].get());
//    }
//
//    setLayout(mainLayout.get());

    mainLayout = std::make_unique<QVBoxLayout>(this);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

//    pageIndexLabel = std::make_unique<QLabel>(std::format("Page 0x{:02x}", m_pageIndex).c_str(), this);
//    mainLayout->addWidget(pageIndexLabel.get());


    for (int i = 0; i < 256; i++) {
        groupBoxes[i] = std::make_unique<QGroupBox>(this);

        groupLayouts[i] = std::make_unique<QHBoxLayout>(groupBoxes[i].get());
        groupLayouts[i]->setSizeConstraint(QLayout::SetMinimumSize);

        byteIndexLabels[i] = std::make_unique<QLabel>(std::format("0x{:02x}{:02x}", m_pageIndex, i).c_str(), groupBoxes[i].get());
//        byteIndexLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        memoryValueLabels[i] = std::make_unique<QTextEdit>("0xC8", groupBoxes[i].get());
//        memoryValueLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        connect(memoryValueLabels[i].get(), &QTextEdit::textChanged, this, [this, i]{ changeMemory(m_pageIndex, i);});

        assemblyDecodingLabels[i] = std::make_unique<QLabel>("INY", groupBoxes[i].get());
//        assemblyDecodingLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

        commentLabels[i] = std::make_unique<QTextEdit>("increment Y register", groupBoxes[i].get());
//        commentLabels[i]->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);


        groupLayouts[i]->addWidget(byteIndexLabels[i].get());
        groupLayouts[i]->addWidget(memoryValueLabels[i].get());
        groupLayouts[i]->addWidget(assemblyDecodingLabels[i].get());
        groupLayouts[i]->addWidget(commentLabels[i].get());

        groupBoxes[i]->setLayout(groupLayouts[i].get());

        mainLayout->addWidget(groupBoxes[i].get());
    }

    setLayout(mainLayout.get());
}

void PageView::changeMemory(Byte page, Byte index) {
    WordToBytes address;
    address.high = page;
    address.low = index;

    std::cout << std::format("Changed value at address 0x{:04x} to {}", address.word, memoryValueLabels[index]->toPlainText().toStdString()) << '\n';
}
