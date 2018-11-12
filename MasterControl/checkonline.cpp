#include "checkonline.h"

// 类变量，所有变量公用这些变量
QMap<QString, bool> CheckOnline::stuAliveMap;
QMap<QString, StuInfo*> CheckOnline::stuInfoMap;

/**
 * 处理学生上线、下线
 * 统计学生人数
 * 因为学生的在线状态可能会被多个窗口只用，所以单独出来一个方便共享变量
 */
CheckOnline::CheckOnline()
{
    sender = new Sender();
}

void CheckOnline::run()
{
    heartBeatTimer = new QTimer();
    connect(heartBeatTimer, &QTimer::timeout,
            this, &CheckOnline::heartBeatTimerSlot);
    heartBeatTimer->start(1000); //start为QTIME自己的函数

    setFalseTimer = new QTimer();
    connect(setFalseTimer, &QTimer::timeout,
            this, &CheckOnline::setFalseTimerSlot);
    setFalseTimer->start(3000);

    this->exec();
}

void CheckOnline::heartBeatTimerSlot()
{
    sender->sendCommand(CM_tHeartBeat);
}

void CheckOnline::sStuInfoSlot(StuInfo *stuInfo)
{
    stuInfoMap.insert(stuInfo->stuMAC,stuInfo);
    QString stuMAC = stuInfo->stuMAC;
    QString stuName = stuInfo->stuName;
    // 接收到学生消息，在线状态置为true
    stuAliveMap.insert(stuMAC, true);
}

/**
 * 定时把所有学生在线状态设置为false
 * 如果没有接收到学生的在线数据，则判定掉线
 */
void CheckOnline::setFalseTimerSlot()
{
    for (int i = 0; i < stuInfoMap.count(); i++) {
        StuInfo *stuInfo = stuInfoMap.values().at(i);
        QString stuMAC = stuInfo->stuMAC;
        stuAliveMap.insert(stuMAC, false);
    }

}


