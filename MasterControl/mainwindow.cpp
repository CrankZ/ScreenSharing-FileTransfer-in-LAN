#include "mainwindow.h"


//QMap<QString, bool> MainWindow::stuWatchList;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralWidget->setLayout(ui->verticalLayout_main);
    // 隐藏菜单栏
    this->menuBar()->hide();
//    this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);

    // 设置这些属性是给QSS用的
    this->setProperty("form", true);
    ui->widgetTop->setProperty("form", "title");
    ui->widgetLeftMain->setProperty("flag", "left");

    // 左侧按钮，载入fontanwsome
    QList<int> pixCharMain;
    QList<QToolButton *> btnsMain;
    pixCharMain << 0xf007 << 0xf009 << 0xf03a;
    btnsMain << ui->tbtnMain1 << ui->tbtnMain2 << ui->tbtnMain3;
    IconHelper::Instance()->setStyle(ui->widgetLeftMain, btnsMain, pixCharMain, 15, 35, 25, "left", 4);

    // 载入皮肤css文件
    skinLoad();

    // 流动布局
    flowLayout = new FlowLayout();
    checkOnline = new CheckOnline();
    qTime = new QTimer();
    changeStuOnlineStateTimer = new QTimer();
    randomTimer = new QTimer();
    sender = new Sender();
    receiver = new Receiver();
    screenSharing1 = new ScreenSharing(1);
    screenSharing2 = new ScreenSharing(2);
    transferFile = new TransferFile();   //这种方式可以操作父窗口

    // 子窗口打开时不可操作父窗口
    transferFile->setWindowModality(Qt::ApplicationModal);


    qTime->start(1000); //start为QTIME自己的函数
    // 这个的时间一定要大于CheckOnline里面的setAliveFalse函数，因为太快的话，肯定会捕获到那个set的那个false
    changeStuOnlineStateTimer->start(5000);
    connect(qTime,&QTimer::timeout,
            this, &MainWindow::on_timeLeft);
    //    on_timeLeft();
    //    randomTimer->start(5*oneSec); //start为QTIME自己的函数
    connect(randomTimer,&QTimer::timeout,
            this, &MainWindow::randomTimerSlot);

    connect(changeStuOnlineStateTimer,&QTimer::timeout,
            this, &MainWindow::changeStuOnlineStateTimerSlot);

    checkOnline->start();

    connect(receiver, &Receiver::sStuInfoSignal,
            checkOnline, &CheckOnline::sStuInfoSlot);

    connect(receiver, &Receiver::sStuInfoSignal,
            this, &MainWindow::sStuInfoSlot);

    connect(receiver, &Receiver::sDownloadInfoSignal,
            transferFile, &TransferFile::sDownloadInfoSlot);

    connect(receiver, &Receiver::sScreenSignal,
            this, &MainWindow::sScreenSlot);

    connect(monitorScreen, &MonitorScreen::startWatchingSignal,
            this, &MainWindow::startWatchingSlot);
    
    connect(monitorScreen, &MonitorScreen::stopWatchingSignal,
            this, &MainWindow::stopWatchingSlot);

    connect(monitorScreen, &MonitorScreen::watchSingleStuScreenSignal,
            this, &MainWindow::watchSingleStuScreenSlot);

    connect(showScreen, &ShowScreen::showScreenCloseSignal,
            this, &MainWindow::showScreenCloseSlot);

    connect(ui->action_About,SIGNAL(triggered()),
            this, SLOT(showAbout()));
    
    stuAliveMap.clear();

    // scrollArea只能放Widget，所以还要包装一下
    QWidget *flowWidget = new QWidget;
    flowWidget->setLayout(this->flowLayout);
    ui->scrollArea->setWidget(flowWidget);
    ui->scrollArea->setWidgetResizable(true);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    startTrayIcon();//启动托盘
//    expireDate("2019-05-05");//软件过期
    //    ui->splitter_testSystem->setContextMenuPolicy(Qt::CustomContextMenu);
    //    connect(ui->splitter_testSystem, SIGNAL(customContextMenuRequested(const QPoint &)),
    //            this, &MainWindow::ctxMenu(const QPoint &)));

    // 学生控件的右键菜单
    actionWatch = new QAction("查看屏幕");
    connect(actionWatch, SIGNAL(triggered()),
            this, SLOT(watchStuScreen()));

    getCurrentTimeFromWeb();


}

bool MainWindow::eventFilter(QObject * obj, QEvent * event)
{
    if(event->type() == QEvent::MouseButtonRelease)//单击事件
    {
        this->obj = obj;
    }
    return false;
}

/**
 * 通过获取当前单击的学生控件的，objectName，也就是MAC地址
 * 来监控学生
 */
void MainWindow::watchStuScreen()
{
    qDebug() << "obj->objectName():" << obj->objectName();
    for(int i = 0;i < stuWatchList.count();i++)
    {
        QString MAC = stuWatchList.keys().at(i);
        stuWatchList.insert(MAC,-1);
    }
    QString myMAC = obj->objectName();
    stuWatchList.insert(myMAC,666);

    showScreen->show();
    sender->sendCommand(CM_tMasterIP,getHostIPV4Address());
    sender->sendMap(CM_tWatchYou,stuWatchList);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *evt)
{
    //    qDebug() << "evt:" << evt;
    //    emit clicked();
}

/**
 * 安排后事
 */
MainWindow::~MainWindow()
{
    delete ui;
    stopWatchingSlot();
}

void MainWindow::showAbout()
{
    About *a = new About();
    a->show();
}

/**
 * 托盘菜单
 */
void MainWindow::startTrayIcon()
{
    //托盘初始化
    QIcon icon = QIcon(":/resource/img/resource/img/boy.png");
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("XX管理系统beta1");
    trayIcon->show(); //必须调用，否则托盘图标不显示

    //创建菜单项动作(以下动作只对windows有效)
    //    minimizeAction = new QAction("最小化~", this);
    //    connect(minimizeAction, SIGNAL(triggered()), this, &MainWindow::hide())); //绑定信号槽

    //    maximizeAction = new QAction("最大化~", this);
    //    connect(maximizeAction, SIGNAL(triggered()), this, &MainWindow::showMaximized()));

    restoreAction = new QAction("显示窗口", this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction("退出", this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit())); //关闭应用，qApp对应的是程序全局唯一指针

    //创建托盘菜单(必须先创建动作，后添加菜单项，还可以加入菜单项图标美化)
    trayIconMenu = new QMenu(this);
    //    trayIconMenu->addAction(minimizeAction);
    //    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);
    trayIcon->setContextMenu(trayIconMenu);


    connect(trayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
}

//  托盘icon事件
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
    case QSystemTrayIcon::Trigger:
        //        trayIcon->showMessage("title","你单击了"); //后面两个默认参数
        showNormal();
        break;
    case QSystemTrayIcon::DoubleClick:
        //        trayIcon->showMessage("title","你双击了");
        showNormal();
        break;
    case QSystemTrayIcon::MiddleClick:
        //        trayIcon->showMessage("title","你中键了");
        showNormal();
        break;
    default:
        break;
    }
}


void MainWindow::skinLoad()
{
    QString cssFile = ":/qss/resource/psblack.css";
    QFile fCss(cssFile);
    if( !fCss.open(QFile::ReadOnly))
    {
        qDebug() << "css载入失败";
        return;
    }
    QString css = fCss.readAll();
    setStyleSheet(css);
    fCss.close();
}

/**
 * 清除主界面所有的控件
 */
void MainWindow::clearLayout(QLayout* layout, bool deleteWidgets)
{
    if(false)
    {
        while (QLayoutItem* item = layout->takeAt(0))
        {
            if (deleteWidgets)
            {
                if (QWidget* widget = item->widget())
                    widget->deleteLater();
            }
            if (QLayout* childLayout = item->layout())
                clearLayout(childLayout, deleteWidgets);
            delete item;
        }
    }
}

/**
 * 负责主界面学生上下线状态的修改
 * 还有下载在线人数的label
 * 这个频率不能太快，因为有一个setFalse的函数，肯定会有一秒钟为false
 */
void MainWindow::changeStuOnlineStateTimerSlot()
{
    QMap<QString, StuInfo*> stuInfoMap = checkOnline->stuInfoMap;
    QMap<QString, bool> stuAliveMap = checkOnline->stuAliveMap;

    int stuCount = stuInfoMap.count();
    for (int i = 0;i < stuInfoMap.count(); i++) {
        StuInfo *stuInfo = stuInfoMap.values().at(i);
        QString stuMAC = stuInfo->stuMAC;
        QString stuNo = stuInfo->stuNo;
        QString stuName = stuInfo->stuName;
        // 更改stuInfoMap有点费劲还是，所以又搞了一个map。。。
        bool stuIsAlive = stuInfo->stuIsAlive;
        stuIsAlive = stuAliveMap.value(stuMAC);
        if(!stuIsAlive) {
            stuCount -= 1;
        }
        for(int i = 0;i < lSpilllter.count();i++) {
            if(lSpilllter.at(i)->objectName() == stuMAC) {
                // 改变在线状态
                if(stuIsAlive == true) {
                    if(lSpilllter.at(i)->palette().background().color() != Qt::white){
                        QPalette pal = palette();
                        pal.setColor(QPalette::Background, Qt::white);
                        lSpilllter.at(i)->setAutoFillBackground(true);
                        lSpilllter.at(i)->setPalette(pal);
                    }
                } else {
                    if(lSpilllter.at(i)->palette().background().color() != Qt::gray){

                        QPalette pal = palette();
                        pal.setColor(QPalette::Background, Qt::gray);
                        lSpilllter.at(i)->setAutoFillBackground(true);
                        lSpilllter.at(i)->setPalette(pal);
                    }
                }
                // 改变学生名称
                QLabel *labelName = (QLabel*)lSpilllter.at(i)->widget(1);
                QString currentStuName = labelName->text();
                if(currentStuName != stuName) {
                    labelName->setText(stuName);
                }
            }
        }
    }
    ui->label_stuCount->setText(tr("当前在线人数：%1 总人数：%2").
                                arg(stuCount).
                                arg(stuInfoMap.count()));
}


/**
 * 主界面增加一个学生
 */
void MainWindow::addNewStu(QString MAC, QString stuName)
{
    // 必须循环结束，所有的都不相等才可以
    for(int i = 0;i < lSpilllter.count();i++) {
        if(lSpilllter.at(i)->objectName() == MAC) {
            // 如果有一个相同的，直接返回
            return;
        }
    }
    QLabel *lbHeadImg = new QLabel;
    QLabel *lbText = new QLabel;
    lLabelName.append(new QLabel(this));//这个要放在这里，不然会增加多
    lSpilllter.append(new QSplitter(Qt::Vertical,0));
    int spIndex = lSpilllter.count() - 1;

    lbText->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    QPixmap stuHeadBoy(":/resource/img/resource/img/boy.png");
    QPixmap stuHeadGirl(":/resource/img/resource/img/girl.png");
    //        lbHeadImg->setPixmap("stuHeadBoy");
    lbHeadImg->setPixmap(stuHeadBoy);

    // 添加头像和姓名
    lSpilllter.at(spIndex)->addWidget(lbHeadImg);
    lSpilllter.at(spIndex)->addWidget(lbText);
    lbText->setText(stuName);
    // 添加objectName是防止重复
    lSpilllter.at(spIndex)->setObjectName(MAC);

    // 添加控件的功能，比如右键有菜单出现等等
    lSpilllter.at(spIndex)->setContextMenuPolicy(Qt::ActionsContextMenu);
    lSpilllter.at(spIndex)->addAction(actionWatch);
    lSpilllter.at(spIndex)->installEventFilter(this);

    flowLayout->addWidget(lSpilllter.at(spIndex));
}

void MainWindow::on_toolButton_screenBC_clicked()
{
    if(ui->toolButton_screenBC->text() == "屏幕演示") {
        ui->toolButton_screenBC->setStyleSheet("background-color: rgb(0, 255, 0);");
        screenSharing2->pause();
        screenSharing1->start();
        screenSharing1->goOn();
        sender->sendCommand(CM_tOpenScreen);
        ui->toolButton_screenBC->setText("关闭演示");
        isOpenScreen = true;
//        QMessageBox::about(this,"开始演示","已经开始演示");
//        this->showMinimized();
        //        this->hide();

    } else {
        ui->toolButton_screenBC->setStyleSheet("");
        screenSharing1->pause();
        sender->sendCommand(CM_tCloseScreen);
        ui->toolButton_screenBC->setText("屏幕演示");
        isOpenScreen = false;
    }
}

void MainWindow::on_toolButton_checkIn_clicked()
{
    //发送一个命令，就是一个学生端弹出窗口
    //然后等待接受
    stuCheckIn->show();
}

void MainWindow::on_toolButton_fileTransfer_clicked()
{
    transferFile = new TransferFile;
    connect(receiver, &Receiver::sDownloadInfoSignal,
            transferFile, &TransferFile::sDownloadInfoSlot);
    transferFile->show();
}


void MainWindow::on_timeLeft()
{
    sender->sendCommand(CM_tHeartBeat);
    checkClassStuNum2();
}


void MainWindow::randomTimerSlot()
{
   multiWatchStu();
}

/**
 * 用来批量监控学生，一定时间定随机更改监控的学生
 * 批量监视学生的话，对流畅度要求不高
 * 因为批量监视学生，带宽占用大，可适当牺牲流畅度
 *
 * TODO: 直接分配1、2、3、4，正好是显示的位置
 */

// 思路：从全体学生列表里读取MAC，然后XX
// 还是需要一个定时器
// 并不是接收数据就打开，不然不好关闭

// 一次+4
void MainWindow::multiWatchStu()
{
    QMap<QString, bool> stuAliveMap = checkOnline->stuAliveMap;
    qDebug() << "multiWatchStu:" << stuAliveMap.count();
    // 先把在线的学生加到stuWatchList里面，然后都设置为-1，也就是不监控

    for (int i = 0;i < stuAliveMap.count(); i++) {
        QString MAC = stuAliveMap.keys().at(i);
        // -1就是不监控
        stuWatchList.insert(MAC, -1);
    }

    if (stuWatchList.count() < 4) {
        for (int i = 0 ;i < stuWatchList.count(); i++) {
            QString MAC = stuWatchList.keys().at(i);
            stuWatchList.insert(MAC, i);
        }
    } else {
        if (watchStuIndex >= stuWatchList.count() - 4) {
            watchStuIndex = 0;
        }

        // 这个watchStuIndex弄成全局变量是为了防止每次进到这个函数都置为0
        int fourPart = 0;
        for(int i = watchStuIndex;i < watchStuIndex + 4; i++) {
            QString MAC = stuWatchList.keys().at(i);
            stuWatchList.insert(MAC, fourPart);
            qDebug() << "MAC:" << MAC << "fourPart:" << fourPart;
            if(++fourPart >= 4) {
                fourPart = 0;
            }
        }
        watchStuIndex += 4;
    }

    sender->sendCommand(CM_tMasterIP,getHostIPV4Address());
    sender->sendMap(CM_tWatchYou,stuWatchList);
}

/**
 * 用于下方label的显示
 * 可显示各个班级当前在线的人数
 */
void MainWindow::checkClassStuNum2()
{
    QMap<QString, StuInfo*> stuInfoMap = checkOnline->stuInfoMap;

    for (int i = 0; i < eachClassStuNum.count(); i++) {
        eachClassStuNum.insert(eachClassStuNum.keys().at(i), 0);
    }


    // 总人数
    int allClassStuNum = 0;
    for (int i = 0; i < stuInfoMap.count(); i++) {
        StuInfo *stuInfo = stuInfoMap.values().at(i);
        QString stuClass = stuInfo->stuClass;
        if(stuClass.isEmpty()) {
            ui->label_stuCountPerClass->hide();
            return;
        } else {
            ui->label_stuCountPerClass->show();
        }
        bool stuIsAlive = stuInfo->stuIsAlive;
        if (stuIsAlive) {
            allClassStuNum++;
            int currentStuNum = eachClassStuNum.value(stuClass);
            eachClassStuNum.insert(stuClass, ++currentStuNum);
        }
    }
    QString stuNumStr = "";
    QString eachClassStuNumStr = "";
    for(int i = 0;i < eachClassStuNum.count();i++) {
        QString className = eachClassStuNum.keys().at(i);
        int stuNum = eachClassStuNum.values().at(i);
        eachClassStuNumStr += tr("；%1 在线人数：%2").arg(className).arg(stuNum);
    }
    // 另一个label已经显示了
//    stuNumStr = tr("%1，在线总人数：%2，人数最高时：%3").
//            arg(eachClassStuNumStr).
//            arg(allClassStuNum).
//            arg(stuInfoMap.count());
    ui->label_stuCountPerClass->setText(stuNumStr);
}


void MainWindow::addStuItem()
{
    //    for(int i = 0;i < 3;i++)
    //    {
    //        //        QList<QLabel*> lLabelName;
    //        //        QList<QSplitter*> lSplitter;

    //        lLabelPixmap.append(new QLabel(this));
    ////        lLabelPixmap.at(i)->setText("dsadsadasdsa");
    //        //        lLabelName.append(new QLabel(this));
    //        //        lSplitter.append(new QSplitter(this));
    //        ui->gridLayout->addWidget(lLabelPixmap.at(i));
    //    }
}

void MainWindow::on_pushButton_clicked()
{
    this->showMinimized();

}

QByteArray MainWindow::encodeStr(QString strSource)
{
    QByteArray baEncode = strSource.toUtf8();
    baEncode = baEncode.toBase64();
    qDebug() << "baEncode:" << baEncode;
    return baEncode;

}

QString MainWindow::decodeBa(QByteArray baEncode)
{
    QString strDecode = QString::fromUtf8(QByteArray::fromBase64(baEncode));
    qDebug() << "strDecode:" << strDecode;
    return strDecode;
}

QString MainWindow::decodeStr(QString strEncode)
{
    QByteArray ba = strEncode.toUtf8();
    QString strDecode = QString::fromUtf8(QByteArray::fromBase64(ba));
    qDebug() << "strDecode:" << strDecode;
    return strDecode;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("退出程序"),
                                   QString(tr("确定退出？")),
                                   QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::No) {
        event->ignore();  //忽略退出信号，程序继续运行
    }
    else if (button == QMessageBox::Yes) {
        event->accept();  //接受退出信号，程序退出
    }
}

void MainWindow::getCurrentTimeFromWeb()
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

void MainWindow::expireDate(QString endDateStr)
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

void MainWindow::on_toolButton_moniterStuScreen_clicked()
{
    //得到所有学生MAC地址，等等，MAC地址会不会重复。
    //MAC地址，然后是一个map，(MAC,bool)。如果是bool是true，则学生端向教师端IP传送屏幕数据。教师端可以查看

    //根据计时器，每秒更换观察的学生数，也就是更改map中的bool就行了。
    //学生端根据bool值来判断自己是否向教师端传输屏幕数据
    //    sender->sendCommand(CM_tMasterIP,getHostIPV4Address());//必须向学生端发送老师端的IP，但是map只能存两个值
    //先把老师的IP放在map里吧
    //    sender->sendCommand(CM_tMasterIP,getHostIPV4Address());
    //    sender->sendMap(CM_tWatchYou,stuWatchList);
    monitorScreen->show();
    sender->sendCommand(CM_tMasterIP,getHostIPV4Address());
    qDebug() << "getHostIPV4Address():" << getHostIPV4Address();
    sender->sendMap(CM_tWatchYou,stuWatchList);
    randomTimerSlot();

    int oneSec = 1000;    //一秒，这个一般不修改1000
    //    randomTimer->start(1);
    //更新列表和发送，应该分开
    randomTimer->start(5*oneSec); //start为QTIME自己的函数
}

QList<int> MainWindow::randomTest(int maxNum)
{
    //一定范围，且不重复的随机数
    //从0开始，所以到不了maxNum
    int i,j;
    QList<int> numbersList;
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(i=0;i<maxNum;i++) {
        numbersList.append(qrand()%maxNum);
        bool flag=true;
        while(flag) {
            for(j=0;j<i;j++) {
                if(numbersList[i]==numbersList[j]) {
                    break;
                }
            }
            if(j<i) {
                numbersList[i]=rand()%maxNum;
            }
            if(j==i) {
                flag=!flag;
            }
        }
    }
    //    for(i=0;i<maxNum;i++)
    //    {
    //        qDebug()<<numbersList[i];
    //    }
    return numbersList;
}


QString MainWindow::getHostIPV4Address()
{
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);

    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            qDebug() << "address.toString():" << address.toString();
            return address.toString();
        }
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    static bool max = false;
    static QRect location = this->geometry();


    if (max) {
        this->setGeometry(location);
    } else {
        location = this->geometry();
        this->setGeometry(QApplication::desktop()->availableGeometry());
    }

    this->setProperty("canMove", max);
    max = !max;
}

void MainWindow::on_pushButton_3_clicked()
{
    this->close();
}

void MainWindow::sScreenSlot(QByteArray qba,int fourPart, QString stuName,QString objName)
{
    //单独查看
    if(fourPart == 666) {
        showScreen->screenRecv(qba);
    } else {
        if(fourPart == 0) {
            monitorScreen->ss1->screenRecv(qba);
            monitorScreen->ss1->show();
            monitorScreen->setName(stuName,0);
            monitorScreen->setObjName(objName,0);
        }
        if(fourPart == 1) {
            monitorScreen->ss2->screenRecv(qba);
            monitorScreen->ss2->show();
            monitorScreen->setName(stuName,1);
            monitorScreen->setObjName(objName,1);
        }
        if(fourPart == 2) {
            monitorScreen->ss3->screenRecv(qba);
            monitorScreen->ss3->show();
            monitorScreen->setName(stuName,2);
            monitorScreen->setObjName(objName,2);
        }
        if(fourPart == 3) {
            monitorScreen->ss4->screenRecv(qba);
            monitorScreen->ss4->show();
            monitorScreen->setName(stuName,3);
            monitorScreen->setObjName(objName,3);
        }
        monitorScreen->show();
    }
}
void MainWindow::startWatchingSlot()
{
    monitorScreen->show();
    sender->sendCommand(CM_tMasterIP,getHostIPV4Address());
    sender->sendMap(CM_tWatchYou,stuWatchList);
    randomTimerSlot();
    int oneSec = 1000;
    randomTimer->start(1*oneSec);
}

void MainWindow::stopWatchingSlot()
{
    sender->sendCommand(CM_tStopWatch);
    randomTimer->stop();
    monitorScreen->hide();
}

void MainWindow::watchSingleStuScreenSlot(QString MAC)
{
    for(int i = 0;i < stuWatchList.count();i++)
    {
        QString MAC = stuWatchList.keys().at(i);
        stuWatchList.insert(MAC,-1);
    }
    stuWatchList.insert(MAC,666);

    showScreen->show();
    sender->sendCommand(CM_tMasterIP,getHostIPV4Address());
    sender->sendMap(CM_tWatchYou,stuWatchList);

}

void MainWindow::showScreenCloseSlot()
{
    sender->sendCommand(CM_tStopWatch);
    showScreen->close();
}

void MainWindow::sStuInfoSlot(StuInfo *stuInfo)
{
    QString stuMAC = stuInfo->stuMAC;
    QString stuName = stuInfo->stuName;
    addNewStu(stuMAC, stuName);
}


void MainWindow::on_toolButton_ExamSystem_clicked()
{
    QString fileName = "ExamTeacher.exe";
    QFileInfo file(fileName);
    if(!file.exists()) {
        QMessageBox::warning(this, "提醒", "考试程序不存在");
        return;
    }
    QProcess *process = new QProcess;
    process->start(fileName);
}
