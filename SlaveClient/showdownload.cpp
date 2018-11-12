#include "showdownload.h"
#include "ui_showdownload.h"

ShowDownload::ShowDownload(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowDownload)
{
    ui->setupUi(this);

    manager = new QNetworkAccessManager(this);

    int oneSec = 1000;    //一秒，这个一般不修改1000
    qtime = new QTimer;
    qtime->start(oneSec*0.01); //start为QTIME自己的函数
//    connect(qtime,SIGNAL(timeout()),this,SLOT(on_timeLeft()));
    this->setLayout(ui->verticalLayout);
    this->adjustSize();
}

ShowDownload::~ShowDownload()
{
    delete ui;
    this->deleteLater();
//    qDebug() << "byebye";
}


void ShowDownload::startRequest(QUrl url)
{
        reply = manager->get(QNetworkRequest(url));
        connect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));

        connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                this, SLOT(updateDataReadProgress(qint64, qint64)));

        connect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));

}
void ShowDownload::httpReadyRead()
{
    if (file) file->write(reply->readAll());
}
float temp;
void ShowDownload::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
        ui->progressBar->setMaximum(100);
        downloadPercentage = bytesRead*100.0/totalBytes;
//        qDebug() << "downloadPercentage:" << downloadPercentage;
        ui->progressBar->setValue(downloadPercentage);
        //    qDebug() << "bytesRead:" <<bytesRead;
        //    qDebug() << "temp:" << temp;
        downloadSpeed = (bytesRead - temp)/1024/1024*10;
        ui->label->setText(tr("下载速度：%1MB/s").arg(downloadSpeed));
        ui->label->adjustSize();
//            qDebug() << "speed" << (bytesRead - temp)/1024/1024*10;
        temp = bytesRead;

        qDebug() << tr("percenage:%1,speed:%2").arg(downloadPercentage).arg(downloadSpeed);
        emit updateDownloadInfoSignal(downloadPercentage, downloadSpeed, downloadTime);

}

/**
 * 下载完成
 */
void ShowDownload::httpFinished()
{
    emit downloadFinishedSignal();
    file->flush();
    file->close();
    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
    mutex = -1;
    this->hide();
}

void ShowDownload::newTransferFolder()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString fullPath = tr("%1/接收文件夹/").arg(desktopPath);
    QDir dir(fullPath);
    if(dir.exists()) {
        qDebug() << "文件夹存在";
    } else {
        qDebug() << "文件夹不存在";
        bool ok = dir.mkdir(fullPath);//只创建一级子目录，即必须保证上级目录存在
        if(ok) {
            qDebug() << "文件夹创建成功";
        } else {
            qDebug() << "文件夹创建失败";
        }
    }
}

QString ShowDownload::decodeGBKURL(QString str)
{
    QByteArray array;
    for(int i=0; i<str.length();) {
        if (0==QString::compare(str.mid(i,1), QString("%"))) {
            if ((i+2)<str.length()) {
                array.append(str.mid(i+1,2).toShort(0,16));
                i=i+3;
            }
            else{
                array.append(str.mid(i,1));
                i++;
            }
        }
        else{
            array.append(str.mid(i,1));
            i++;
        }
    }
    QTextCodec *code=QTextCodec::codecForName("GBK");
    return code->toUnicode(array);
}

bool ShowDownload::isDownload(QMap<QString,bool> isDownloadStuList)
{
    for(int i = 0;i < isDownloadStuList.count();i++) {
        if(isDownloadStuList.keys().at(i) == getMACAdress()) {
            if(isDownloadStuList.values().at(i) == true) {
                return true;
            }
        }
    }
    return false;
}

void ShowDownload::startDownload(QString sendUrl)
{
    mutex = 1;
    newTransferFolder();
    QString url = sendUrl;
    QStringList pathList = url.split("/");
    QString fileName = pathList[pathList.count() - 1];
    //        QFileInfo info(url.path());
    //        QString fileName(info.fileName());
    if (fileName.isEmpty()) fileName = "index.html";
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

//    QString decodeFileName = decodeGBKURL(fileName);//因为传过来的是经过URL编码的，所以这里要解码，不然中文会有乱码


    QString saveName = tr("%1/接收文件夹/%2").arg(desktopPath).arg(fileName);
    qDebug() << "saveName:" << saveName;
    file = new QFile(saveName);
    if(!file->open(QIODevice::WriteOnly))
    {
        qDebug() << "file open error";
        delete file;
        file = 0;
        return;
    }
    //        QUrl urlx = sendUrl ;
    startRequest(url);
    ui->progressBar->setValue(0);
    ui->progressBar->show();
}
int s = 0;
int min = 0;
void ShowDownload::on_timeLeft()
{
    if(mutex == 1) {
        if(s < 60) {
            s+=1;
        }
        if(s >= 60) {
            min += 1;
            s = 0;
        }
        downloadTime = tr("下载时间：%1分%2秒").arg(min).arg(s);
        qDebug() << downloadTime;
        ui->label_time->setText(downloadTime);
        ui->label_time->adjustSize();
    }
}

// 停止下载工作;
void ShowDownload::stopWork()
{
    if (reply != NULL) {
        disconnect(reply, SIGNAL(readyRead()), this, SLOT(httpReadyRead()));

        disconnect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                this, SLOT(updateDataReadProgress(qint64, qint64)));

        disconnect(reply, SIGNAL(finished()), this, SLOT(httpFinished()));
        reply->abort();
        reply->deleteLater();
        reply = NULL;
    }
}
void ShowDownload::on_pushButton_clicked()
{

}

void ShowDownload::on_pushButton_cancel_clicked()
{
    stopWork();
}

//获得MAC地址
QString ShowDownload::getMACAdress()
{
    QList<QNetworkInterface> NetList;//网卡链表
    int NetCount = 0;//网卡个数
    int Neti=0;
    QNetworkInterface thisNet;//所要使用的网卡

    NetList = QNetworkInterface::allInterfaces();//获取所有网卡信息
    NetCount = NetList.count();//统计网卡个数

    for(Neti = 0; Neti < NetCount; Neti++){//遍历所有网卡
        if( NetList[Neti].isValid() ){//判断该网卡是否是合法
            thisNet = NetList[Neti];//将该网卡置为当前网卡
            break;
        }
    }
    return(thisNet.hardwareAddress());//获取该网卡的MAC
}
