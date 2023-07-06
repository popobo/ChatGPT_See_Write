#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class GPTController;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void layoutInit();

    void moduleInit();

private slots:
    void sendData();

    void getResponse(const QString& str);

private:
    QTextBrowser* m_receivedBrowser;

    QTextEdit* m_sendEdit;

    QPushButton* m_serialButton;

    QPushButton* m_sendButton;

    /* vertical layout */
    QVBoxLayout* m_vboxLayout;

    /* grid layout */
    QGridLayout *m_gridLayout;

    /* main widget */
    QWidget *m_mainWidget;

    /* func widget */
    QWidget *m_funcWidget;

private:
    GPTController* m_gptController;
};
#endif // MAINWINDOW_H
