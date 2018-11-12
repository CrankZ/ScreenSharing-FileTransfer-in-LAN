#ifndef Receiver_H
#define Receiver_H

#include <QUdpSocket>
#include <QFile>

#include "justenum.h"
#include "sender.h"
#include "showscreen.h"
#include "monitorscreen.h"
#include "stuinfo.h"


class Receiver : public QThread
{
    Q_OBJECT
public:
    Receiver();

    bool bool_downLoadInfo = false;
    bool bool_stuName = false;

private:
    QUdpSocket * udpSocketRecv;

signals:
    void innerSendData(MessageType,QString);
    void innerSendIsWatching(bool);
    void sScreenSignal(QByteArray, int, QString, QString);
    void sDownloadInfoSignal(QString);
    void sStuInfoSignal(StuInfo*);

private slots:
    void innerRecvData(bool isWatching);
    void processPendingDatagram();
};

#endif // Receiver_H
