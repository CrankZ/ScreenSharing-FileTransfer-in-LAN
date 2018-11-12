#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    receiver = new Receiver;
    sender = new Sender;

    int oneSec = 1000;    //一秒，这个一般不修改1000
    qTime = new QTimer;
    qTime->start(5*oneSec); //start为QTIME自己的函数
    connect(qTime,&QTimer::timeout,
            this,&Widget::on_timeLeft);
//    this->hide();//这个主Widget是main函数打开的，所以不能在这里隐藏

    sd = new ShowDownload;
//    sd->show();

    showScreen = new ShowScreen;
    screenSharing = new ScreenSharing(1);
    prohibitor = new Prohibitor;
    //    sendStuName();//上线主动发送学生姓名


    connect(sd, &ShowDownload::updateDownloadInfoSignal,
            this, &Widget::sendDownloadInfoSlot);

    connect(sd, &ShowDownload::downloadFinishedSignal,
            this, &Widget::downloadFinishedSlot);

    connect(receiver, &Receiver::tScreen1Signal,
            this, &Widget::tScreen1Slot);

    connect(receiver, &Receiver::tCursorPosSignal,
            this, &Widget::tCursorPosSlot);

    connect(receiver, &Receiver::tCloseScreenSignal,
            this, &Widget::tCloseScreenSlot);

    connect(receiver, &Receiver::tDownloadSignal,
            this, &Widget::tDownloadSlot);

    connect(receiver, &Receiver::tOpenScreenSignal,
            this, &Widget::tOpenScreenSlot);

    connect(receiver, &Receiver::tHeartBeatSignal,
            this, &Widget::tHeartBeatSlot);
    connect(receiver, &Receiver::tWatchYouSignal,
            this, &Widget::tWatchYouSlot);

    connect(receiver, &Receiver::tMasterIPSignal,
            this, &Widget::tMasterIPSlot);

    connect(receiver, &Receiver::tStopWatchSignal,
            this, &Widget::tStopWatchSlot);

    connect(receiver, &Receiver::tExamPaperInfoSignal,
            this, &Widget::tExamPaperInfoSlot);

    connect(receiver, &Receiver::tExamStartSignal,
            this, &Widget::tExamStartSlot);

    connect(receiver, &Receiver::tRemoveStuInfoSignal,
            this, &Widget::tRemoveStuInfoSlot);


    connect(receiver, &Receiver::tCheckInSignal,
            this, &Widget::tCheckInSlot);

    connect(receiver, &Receiver::tExamConfirmSignal,
            examTest, &ExamTest::tExamConfirmSlot);

    this->setLayout(ui->verticalLayout);

    //开机启动
    setAutoStart(true);

    // 解除限制
    prohibitor->hook(false);
    prohibitor->disableTaskManager(false);
    prohibitor->hideTaskWindow(false);

    // 系统托盘
    startTrayIcon();
    //软件过期
//    expireDate("2019-05-05");
}

Widget::~Widget()
{
    delete ui;
    prohibitor->disableTaskManager(false);
    prohibitor->hook(false);
    prohibitor->hideTaskWindow(false);
}

void Widget::tExamStartSlot()
{
    examTest->startExam();
}

void Widget::tExamPaperInfoSlot(ExamInfo *examInfo)
{
    killProcess();
//    qDebug() << "examInfo->paperChoice:" << examInfo->paperChoice;
    examTest->deleteLater();
    examTest = new ExamTest();

    connect(receiver, &Receiver::tExamConfirmSignal,
            examTest, &ExamTest::tExamConfirmSlot);
    examTest->DealWithExamInfo(examInfo);

    examTest->showMaximized();  // 窗口最大化
    examTest->setWindowState(Qt::WindowFullScreen);   // 全屏且无标题栏
//    examTest->setWindowFlags (Qt::WindowStaysOnTopHint);    // 强制置顶
    examTest->raise();
    examTest->show();
}

//用来杀死屏幕取词软件
void Widget::killProcess()
{
    QProcess *process=new QProcess;
    process->execute("TASKKILL /IM PowerWord.exe /F");//金山
    process->execute("TASKKILL /IM YoudaoDict.exe /F");//有道
    process->execute("TASKKILL /IM Lingoes.exe /F");//Lingoes
}

/**
 * 托盘相关
 */
void Widget::startTrayIcon()
{
    //托盘初始化
    QIcon icon = QIcon(":/resource/img/boy.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("XX电子教室beta1   --学生端");
    QString title = "XX电子教室";
    QString text = "启动成功";
    trayIcon->show();
    trayIcon->showMessage(title,text,QSystemTrayIcon::Information,5000); //最后一个参数为提示时长，默认10000，即10s

    trayIcon->show(); //必须调用，否则托盘图标不显示

    restartAction = new QAction("重启本程序", this);
    aboutMeAction = new QAction("关于",this);

    connect(restartAction, SIGNAL(triggered()), this, SLOT(restart()));
    connect(aboutMeAction, SIGNAL(triggered()), this, SLOT(aboutMe()));

    //创建托盘菜单(必须先创建动作，后添加菜单项，还可以加入菜单项图标美化)
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(restartAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(aboutMeAction);

    trayIcon->setContextMenu(trayIconMenu);

    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this,SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

void Widget::downloadFinishedSlot()
{
    showTrayMessage("文件接收成功，请查看桌面“接收文件夹”");
}

void Widget::showTrayMessage(QString msg)
{
    QString title = "XX电子教室";
    QString text = msg;
    trayIcon->show();
    trayIcon->showMessage(title,text,QSystemTrayIcon::Information,5000);
}


void Widget::skinLoad()
{
    // 加载CSS文件
    QString absltPath = QCoreApplication::applicationDirPath();
    qDebug()<< "absltPath:" << absltPath;
    QString strCssFile = "D:/cm.css";
    QFile fCss(strCssFile);
    if( !fCss.open(QFile::ReadOnly))
    {
        //        qDebug() << tr("css File %1 load false").arg(strCssFile);
        return;
    }
    QString strCssContent(fCss.readAll());
    setStyleSheet(strCssContent);
    fCss.close();
}

void Widget::sendAlive()
{
    //CM_sAlive + MAC地址
    //    qDebug() << "MAC:" << getMACAdress();
    QString MACAndStuClass = tr("%1$%2").arg(getMACAdress()).arg(getStuClass());
    sender->sendCommand(CM_sAlive,MACAndStuClass);
}

//int num = 5;

// 如果master没有发heartbeat则关闭
void Widget::on_timeLeft()
{
    // 如果收到关闭的指令，则直接返回
    if(isReceivedClose) {
        ::SetThreadExecutionState( ES_CONTINUOUS );
        prohibitor->hook(false);
        prohibitor->disableTaskManager(false);
        prohibitor->hideTaskWindow(false);

        return;
    }

    // 收到屏幕演示的指令之后，禁止学生所有操作
    // 只在真实情况下启用，
    if(istScreen && false) {
        if(justOnce) {
            ::SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);//阻止睡眠
            prohibitor->hook(true);
            prohibitor->disableTaskManager(true);
            prohibitor->hideTaskWindow(true);
            justOnce = false;
        }
    }

    if(showScreen->isHidden()) {
        if(!justOnce) {
            qDebug() << "如果ss隐藏，则fh就解锁";
            ::SetThreadExecutionState( ES_CONTINUOUS );
            prohibitor->hook(false);
            prohibitor->disableTaskManager(false);
            prohibitor->hideTaskWindow(false);
            justOnce = true;
        }
        justOnce = true;
    }

    // Master不在时该做的事情
    if(isMasterOnline == false) {
        // 为了防止教师端在监控学生的时候意外退出。也就是说学生端还在发送屏幕数据，没有接收到停止的指令。
        // 这个时候，检测到教师端已经不在线，就应该关闭
        tStopWatchSlot();
    }

    istScreen = false;
    isMasterOnline = false;
}

void Widget::restart()
{
    qDebug() << "restart";
    qApp->exit();
    QProcess::startDetached(qApp->applicationFilePath(), QStringList());
}

void Widget::aboutMe()
{
    QMessageBox::about(this,"关于我","QQ:331714375\nEmail:crankzcool@gmail.com");
}


/**
 * 发送下载信息
 */
void Widget::sendDownloadInfoSlot(float downloadPercentage,float downloadSpeed, QString downloadTime)
{
    qDebug() << "out_percentage:" << downloadPercentage;
    qDebug() << "out_speed:" << downloadSpeed;
    qDebug() << "out_time:" << downloadTime;

    //1是正在下载
    if(sd->mutex == 1) {
        dlInfo = tr("M$%1$N$%2$D$%3$P$%4$S$%5$T$%6")
                .arg(getMACAdress())
                .arg(getStuName())
                .arg("downloading")
                .arg(downloadPercentage)
                .arg(downloadSpeed)
                .arg(downloadTime);
        sender->sendCommand(CM_sDownloadInfo,dlInfo);
        qDebug() << "正在下载：" << dlInfo;
    }
    if(sd->mutex == -1) {
        qDebug() << "下载完成";
        dlInfo = tr("M$%1$N$%2$D$%3$P$%4$S$%5$T$%6")
                .arg(getMACAdress())
                .arg(getStuName())
                .arg("downloadFinished")
                .arg(100.00)
                .arg(0.00)
                .arg(downloadTime);
        sender->sendCommand(CM_sDownloadInfo,dlInfo);
        sd->mutex = 0;
        dlInfo = "";

        QString title="下载完成";
        QString text="请在桌面“接收文件夹”内查看文件";
        trayIcon->show();
        trayIcon->showMessage(title,text,QSystemTrayIcon::Information,5000); //最后一个参数为提示时长，默认10000，即10s

        //        disconnect(qTime,SIGNAL(timeout()),this,SLOT(on_timeLeft()));
    }
}

QString Widget::getStuName()
{
    QString readName = "stuInfo.txt";

    QString strLocalHostName = QHostInfo::localHostName();
    //定义文件对象
    QFile fileIn(readName);
    if(!fileIn.open(QIODevice::ReadOnly)) {
//        qDebug() << "文件不存在";
        return strLocalHostName;
    } else {
        QByteArray ba = fileIn.readAll();
        QString sAll = QString::fromUtf8(ba);
        sAll = decodeStr(sAll);
        if(sAll.contains("$")) {
            QStringList sList = sAll.split("$");

//            qDebug() << "sList:" << sList;
            QString stuName = sList[1];
            if(!stuName.isEmpty()) {
//                qDebug() << "stuName:" << stuName;
                return stuName;
            }
        }
    }
    return strLocalHostName;
}

QString Widget::getStuClass()
{
    QString readName = "stuInfo.txt";

    QFile fileIn(readName);
    if(!fileIn.open(QIODevice::ReadOnly)) {
//        qDebug() << "文件不存在";
        return "";
    } else {
        QByteArray ba = fileIn.readAll();
        QString sAll = QString::fromUtf8(ba);
        sAll = decodeStr(sAll);
        if(sAll.contains("$")) {
            QStringList sList = sAll.split("$");

            QString className = sList[2];
            if(!className.isEmpty()) {
//                qDebug() << "className:" << className;
                return className;
            }
        }
    }
    return "";
}



void Widget::deleteShow()
{
    //关闭窗口实例后，并没有真正的关闭窗口实例，而仅仅是隐藏起来了，带来了一些麻烦
    //这个函数就是彻底关闭掉窗口实例
    //    qDebug() << "sd-> ()->className()" << sd->metaObject()->className();
    //    if(QString(sd->metaObject()->className()) == "ShowDownload")
    //    {
    //        if(sd->isHidden())
    //        {
    //            sd->deleteLater();
    //            sd = new ShowDownload;
    //        }
    //    }
    //    if(QString(showScreen->metaObject()->className()) == "ShowScreen")
    //    {
    //        if(showScreen->isHidden())
    //        {
    //            showScreen->deleteLater();
    //            ss = new ShowScreen;
    //        }
    //    }
}



//获得MAC地址
QString Widget::getMACAdress()
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

void Widget::closeEvent(QCloseEvent *event)
{
    //重写关闭代码
    //    if(!isClose)
    //    {
    //        QMessageBox::StandardButton button;
    //        button = QMessageBox::question(this, tr("退出程序"),
    //                                       QString(tr("退出后将无法继续考试，是否退出？")),
    //                                       QMessageBox::Yes | QMessageBox::No);

    //        if (button == QMessageBox::No) {
    //            event->ignore();  //忽略退出信号，程序继续运行
    //        }
    //        else if (button == QMessageBox::Yes) {
    //            event->accept();  //接受退出信号，程序退出
    //        }

    //    }
    //    else
    //    {
    //        QMessageBox::warning(this,tr("禁止关闭"),tr("考试未结束，禁止关闭程序！"));
    //        event->ignore();
    //    }
}

void Widget::on_pushButton_hide_clicked()
{
//    this->hide();
    prohibitor->hook(false);
    prohibitor->disableTaskManager(false);
    prohibitor->hideTaskWindow(false);
}


/**
 * 开机自启
 */
void Widget::setAutoStart(bool is_auto_start)
{
    QString REG_RUN = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
    QString application_name = QApplication::applicationName();
    QSettings *settings = new QSettings(REG_RUN, QSettings::NativeFormat);
    if(is_auto_start) {
        QString application_path = QApplication::applicationFilePath();
        settings->setValue(application_name, application_path.replace("/", "\\"));
    } else {
        settings->remove(application_name);
    }
    delete settings;
}


void Widget::getCurrentTimeFromWeb()
{
    QTcpSocket *socket = new QTcpSocket();
    socket->connectToHost("time.nist.gov", 13);
    if (socket->waitForConnected()) {
        if (socket->waitForReadyRead()) {
            QString str(socket->readAll());
            str = str.trimmed();
            qDebug() << str;
            //"58424 18-11-02 06:04:48 03 0 0 885.9 UTC(NIST) *"
            str = str.section(" ", 1, 1);
            str = "20" + str;
            qDebug() << str;
            //"18-11-02 06:04:48"
        }
    }
    socket->close();
    delete socket;
}


void Widget::expireDate(QString endDateStr)
{
    //获取系统当前时间
    QDateTime currentDate = QDateTime::currentDateTime();
    QDateTime endDate = QDateTime::fromString(endDateStr, "yyyy-MM-dd");

    qint64 nSecs = endDate.secsTo(currentDate);
    if (nSecs > 0) {
        QMessageBox::about(this,"软件过期","软件过期，请购买正版\nQQ：331714375\nEmail:331714375@qq.com");
        this->deleteLater();
    }
}
QString Widget::getHostIPV4Address()
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

QString Widget::decodeStr(QString strEncode)
{
    QByteArray ba = strEncode.toUtf8();
    QString strDecode = QString::fromUtf8(QByteArray::fromBase64(ba));
    return strDecode;
}



void Widget::tOpenScreenSlot()
{
    qDebug() << "tOpenScreenSlot";
    showScreen->show();
    isReceivedClose = false;
    istScreen = true;
}

void Widget::tCloseScreenSlot()
{
    // 隐藏就行了，没有必要退出
    // 因为教师端不发送数据，并没有什么损失
    showScreen->hide();
    // 如果收到close的命令，则强制关闭
    isReceivedClose = true;
    istScreen = false;
}


void Widget::tDownloadSlot(QString url, QMap<QString, bool> isDownloadStuList)
{
    qDebug() << "CM_tDownload";
    qDebug() << "url:" << url;
    //            sd->startDownload(url);
    //            sd->show();

    if(sd->isDownload(isDownloadStuList)) {
        sd->startDownload(url);
        sd->show();
    } else {
        qDebug() << "接收到下载命令，但是不允许下载";
    }
    //            sd->startDownload(url);
}

void Widget::tHeartBeatSlot()
{

//    sendStuName();//姓名本不应该一直获取，暂时先放在这里吧
    //如果是教师端不关闭，上线就获取姓名，然后就可以关闭
    //但是再测试阶段，基本上，打开后还是会关闭的，那就麻烦了
    //学生端基本不关闭，如果只能发送一次，不符合实际
    isMasterOnline = true;

    sendStuInfo();
}

void Widget::sendStuName()
{
    QString stuName = getStuName();
//    qDebug() << "stuName:" << stuName;
    QString addMAC = tr("%1$%2").arg(getMACAdress()).arg(stuName);
    sender->sendCommand(CM_sStuName,addMAC);
}

void Widget::sendStuInfo()
{
    QString stuMAC = getMACAdress();
    QString stuName = getStuName();
    QString stuClass = getStuClass();


    StuInfo *stuInfo = new StuInfo;
    stuInfo->stuMAC = stuMAC;
    stuInfo->stuName = stuName;
    stuInfo->stuClass = stuClass;
    stuInfo->stuIsAlive = true;

    sender->sendStuInfo(CM_sStuInfo, stuInfo);
}

void Widget::tWatchYouSlot(QMap<QString,int> stuWatchList)
{
    for(int i = 0;i < stuWatchList.count(); i++) {
        QString MAC = stuWatchList.keys().at(i);
        int fourPart = stuWatchList.values().at(i);
        qDebug() << "fourPart:" << fourPart;
        if(MAC == getMACAdress()) {
            // -1就是不监控
            if (fourPart == -1) {
                screenSharing->pause();
                screenSharing->quit();
                return;
            }

            screenSharing->fourPart = fourPart;
            screenSharing->stuName = getStuName();
            screenSharing->pause();
            screenSharing->sleep(1);
            screenSharing->start();
            screenSharing->goOn();
        }
    }
}

void Widget::tStopWatchSlot()
{
    screenSharing->pause();
    screenSharing->quit();
}

void Widget::tMasterIPSlot(QString masterIP)
{
    qDebug() << "masterIP:" << masterIP;
    screenSharing->destIP = masterIP;
}

void Widget::on_pushButton_clicked()
{
    screenSharing->quit();
    screenSharing->pause();
}


/**
 * 这个是真的接收屏幕传输的数据的
 */
void Widget::tScreen1Slot(QByteArray bytes)
{
    if (isReceivedClose) {
        return;
    }
    istScreen = true;
    showScreen->show();
    showScreen->screenRecv(bytes,1);
    // 如果有数据就打开，没有就关闭
    // 这个函数进来说明肯定有数据

    // 打开不能放这里，因为数据传输不会立即终端，会延迟一段时间
//    showScreen->show();
}

void Widget::tPixmapSlot(QPixmap pixmap)
{
    label.setPixmap(pixmap);
    label.show();
}

void Widget::tCursorPosSlot(QSize sourceScreenSize,QString cursorType, QPoint masterCursorPos)
{
    showScreen->drawCursor(sourceScreenSize, cursorType, masterCursorPos);
}

// 删除学生数据
void Widget::tRemoveStuInfoSlot()
{
    qDebug() << "学生数据清除成功";
    QFile::remove("stuInfo.txt");
}

void Widget::tCheckInSlot(QList<QString> classNameList)
{
    CheckIn *checkIn = new CheckIn;
    //如果姓名文件已经存在了，就不弹出窗口了
    if(!checkIn->isFileExist()) {
        checkIn->addClassNameItem(classNameList);
        checkIn->show();
    }
}
