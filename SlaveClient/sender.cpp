#include "sender.h"

Sender::Sender(QObject *parent) : QObject(parent)
{
    udp = new QUdpSocket(this);
}


void Sender::sendCommand(MessageType msgType, QString msg)
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
    case CM_sDownloadInfo:
    {
        qDebug() << "msg:" << msg;
        out << msg;
        break;
    }
    case CM_sAlive:
    {
        //返回存活信息
        out << msg;
        break;
    }
    case CM_sStuName:
    {
        //发送学生姓名
        out << msg;
        //        qDebug() << "CM_sStuName发送成功" << msg;
        break;
    }
        break;
    }
    QByteArray compressedBytes = qCompress(data);
    udp->writeDatagram(compressedBytes,compressedBytes.length(),QHostAddress::Broadcast, PORT);
    //    udp->waitForBytesWritten();
}

void Sender::sendExamInfo(MessageType msgType, ExamInfo *examInfo)
{
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送
    out << msgType;

    switch(msgType)
    {
    case CM_sExam:
    {
        out << examInfo;
        break;
    }
        break;
    }
    QByteArray compressedBytes = qCompress(data);
    // 注意这里端口变了
    udp->writeDatagram(compressedBytes,compressedBytes.length(),
                       QHostAddress::Broadcast, EXAMPORT);
    //    udp->waitForBytesWritten();

}
void Sender::sendStuInfo(MessageType msgType, StuInfo *stuInfo)
{
    QByteArray data;    //字节数组
    //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
    QDataStream out(&data, QIODevice::WriteOnly);//out为待发送
    out << msgType;

    switch(msgType)
    {
    case CM_sStuInfo:
    {
        out << stuInfo;
        break;
    }
        break;
    }
    QByteArray compressedBytes = qCompress(data);
    // 注意这里端口变了
    udp->writeDatagram(compressedBytes,compressedBytes.length(),
                       QHostAddress::Broadcast, PORT);
    //    udp->waitForBytesWritten();

}
