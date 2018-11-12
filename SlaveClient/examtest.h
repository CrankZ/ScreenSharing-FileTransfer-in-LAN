#ifndef EXAMTEST_H
#define EXAMTEST_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QFile>
#include <QMessageBox>
#include <QLabel>
#include <QRadioButton>
#include <QDebug>
#include <QGroupBox>
#include <QSplitter>
#include <QUdpSocket>
#include <QScrollArea>
#include <QtNetwork>
#include <QDesktopWidget>
#include <QCloseEvent>
#include <QPlainTextEdit>

#include "justenum.h"
#include "sendinfo.h"
#include "examinfo.h"
#include "prohibitor.h"
#include "sender.h"
#include "JlCompress.h"

namespace Ui {
class ExamTest;
}

class ExamTest : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExamTest(QWidget *parent = 0);
    ~ExamTest();

    void DealWithExamInfo(ExamInfo *examInfo);
    void startExam();

private:
    Ui::ExamTest *ui;

    QList<QSplitter*> lSpChoice;//用Splitter是因为能后期操作被他包含的radio

    QList<QSplitter*> lSpWXTK;//用Splitter是因为能后期操作被他包含的radio

    QMap<QString, QList<QSplitter*>> lSpCompletionMap;

    QMap<QString, QList<QSplitter*>> lSpMap;


    // 这三个目前来讲，还必须要全局变量
    // 因为有等多个content的话，还是要重复存取
    QList<QLabel*> lLbContent;
    QMap<QString, QList<QLabel*>> lLbContentMap;
    QList<QPlainTextEdit*> lPlaintTextEdit;
    QMap<QString, QList<QPlainTextEdit*>> lPlaintTextEditMap;
    QList<QSplitter*> lSp;
    int titleNum = 0;
    int globalTitleNum = 0;


    QMap<int, QString> rightAns;

    QMap<QString, QMap<int, QString>> rightAnsChoiceMap;

    QMap<int, QString> realAns;
    QMap<int, bool> realAnsIsRight;

//    QMap<int, QString> rightAnsCom;
    QMap<int, QMap<int, QString>> rightAnsCom; // 一个填空题里面可能有多个问题，所以外层还要一个Map来包裹

    // 题型
    QMap<QString, QMap<int, QMap<int, QString>>> rightAnsComMap;
    QMap<int, bool> realAnsIsRightCom;

    QUdpSocket *udp;
    QVBoxLayout *vbox;
    QWidget *wgNothing;
    QScrollArea *scrollArea;
    QTimer *startExamTimer;
    int timeStartNum;
    bool isStartExam = false;
    int hour = -1;
    int min = -1;
    int sec = -1;

    ExamInfo *examInfo;
    Prohibitor *prohibitor;

    int totalScoreChoice = 0;
    int totalScoreCompletion = 0;

    QList<int> randomNumList(int maxNum);

    int offset = 0; // 这个是选择题选项的数量，如果这个为0.那么就是4个选项，如果为1那么就是5个选择题，以此类推
    int choiceTitleNum = 0;
    int wxtkTitleNum = 0;   // 完形填空
    int compTitleNum = 0;

    QMap<QString, int> titleNumMap;

    QString justNextContent = "";
    QString JustRadio;

    QString justRadioAndAns;
    QString nextContextAndRadio;
    Sender *sender = new Sender;
    bool startWaitingConfirm = false;
    bool isReceived = false;
private:
    bool regExIsContainsChinese(QString str);
    bool regExIsContainsEnglish(QString str);
    bool regExIsContainsNumber(QString str);


    void setRealAnsIsRightCom();



    QString getMACAdress();
    void baseInfo();
    void printSendInfo();
    void sendExamInfo();

    bool isTimeOver();
    void timeOverTodo();

    bool savePaper(QString saveName, QString paper);

    void getStuInfo();
    void setSpDisable();
    void killProcess();
    void skinLoad();
    void normalAddWidget(QString type);
    void randomAddWidget(QString type);
    QByteArray encodeStr(QString strSource);
    QString decodeBa(QByteArray baEncode);
    QString decodeStr(QString strEncode);

    ////////////////////
    int choiceNum(QString fileName);

    void loadAuto(QString type, QString str);
    void loadChoice(QString type, QString str);
    void loadCompletion(QString type, QString str);

    void addChoiceRightAns(QString type, QString str);

    void checkAnsIsRightAll();
    void checkAnsIsRightChoiceAll();
    void checkAnsIsRightChoice(QString type);
    void checkAnsIsRightCompletionAll();
    void checkAnsIsRightCompletion(QString type);


    void selectRightAnsAll();
    void selectRigthAnsChoiceAll();
    void selectRigthAnsChoice(QString type);
    void selectRightAnsComAll();
    void selectRightAnsCom(QString type);

    void makeJustRadio(QString strAll);
    void makeContent(QString type, int i, QString strAll);
    void randomSelect();
    void setStuInfo();
    QString getHostIPV4Address();

    void saveStuInfo();

    int choiceNum2(QString strAll);
    void makeWidget(QString type, QString str);
    void reformStr(QString split, QString &str);
    int figureRealScore();
    void showTime(int timeStartNum);
    void backupExamInfo();
    bool isDirExist(QString fullPath);
    void compressDir(QString dir,QString saveName);
    bool DelDir(QString path);

private slots:
    void on_pushButton_submit_clicked();

public slots:
    void startExamTimerSlot();
    void closeEvent(QCloseEvent *event);
    void tExamConfirmSlot(QMap<QString, bool> stuConfirmMap);
};

#endif // EXAMTEST_H
