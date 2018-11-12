#ifndef CheckOnline_H
#define CheckOnline_H

#include "sender.h"
#include "stuinfo.h"


class CheckOnline : public QThread
{
    Q_OBJECT

public:
    CheckOnline();

    Sender *sender;

    QMap<QString,int> stuWatchList;//QString stuMAC,bool isWatching

    // 更改stuInfoMap有点费劲还是，所以又搞了一个map。。。
    // MAC，是否上线
    static QMap<QString, bool> stuAliveMap;
    static QMap<QString, StuInfo*> stuInfoMap;

protected:
    void run();
private:
    QTimer *heartBeatTimer;
    QTimer *setFalseTimer;
    int stuIdx = 0;
private:
    QString getHostIPV4Address();
    void stuAliveList(QString MAC,bool alive);
    void stuClassMap(QString MAC,QString stuClassX);
public slots:
    void setFalseTimerSlot();
    void sStuInfoSlot(StuInfo *stuInfo);
    void heartBeatTimerSlot();
};

#endif // CheckOnline_H
