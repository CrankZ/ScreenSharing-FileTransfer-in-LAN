#ifndef ScreenSharing_H
#define ScreenSharing_H

#include <QUdpSocket>
#include <QThread>
#include <QMutex>
#include <windows.h>
#include <QPixmap>
#include <QFile>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QBuffer>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QPainter>
#include <QTime>
#include <QSize>

//为了引入枚举类型
#include "justenum.h"
#include "sender.h"

class ScreenSharing : public QThread
{
public:
    ScreenSharing(int num);
    ~ScreenSharing();

    void run();
    void pause();
    void goOn();
    void down();
    void readSettings();
private:
    int flag;

    int quality;
    bool lock;


    QMutex *mutex;
    QImage img1;
    Sender *sender;
    QString cursorType;
    bool firstScreen = true;

private:
    void sendScreen();
    bool compareImg(QImage img1,QImage img2);
    void sendCharBuffer(int dataLength, unsigned char *dataBuffer);
    QSize getCurrentScreenSize();
    QPoint getCurrentCursorPos();
    void sendCursorInfo();

};

#endif // ScreenSharing_H
