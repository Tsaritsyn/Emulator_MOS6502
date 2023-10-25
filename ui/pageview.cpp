//
// Created by Mikhail on 07/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PageView.h" resolved

#include "pageview.hpp"


PageView::PageView(Byte pageIndex, ROM &memory, QWidget *parent): QWidget(parent), m_pageIndex(pageIndex), m_memory(memory) {

    mainLayout = std::make_unique<QVBoxLayout>(this);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

    for (int i = 0; i <= UINT8_MAX; i++) {
        WordToBytes address;
        address.high() = pageIndex;
        address.low() = (Byte)i;

        byteViews[i] = std::make_unique<ByteView>(memory, address.word, this);
        mainLayout->addWidget(byteViews[i].get());
    }

    setLayout(mainLayout.get());
}
