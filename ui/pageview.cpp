//
// Created by Mikhail on 07/10/2023.
//

// You may need to build the project (run Qt uic code generator) to get "ui_PageView.h" resolved

#include "pageview.hpp"


PageView::PageView(Byte pageIndex, ROM &memory, QWidget *parent): QWidget(parent), byteViews() {

    mainLayout = new QVBoxLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetMinimumSize);

    for (size_t i = 0; i <= UINT8_MAX; i++) {
        WordToBytes address;
        address.high() = pageIndex;
        address.low() = static_cast<Byte>(i);

        byteViews[i] = new ByteView(memory, address.word, this);
        mainLayout->addWidget(byteViews[i]);
    }

    setLayout(mainLayout);
}

PageView::~PageView() {
    delete mainLayout;
    for (auto byteView: byteViews) delete byteView;
}
