#include "receiver.h"


Receiver::Receiver(QObject *parent) : QObject(parent)
{
    udpSocketRecv = new QUdpSocket(this);
    udpSocketRecv->bind(PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocketRecv,SIGNAL(readyRead()),
            this,SLOT(processPendingDatagram()));
}

void Receiver::processPendingDatagram()
{
    QByteArray datagram;
    while (udpSocketRecv->hasPendingDatagrams())
    {
        datagram.resize(udpSocketRecv->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        udpSocketRecv->readDatagram(
                    datagram.data(),
                    datagram.size(),
                    &sender,
                    &senderPort);

        QByteArray qUncom = qUncompress(datagram);

        QDataStream in(&qUncom, QIODevice::ReadOnly);//因为其属性为只读，所以是输入
        int msgType;

        in >> msgType;    //读取1个32位长度的整型数据到messageTyep中
        switch(msgType)
        {
        case CM_tCursorPos:
        {
            QSize sourceScreenSize;
            QString cursorType;
            QPoint masterCursorPos;

            in >> sourceScreenSize;
            in >> cursorType;
            in >> masterCursorPos;
            emit tCursorPosSignal(sourceScreenSize, cursorType, masterCursorPos);
            break;
        }

        case CM_tScreen1:
        {
            QByteArray bytes;
            in >> bytes;
            emit tScreen1Signal(bytes);
            break;
        }

        case CM_tScreen2:
        {
//            QByteArray qba;    //此处的int为qint32，在Qt中，qint8为char，qint16为uint
//            in >> qba;
//            showScreen->screenRecv(qba,2);
//            istScreen = true;
//            bHide = false;
            break;
        }
        case CM_tOpenScreen:
        {
            emit tOpenScreenSignal();
            break;
        }
        case CM_tCloseScreen:
        {
            emit tCloseScreenSignal();
            break;
        }
        case CM_tDownload:
        {
            QString url;
            QMap<QString,bool> isDownloadStuList;
            in >> url;
            in >> isDownloadStuList;
            emit tDownloadSignal(url, isDownloadStuList);

            break;
        }
        case CM_tHeartBeat:
        {
            //返回存活信息
            emit tHeartBeatSignal();
            break;
        }
        case CM_tCheckIn:
        {
            //用这个分三种情况1、学生端上线主动发送2、教师端上线主动发送3、新设置姓名重新发送
            //但是就需要新建单独的发送类，暂时先不搞了
            QList<QString> classNameList;
            in >> classNameList;
            emit tCheckInSignal(classNameList);
            break;
        }
        case CM_tMasterIP:
        {
            QString masterIP;
            in >> masterIP;
            emit tMasterIPSignal(masterIP);
            break;

        }
        case CM_tWatchYou:
        {
            QMap<QString, int> stuWatchList;
            in >> stuWatchList;
            emit tWatchYouSignal(stuWatchList);
            break;
        }
        case CM_tStopWatch:
        {
            emit tStopWatchSignal();
            break;
        }
        case CM_tExamPaperInfo:
        {
            ExamInfo *examInfo = new ExamInfo;
            in >> examInfo;
            emit tExamPaperInfoSignal(examInfo);
            break;
        }
        case CM_tExamStart:
        {
            emit tExamStartSignal();
            break;
        }
        case CM_tRemoveStuInfo:
        {
            emit tRemoveStuInfoSignal();
            break;
        }
        case CM_tExamConfirm:
        {
            QMap<QString, bool> stuConfirmMap;
            in >> stuConfirmMap;
            emit tExamConfirmSignal(stuConfirmMap);
            break;
        }
            break;
        }
    }
}
