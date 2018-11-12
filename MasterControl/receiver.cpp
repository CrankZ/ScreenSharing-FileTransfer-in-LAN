#include "Receiver.h"

Receiver::Receiver()
{
    udpSocketRecv = new QUdpSocket(this);
    udpSocketRecv->bind(PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(udpSocketRecv,SIGNAL(readyRead()),this,SLOT(processPendingDatagram()));
}

void Receiver::innerRecvData(bool isWatching)
{
    innerSendIsWatching(false);
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
        case CM_sDownloadInfo:
        {
            QString downLoadInfo;
            in >> downLoadInfo;
            qDebug() << "CM_sDownloadInfo,downLoadInfo" << downLoadInfo;
            emit sDownloadInfoSignal(downLoadInfo);
            break;
        }
        case CM_sScreen:
        {
            QByteArray qba;
            QString stuName = "stuName";
            QString objName = "objName";
            int fourPart = -1;
            in >> qba;
            in >> fourPart;
            in >> stuName;
            in >> objName;
            emit sScreenSignal(qba, fourPart, stuName, objName);
            break;
        }
        case CM_sAlive:
        {
            QString MACAndClass;
            in >> MACAndClass;
            emit innerSendData(CM_sAlive, MACAndClass);
            break;
        }
        case CM_sStuName:
        {
            QString temp;
            in >> temp;
            emit innerSendData(CM_sStuName,temp);
            break;
        }
        case CM_sStuInfo:
        {
            StuInfo *stuInfo = new StuInfo();
            in >> stuInfo;
            emit sStuInfoSignal(stuInfo);
            break;
        }

        }
    }
}
