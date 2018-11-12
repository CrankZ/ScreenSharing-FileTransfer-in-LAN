#ifndef RECEIVER_H
#define RECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QtNetwork>
#include <QPoint>
#include <QProcess>
#include <QSize>

#include "justenum.h"
#include "ShowDownload.h"
#include "screensharing.h"

#include "examtest.h"
#include "showscreen.h"

class Receiver : public QObject
{
    Q_OBJECT
public:
    explicit Receiver(QObject *parent = nullptr);

private:
    QUdpSocket * udpSocketRecv;



signals:
    void tScreen1Signal(QByteArray);
    void tOpenScreenSignal();
    void tCloseScreenSignal();
    void tDownloadSignal(QString, QMap<QString,bool>);
    void tHeartBeatSignal();
    void tWatchYouSignal(QMap<QString,int>);
    void tMasterIPSignal(QString);
    void tStopWatchSignal();
    void tCursorPosSignal(QSize, QString, QPoint);
    void tExamPaperInfoSignal(ExamInfo*);
    void tExamStartSignal();
    void tRemoveStuInfoSignal();
    void tCheckInSignal(QList<QString>);
    void tExamConfirmSignal(QMap<QString, bool>);

public slots:
    void processPendingDatagram();
};

#endif // RECEIVER_H
