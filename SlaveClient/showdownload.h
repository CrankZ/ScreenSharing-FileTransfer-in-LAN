#ifndef SHOWDOWNLOAD_H
#define SHOWDOWNLOAD_H

#include <QWidget>
#include <QtNetwork>
#include "justenum.h"

namespace Ui {
class ShowDownload;
}

class ShowDownload : public QWidget
{
    Q_OBJECT

public:
    explicit ShowDownload(QWidget *parent = 0);
    ~ShowDownload();

    void startRequest(QUrl url); //请求链接
    bool isDownload(QMap<QString,bool> isDownloadStuList);
    void startDownload(QString sendUrl);
    float downloadPercentage = 0;
    float downloadSpeed = 0;
    int mutex = 0;
    QString downloadTime;
    void stopWork();
    QString getMACAdress();



private:
    Ui::ShowDownload *ui;

    QNetworkAccessManager *manager;
    QNetworkReply *reply;
//    QUrl url;   //存储网络地址
    QFile *file;  //文件指针
    QTimer *qtime;
    void newTransferFolder();
    QString decodeGBKURL(QString str);

private slots:
    void on_pushButton_clicked();

    void on_pushButton_cancel_clicked();

private slots:
    void httpFinished();  //完成下载后的处理
    void httpReadyRead();  //接收到数据时的处理
    void updateDataReadProgress(qint64, qint64); //更新进度条
    void on_timeLeft();

signals:
    void updateDownloadInfoSignal(float, float, QString);
    void downloadFinishedSignal();


};

#endif // SHOWDOWNLOAD_H
