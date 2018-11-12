#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QProcess>
#include <QFileDialog>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QScrollArea>
#include <QChar>
#include <QDesktopWidget>
#include <QThread>

#include "ui_mainwindow.h"
#include "sender.h"
#include "transferfile.h"
#include "Receiver.h"
#include "screensharing.h"
#include "flowlayout.h"
#include "about.h"
#include "stucheckin.h"
#include "monitorscreen.h"
#include "checkonline.h"
#include "iconhelper.h"
#include "justenum.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    
public:
    Receiver *receiver;
    QMap<QString, bool> stuAliveMap;
    QMap<QString, QString> mapStuName;
    
private:
    QTimer *qTime;
    QTimer *randomTimer;
    QTimer *changeStuOnlineStateTimer;
    Sender *sender;
    QList<QLabel*> lLabelPixmap;
    ScreenSharing *screenSharing1;
    ScreenSharing *screenSharing2;
    FlowLayout *flowLayout;
    bool isFull = false;
    QList<QLabel*> lLabelName;
    QList<QSplitter*> lSpilllter;
    bool isOpenScreen = false;
    //以下为托盘内容
    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
    //窗口管理动作
    QAction *minimizeAction;
    QAction *maximizeAction;
    QAction *restoreAction;
    QAction *quitAction;
    QMap<QString, int> stuWatchList;//QString stuMAC,bool isWatching
    int stuIdx = 0;
    MonitorScreen *monitorScreen = new MonitorScreen;
    QObject *obj;
    QAction* actionWatch;
    QMap<QString,int> eachClassStuNum;
    QScrollArea *scrollArea;
    CheckOnline *checkOnline;
    int fourPart = 0;
    ShowScreen *showScreen = new ShowScreen;
    TransferFile *transferFile;

    StuCheckIn *stuCheckIn = new StuCheckIn;
    int watchStuIndex = 0;

private:
    void skinLoad();
    void setStuName(QString stuNameAll);
    void addStuItem();
    void addNewStu(QString MAC, QString stuName);
    void clearLayout(QLayout* layout, bool deleteWidgets = true);
    void changeStuOnlineStateTimerSlot();
    void startTrayIcon();
    void expireDate(QString endDateStr);
    QString getHostIPV4Address();
    QList<int> randomTest(int maxNum);
    QByteArray encodeStr(QString strSource);
    QString decodeBa(QByteArray baEncode);
    QString decodeStr(QString strEncode);
    void multiWatchStu();
    void sStuInfoSlot(StuInfo *stuInfo);
    void getCurrentTimeFromWeb();
    
protected:
    void mouseReleaseEvent( QMouseEvent* );
    bool eventFilter(QObject * obj, QEvent * event);

signals:
    void clicked();
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_toolButton_screenBC_clicked();
    void on_toolButton_moniterStuScreen_clicked();
    void on_toolButton_fileTransfer_clicked();
    void on_toolButton_checkIn_clicked();
    void on_toolButton_ExamSystem_clicked();
    
private slots:
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void closeEvent(QCloseEvent *event);
    void showAbout();
    void watchStuScreen();
    void checkClassStuNum2();
    void sScreenSlot(QByteArray qba, int fourPart, QString stuName, QString objName);
    void on_timeLeft();
    void randomTimerSlot();
    void startWatchingSlot();
    void stopWatchingSlot();
    void watchSingleStuScreenSlot(QString stuMAC);
    void showScreenCloseSlot();

    

};

#endif // MAINWINDOW_H
