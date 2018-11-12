#ifndef SENDER_H
#define SENDER_H

#include <QObject>
#include <QtNetwork>

#include "justenum.h"
#include "examinfo.h"
#include "stuinfo.h"

class Sender : public QObject
{
    Q_OBJECT
public:
    explicit Sender(QObject *parent = nullptr);

signals:

public slots:


public:
    void sendCommand(MessageType msgType, QString msg = "");
    void sendExamInfo(MessageType msgType, ExamInfo *examInfo);
    void sendStuInfo(MessageType msgType, StuInfo *stuInfo);
private:
    QUdpSocket *udp;
};

#endif // SENDER_H
