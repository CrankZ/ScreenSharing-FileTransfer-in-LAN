#ifndef TRANSFERFILE_H
#define TRANSFERFILE_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QSplitter>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QCheckBox>
#include <QHostInfo>

#include "justenum.h"
#include "sender.h"
#include "receiver.h"
#include "checkonline.h"

namespace Ui {
class TransferFile;
}

class TransferFile : public QWidget
{
    Q_OBJECT

public:
    explicit TransferFile(QWidget *parent = 0);
    ~TransferFile();

private:
    Ui::TransferFile *ui;

private:
    Receiver *receiver;
    CheckOnline *checkOnline;
    Sender *sender;
    QMap<QString, bool> stuAliveMap;
    QList<QLabel*> lLabel1;
    QList<QProgressBar*> lProgressBar;
    QList<QLabel*> lLabel2;
    QList<QSplitter*> lSplitter;
    QList<QCheckBox*> lCb;
    QMap<QString, bool> isDownloadStuList;
    QMap<QString, StuInfo*> stuInfoMap;
    QString httpServer;

private:
    void addDownloadItem();
    QString getHostIPV4Address();
    void setIsDownloadStuList();
    void sendFile(QString filePath);
    void killProcess(QString processName);

public slots:
    void sDownloadInfoSlot(QString downLoadInfoAll);

private slots:
    void on_toolButton_selectFile_clicked();
    void on_checkBox_selectAllStu_clicked();
    void on_toolButton_stop_clicked();

protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
};

#endif // TRANSFERFILE_H
