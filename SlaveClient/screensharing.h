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

#include "justenum.h"

class ScreenSharing : public QThread
{
public:
    ScreenSharing(int num);
    ~ScreenSharing();

    void run();
    void pause();
    void goOn();
    void down();
    HBITMAP Copybitmap(LPRECT lprect);
    QString destIP;
    int fourPart = -1;
    QString stuName;

private:
    QUdpSocket *udp;
    char *data;
    QMutex *mutex;
    int flag;

    bool lock;
    QPixmap pm_edit;
    QString getMACAdress();
    QImage img1;
    int quality;
    bool compareImg(QImage img1,QImage img2);
    bool firstScreen = true;
};

#endif // ScreenSharing_H
