#include "directionwidget.h"

DirectionWidget::DirectionWidget(QWidget *parent) : QWidget(parent)
{
    // 创建四个按钮
    forwardButton = new QPushButton("forward", this);
    backwardButton = new QPushButton("backward", this);
    leftButton = new QPushButton("left", this);
    rightButton = new QPushButton("right", this);

    // 创建一个垂直布局
    layout = new QVBoxLayout(this);

    // 添加按钮到布局中，按照上下左右的顺序
    layout->addWidget(forwardButton);

    // 创建一个水平布局
    horizontalLayout = new QHBoxLayout();

    // 添加左右按钮到水平布局中
    horizontalLayout->addWidget(leftButton);
    horizontalLayout->addWidget(rightButton);

    // 添加水平布局到垂直布局中
    layout->addLayout(horizontalLayout);

    layout->addWidget(backwardButton);

    // 设置布局到Widget中
    setLayout(layout);
}
