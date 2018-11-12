#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtNetwork>
#include <QMessageBox>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QPoint>
#include <QThread>
#include <QSize>

#include "showscreen.h"
#include "showdownload.h"
#include "checkin.h"
#include "screensharing.h"
#include "justenum.h"
#include "examtest.h"
#include "examinfo.h"
#include "prohibitor.h"
#include "receiver.h"
#include "sender.h"
#include "stuinfo.h"

namespace Ui {
class Widget;
}
class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
private:
    Ui::Widget *ui;

public:
    bool istScreen = false;
    void closeEvent(QCloseEvent *event);
private:
    char* c_pdata;
    int onlineNum;
    char* data;
    QTimer *qTime;
    QString dlInfo = "";
    bool isShow = true;
    ShowScreen *showScreen;
    Prohibitor *prohibitor;
    //    bool isOpenScreen = false;
    bool isSendStuName = false;//姓名发送一次就停止
    bool justOnce = true;
    //以下为托盘内容
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    //窗口管理动作
    QAction *restartAction;
    QAction *aboutMeAction;
    Receiver *receiver;
    Sender *sender;
    bool isMasterOnline = false;
    ExamTest *examTest = new ExamTest;
    bool isReceivedClose = false;
    QLabel label;

private:
    void sendAlive();
    void deleteShow();
    void sendDownloadInfo();
    void skinLoad();
    void setAutoStart(bool is_auto_start);
    QString getStuClass();
    void startTrayIcon();
    ShowDownload *sd;
    ScreenSharing *screenSharing;
    void expireDate(QString endDateStr);
    QString getHostIPV4Address();
    QString decodeStr(QString strEncode);
    QString getMACAdress();
    QString getStuName();
    void sendStuName();
    void killProcess();
    void sendStuInfo();
    void getCurrentTimeFromWeb();
    void showTrayMessage(QString msg);

public slots:
    void on_timeLeft();
    void on_pushButton_hide_clicked();
    void restart();
    void aboutMe();
    void tScreen1Slot(QByteArray bytes);
    void tCursorPosSlot(QSize sourceScreenSize, QString cursorType, QPoint masterCursorPos);
    void tOpenScreenSlot();
    void tCloseScreenSlot();
    void tDownloadSlot(QString url, QMap<QString, bool> isDownloadStuList);
    void tHeartBeatSlot();
    void tWatchYouSlot(QMap<QString,int> stuWatchList);
    void tMasterIPSlot(QString masterIP);
    void tStopWatchSlot();
    void tPixmapSlot(QPixmap pixmap);
    void sendDownloadInfoSlot(float downloadPercentage,float downloadSpeed, QString downloadTime);
    void tExamPaperInfoSlot(ExamInfo *examInfo);
    void tExamStartSlot();
    void tRemoveStuInfoSlot();
    void tCheckInSlot(QList<QString> classNameList);
    void downloadFinishedSlot();
private slots:
    void on_pushButton_clicked();
};

#endif // WIDGET_H
