#ifndef Sender_H
#define Sender_H

#include <QUdpSocket>
#include <QNetworkInterface>
#include <QDataStream>
#include <QPoint>
#include <QPixmap>

#include "justenum.h"
#include "examinfo.h"
class Sender  : public QThread
{
    Q_OBJECT

public:
    Sender();
    void sendCommand(MessageType msgtype, QString msg = "");
    void sendEi(MessageType msgtype, ExamInfo *ei);
    void sendList(MessageType msgtype,QList<QString> className);
    void sendMap(MessageType msgtype, QMap<QString, int> stuWatchList);
    void sendDownloadInfo(MessageType msgtype, QString downloadUrl,QMap<QString,bool> isDownloadStuList);
    void sendCursorInfo(MessageType msgType, QSize sourceScreenSize, QString cursorType, QPoint point);


private:
    QUdpSocket *udp;

};

#endif // Sender_H
