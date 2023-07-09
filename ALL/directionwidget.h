#ifndef DIRECTIONWIDGET_H
#define DIRECTIONWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class DirectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit DirectionWidget(QWidget *parent = nullptr);

public:
    // 创建四个按钮
    QPushButton* forwardButton;
    QPushButton* backwardButton;
    QPushButton* leftButton;
    QPushButton* rightButton;

    // 创建一个垂直布局
    QVBoxLayout* layout;
    QHBoxLayout* horizontalLayout;
};

#endif // DIRECTIONWIDGET_H
