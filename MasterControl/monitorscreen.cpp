#include "monitorscreen.h"
#include "ui_monitorscreen.h"
#include "qaction.h"

MonitorScreen::MonitorScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MonitorScreen)
{
    ui->setupUi(this);
//    setAttribute(Qt::WA_DeleteOnClose);//真的关闭
    this->setLayout(ui->gridLayout);

    ss1 = new ShowScreen(ui->label_1);
    ss1->show();

    ss2 = new ShowScreen(ui->label_2);
    ss2->show();

    ss3 = new ShowScreen(ui->label_3);
    ss3->show();

    ss4 = new ShowScreen(ui->label_4);
    ss4->show();

    ss1->resize(ui->label_1->width(),ui->label_1->height());
    ss2->resize(ui->label_2->width(),ui->label_2->height());
    ss3->resize(ui->label_3->width(),ui->label_3->height());
    ss4->resize(ui->label_4->width(),ui->label_4->height());


    actionWatch = new QAction("查看屏幕");

    ui->label_1->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->label_2->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->label_3->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->label_4->setContextMenuPolicy(Qt::ActionsContextMenu);

    ui->label_1->addAction(actionWatch);
    ui->label_2->addAction(actionWatch);
    ui->label_3->addAction(actionWatch);
    ui->label_4->addAction(actionWatch);

    ui->label_1->installEventFilter(this);
    ui->label_2->installEventFilter(this);
    ui->label_3->installEventFilter(this);
    ui->label_4->installEventFilter(this);

    connect(actionWatch, SIGNAL(triggered()),
            this, SLOT(watchSingleStuScreen()));
}

MonitorScreen::~MonitorScreen()
{
    qDebug() << "MonitorScreen,byebye";
    delete ui;
}

void MonitorScreen::setName(QString name,int fourPart)
{
    if(fourPart == 0) ui->label_1stuName->setText(name);
    if(fourPart == 1) ui->label_2stuName->setText(name);
    if(fourPart == 2) ui->label_3stuName->setText(name);
    if(fourPart == 3) ui->label_4stuName->setText(name);
}

void MonitorScreen::setObjName(QString name,int fourPart)
{
    if(fourPart == 0) ui->label_1->setObjectName(name);
    if(fourPart == 1) ui->label_2->setObjectName(name);
    if(fourPart == 2) ui->label_3->setObjectName(name);
    if(fourPart == 3) ui->label_4->setObjectName(name);
}

void MonitorScreen::resizeEvent(QResizeEvent* event)
{
    ss1->resize(ui->label_1->width(),ui->label_1->height());
    ss2->resize(ui->label_2->width(),ui->label_2->height());
    ss3->resize(ui->label_3->width(),ui->label_3->height());
    ss4->resize(ui->label_4->width(),ui->label_4->height());
}

bool MonitorScreen::eventFilter(QObject * obj, QEvent * event)
{
    //单击事件
    if(event->type() == QEvent::MouseButtonRelease) {
        this->obj = obj;
    }
    return false;
}

void MonitorScreen::keyReleaseEvent(QKeyEvent *ev)
{
    if(ev->key() == Qt::Key_Space) {
        qDebug() << "空格";
        pauseMonitor();
    }
}

/**
 * 可以不关闭窗口，只是暂停画面和接收数据
 * 因为老师有可能需要暂停画面，仔细看一下学生的画面
 */
void MonitorScreen::pauseMonitor()
{
    if(this->styleSheet() == "") {
        emit startWatchingSignal();
        this->setStyleSheet("background-color: rgb(0, 0, 0);");
    } else {
        emit stopWatchingSignal();
        this->setStyleSheet("");
    }
}

/**
 * 单独查看某个学生
 */
void MonitorScreen::watchSingleStuScreen()
{
    //向主界面发送那个命令
    QString stuMac = obj->objectName();
    qDebug() << "发送：" << stuMac;
    emit stopWatchingSignal();
    emit watchSingleStuScreenSignal(stuMac);
}

void MonitorScreen::closeEvent(QCloseEvent *event)
{
    emit stopWatchingSignal();
    this->close();
}
