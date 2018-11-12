#include "showscreen.h"
#include "ui_showscreen.h"

ShowScreen::ShowScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowScreen)
{
    ui->setupUi(this);
    ui->label->setAlignment(Qt::AlignCenter);//标签居中显示
    //    setWindowState(Qt::WindowMaximized);    //最大化，有标题栏
        //    setWindowFlags (Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::WindowFullScreen);
    //        setWindowFlags(Qt::WindowStaysOnTopHint); //置顶
//    setWindowState(Qt::WindowFullScreen);   //全屏且无标题栏
    ui->label->setText("");
}

ShowScreen::~ShowScreen()
{
    delete ui;
}

void ShowScreen::screenRecv(QByteArray qba)
{
    static int num = 1;
    static uint size = 0;

    PackageHeader *packageHead = (PackageHeader *)qba.data();
    if (packageHead->uDataPackageCurrIndex == num)
    {
        num++;
        size += packageHead->uTransPackageSize-packageHead->uTransPackageHdrSize;
        if (size > 1024*1000)
        {
            qDebug()<<"image too big";
            num = 1;
            size = 0;
            return;
        }
        if (packageHead->uDataPackageOffset > 1024*1000)
        {
            qDebug()<<"image too big";
            packageHead->uDataPackageOffset = 0;
            num = 1;
            size = 0;
            return;
        }
        memcpy(imageData.data+packageHead->uDataPackageOffset, qba.data()+packageHead->uTransPackageHdrSize,
               packageHead->uTransPackageSize-packageHead->uTransPackageHdrSize);
        if ((packageHead->uDataPackageNum == packageHead->uDataPackageCurrIndex)
                && (size == packageHead->uDataSize))
        {
            imageData.length = packageHead->uDataSize;

            QImage image;
            image.loadFromData((uchar *)imageData.data,imageData.length,"jpg");
            QPixmap pixmap=QPixmap::fromImage(image);
            ui->label->resize(this->size());
            int width = ui->label->width();
            int height = ui->label->height();
            pixmap = pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation);

            ui->label->setPixmap(pixmap);
//            ui->label->setScaledContents(true);
//            ui->label->adjustSize();
            recvImageNum++;
            memset(&imageData,0,sizeof(UdpUnpackData));
            num = 1;
            size = 0;
        }
    }
    else
    {
        num = 1;
        size = 0;
        memset(&imageData,0,sizeof(UdpUnpackData));
    }
    //组包结束+++++++++++++++++++++++++++
}

void ShowScreen::closeEvent(QCloseEvent *event)
{
    qDebug() << "关闭事件";
    emit showScreenCloseSignal();
    this->close();
}

