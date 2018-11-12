#ifndef SHOWSCREEN_H
#define SHOWSCREEN_H

#include <QWidget>
#include <QDebug>
#include <QFlags>
#include <QCloseEvent>
#include <QPainter>
#include <QRect>
#include <QCursor>
#include <QLabel>
#include <QSize>

#include "justenum.h"


namespace Ui {
class ShowScreen;
}

class ShowScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ShowScreen(QWidget *parent = 0);
    ~ShowScreen();

private:
    Ui::ShowScreen *ui;

public:
    void screenRecv(QByteArray qba, int part);
    void closeEvent(QCloseEvent *event);
    void drawCursor(QSize sourceScreenSize, QString cursorType, QPoint masterCursorPos);
    void setCursorType(QString cursorType);

private:
    UdpUnpackData imageData;
    int recvImageNum;
    QPixmap cursorPixmap;
    QLabel *cursorLabel;
    QImage image;
    QPixmap pixmap;

private:


};

#endif // SHOWSCREEN_H
