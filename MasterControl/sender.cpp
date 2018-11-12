#include "Sender.h"

/**
 * UDP 广播
 */
Sender::Sender()
{
    udp = new QUdpSocket;
}

void Sender::sendCommand(MessageType msgType,QString msg)
{
//    qDebug() << "Sender:" << msgType;
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送

    out << msgType;

    switch(msgType)
    {
        case CM_tDownload://这里只有download因为下载需要额外的信息，就是地址。其他都只需要类型就行了
        {
            out << msg;
            break;
        }
        case CM_tMasterIP:
        {
            out << msg;
            break;
        }
        case CM_sStuName:
        {
            out << msg;
            break;
        }

    }
    QByteArray compressedBytes = qCompress(data);
    udp->writeDatagram(compressedBytes,compressedBytes.length(),QHostAddress::Broadcast, PORT);
    udp->waitForBytesWritten();
}

/**
 * 发送鼠标相关信息
 * 包括当前屏幕分辨率和鼠标坐标
 */
void Sender::sendCursorInfo(MessageType msgType, QSize sourceScreenSize,QString cursorType, QPoint point)
{
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送

    out << msgType;
    switch(msgType)
    {
        case CM_tCursorPos:
        {
            out << sourceScreenSize;
            out << cursorType;
            out << point;
            break;
        }
    }
    QByteArray compressedBytes = qCompress(data);
    udp->writeDatagram(compressedBytes,compressedBytes.length(),QHostAddress::Broadcast, PORT);
    udp->waitForBytesWritten();
}


void Sender::sendList(MessageType msgType,QList<QString> className)
{
    qDebug() << "sendList";
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送

    out << msgType;

    //一下都只是把数据存在out里
    //但是out又在data中
    //所以最后发送的还是data
    switch(msgType)
    {
        case CM_tCheckIn:
        {
            out << className;
            break;
        }
    }
    QByteArray compressedBytes = qCompress(data);
    udp->writeDatagram(compressedBytes,compressedBytes.length(),QHostAddress::Broadcast, PORT);
    udp->waitForBytesWritten();
}

void Sender::sendMap(MessageType msgType, QMap<QString, int> stuWatchList)
{
    qDebug() << "sendMap";
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送

    out << msgType;

    //一下都只是把数据存在out里
    //但是out又在data中
    //所以最后发送的还是data
    switch(msgType)
    {
        case CM_tWatchYou:
        {
            out << stuWatchList;
            break;
        }
    }
    QByteArray compressedBytes = qCompress(data);
    udp->writeDatagram(compressedBytes,compressedBytes.length(),QHostAddress::Broadcast, PORT);
    udp->waitForBytesWritten();
}

void Sender::sendEi(MessageType msgType, ExamInfo *ei)
{
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送

    out << msgType;

    //一下都只是把数据存在out里
    //但是out又在data中
    //所以最后发送的还是data
    switch(msgType)
    {
        case CM_tExamPaperInfo:
        {
            out << ei;
            break;
        }
    }
    QByteArray compressedBytes = qCompress(data);
    udp->writeDatagram(compressedBytes,compressedBytes.length(),QHostAddress::Broadcast, PORT);
//    udp->waitForBytesWritten();
}

void Sender::sendDownloadInfo(MessageType msgType, QString downloadUrl,QMap<QString,bool> isDownloadStuList)
{
    qDebug() << "sendDownloadInfo";
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送

    out << msgType;
    out << downloadUrl;
    out << isDownloadStuList;
    QByteArray compressedBytes = qCompress(data);
    udp->writeDatagram(compressedBytes,compressedBytes.length(),QHostAddress::Broadcast, PORT);
    udp->waitForBytesWritten();
}


