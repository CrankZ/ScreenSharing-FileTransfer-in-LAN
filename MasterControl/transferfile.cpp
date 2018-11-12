#include "transferfile.h"
#include "ui_transferfile.h"
#include <QLine>

TransferFile::TransferFile(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TransferFile)
{
    ui->setupUi(this);

    receiver = new Receiver();
    sender = new Sender();
    checkOnline = new CheckOnline();
    httpServer = "pyhttpserver.exe";

    this->stuAliveMap = checkOnline->stuAliveMap;
    this->setLayout(ui->verticalLayout_5);
    ui->scrollArea->setLayout(ui->verticalLayout_2);
    ui->scrollArea->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    ui->scrollArea->setWidgetResizable(true);

    QWidget* widget_noting = new QWidget(this);
    widget_noting->setLayout(ui->verticalLayout_2);
    ui->scrollArea->setWidget(widget_noting);
//    setLayout(ui->horizontalLayout);
//    ui->horizontalLayout_2->setAlignment(Qt::AlignTop);
    this->stuAliveMap = checkOnline->stuAliveMap;
    this->stuInfoMap = checkOnline->stuInfoMap;

//    ui->splitter_sendFile->hide();
    setAcceptDrops(true);
    addDownloadItem();
}


TransferFile::~TransferFile()
{
    delete ui;
}

/**
 * 打开软件界面的时候，添加所有学生控件
 * 根据stuAliveMap来显示在线学生
 */
void TransferFile::addDownloadItem()
{
    if(lSplitter.count() >= stuAliveMap.count()) {
        return;
    }
    for(int i = 0;i < stuAliveMap.count();i ++) {
        StuInfo *stuInfo = stuInfoMap.values().at(i);
        QString stuName = stuInfo->stuName;
        QString stuMAC = stuInfo->stuMAC;
        lCb.append(new QCheckBox(this));

        lLabel1.append(new QLabel(this));
        lProgressBar.append(new QProgressBar(this));
        lLabel2.append(new QLabel(this));
        lSplitter.append(new QSplitter(this));

        lCb.at(i)->setCheckState(Qt::Checked);

        lLabel1.at(i)->setText(tr("主机名：%1").arg(stuName));
        lSplitter.at(i)->setObjectName(stuMAC);
        lProgressBar.at(i)->setMaximum(100);
        lProgressBar.at(i)->setValue(0);
        lLabel2.at(i)->setText("传输速度：+ 传输时间：");

        lSplitter.at(i)->addWidget(lCb.at(i));

        lSplitter.at(i)->addWidget(lLabel1.at(i));
        lSplitter.at(i)->addWidget(lProgressBar.at(i));
        lSplitter.at(i)->addWidget(lLabel2.at(i));

        lSplitter.at(i)->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);

        //默认是居中显示，下面就是为了让他从上到下显示
        //因为Splitter没有setAlignment()函数，所以套了一层Layout
        QHBoxLayout *hLayout = new QHBoxLayout;
        hLayout->setAlignment(Qt::AlignTop);

        lSplitter.at(i)->setFrameShape(QFrame::StyledPanel);
        hLayout->addWidget(lSplitter.at(i));

        ui->verticalLayout_2->addLayout(hLayout);
        ui->verticalLayout_2->setAlignment(Qt::AlignTop);
        //        ui->verticalLayout_2->addWidget(lSplitter.at(i));//以前直接这一行就行了

    }
}

/**
 * 显示学生下载的进度等相关内容
 */
void TransferFile::sDownloadInfoSlot(QString downLoadInfoAll)
{
    qDebug() << "downLoadInfoAll:" << downLoadInfoAll;
//    receiver->bool_downLoadInfo = false;//接收时会设置成true，意思就是如果不继续接收，就为false

    QStringList downLoadInfoList = downLoadInfoAll.split("$");
    QString MAC = downLoadInfoList[1];
    QString stuName = downLoadInfoList[3];
    QString downloadFlag = downLoadInfoList[5];
    QString downLoadPercentage = downLoadInfoList[7];
    QString downLoadSpeed = downLoadInfoList[9];
//    QString downloadTime = downLoadInfoList[11];//时间多次传输时有问题，暂时不搞
    QString downloadTime = "0";
    qDebug() << "MAC:" << MAC << endl
             << "stuName:" << stuName << endl
             << "flag:" << downloadFlag << endl
             << "downLoadPercentage:" << downLoadPercentage << endl
             << "speed:" << downLoadSpeed << endl
             << "time:" << downloadTime;
    qDebug() << "count:" << lSplitter.count();

    for(int i = 0;i < lSplitter.count(); i++) {

        if(MAC == lSplitter.at(i)->objectName()) {
            if(downloadFlag == "downloadFinished") {
                lProgressBar.at(i)->setValue(100);
                qDebug() << "tMain_下载完成";
            } else if(downloadFlag == "downloadStop") {

                qDebug() << "transferfile_传输终止";
            } else {



                //显示学生端的下载信息
                lProgressBar.at(i)->setValue(downLoadPercentage.toDouble());

                lLabel1.at(i)->setText(stuName);
                lLabel1.at(i)->adjustSize();

                lLabel2.at(i)->setText(tr("传输速度%1MB/s").arg(downLoadSpeed) + downloadTime);
                lLabel2.at(i)->adjustSize();
                lSplitter.at(i)->adjustSize();
            }
        }
    }
}

void TransferFile::on_toolButton_selectFile_clicked()
{
    setIsDownloadStuList();
    killProcess(httpServer);
    //获取将要打开的文件名
    QString filePath = QFileDialog::getOpenFileName(
                this,
                tr("打开配置文件"),
                tr("."),
                tr("All files(*)")
                );
    if(!filePath.isEmpty()) {
        sendFile(filePath);
    }
}

void TransferFile::sendFile(QString filePath)
{
    QString httpPort = "8866";
    setIsDownloadStuList();
    //定义文件对象
    qDebug() << "filePath:" << filePath;
    QFile fileIn(filePath);
    if(!fileIn.open(QIODevice::ReadOnly)) {
        return; //不处理文件
    } else {
        QStringList pathList = filePath.split("/");
        QString disk = pathList[0];
//            qDebug() << "disk:" << disk;
        int pathListCount = pathList.count() - 1;
        QString fileName = pathList[pathListCount];
        QStringList pathList2 = filePath.split(fileName);
        QString folder = pathList2[0];
        // 带参数启动
        QString program = httpServer;
        QStringList arguments;
//        arguments << "-r" << folder << "-p" << httpPort;
        arguments << folder << httpPort;

        QProcess *myProcess = new QProcess();
        myProcess->start(program, arguments);
        //url编码
//        QByteArray temp = fileName.toLocal8Bit();
//        fileName = temp.toPercentEncoding();


        QString downloadUrl = tr("http://%1:%2/%3").arg(getHostIPV4Address()).arg(httpPort).arg(fileName);
        qDebug() << "downloadUrl:" << downloadUrl;

        // 发送下载指令
        sender->sendDownloadInfo(CM_tDownload,downloadUrl,isDownloadStuList);
        //            process->close();//不能关闭，否则打不开
    }

}

void TransferFile::on_checkBox_selectAllStu_clicked()
{
    for(int i = 0;i < lSplitter.count();i++) {
        QCheckBox *cb = (QCheckBox*)lSplitter.at(i)->widget(0);
        // 单击这个checkBox之后，如果是打勾状态，则全选。
        if(ui->checkBox_selectAllStu->isChecked()) {
            cb->setCheckState(Qt::Checked);
        } else {
            cb->setCheckState(Qt::Unchecked);
        }
    }
}

/**
 * 把主界面打勾的学生放到list里
 * 因为发送是组播发送的，并不是单独每个学生
 */
void TransferFile::setIsDownloadStuList()
{
    for(int i = 0;i < lSplitter.count();i++) {
        QCheckBox *checkBox = (QCheckBox*)lSplitter.at(i)->widget(0);
        QString MAC = lSplitter.at(i)->objectName();
        if(checkBox->isChecked()) {
            isDownloadStuList.insert(MAC,true);
        } else {
            isDownloadStuList.insert(MAC,false);
        }
    }
}

QString TransferFile::getHostIPV4Address()
{
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);

    foreach(QHostAddress address,info.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol) {
            qDebug() << address.toString();
            return address.toString();
        }
    }
}

void TransferFile::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void TransferFile::dropEvent(QDropEvent *e)
{
    setIsDownloadStuList();
    killProcess(httpServer);
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString filePath = url.toLocalFile();
        if(filePath.isEmpty()) {
            //空字符串不处理，返回
            return;
        } else {
            sendFile(filePath);
        }
    }
}


/**
 * 结束进程
 */
void TransferFile::killProcess(QString processName)
{
    QProcess *process = new QProcess;
    process->execute(tr("taskkill /fi \"imagename eq %1\" /f").arg(processName));
}

void TransferFile::on_toolButton_stop_clicked()
{
    killProcess(httpServer);
}
