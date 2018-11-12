#include "showscreen.h"
#include "ui_showscreen.h"

ShowScreen::ShowScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShowScreen)
{
    ui->setupUi(this);
    ui->label->setAlignment(Qt::AlignCenter);//标签居中显示
    // 以下两个只在真实情况下启用
    // 不然不好关闭
//    setWindowState(Qt::WindowFullScreen);   //全屏且无标题栏
//    setWindowFlags (Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
    setWindowFlags (Qt::WindowStaysOnTopHint);
    ui->label->setText("");
    this->setLayout(ui->horizontalLayout);

    cursorPixmap.load(":/cursor/resource/cursor/cursor-arrow.png");
    cursorLabel = new QLabel(this);
    cursorLabel->setPixmap(cursorPixmap);
    cursorLabel->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    cursorLabel->show();
}

ShowScreen::~ShowScreen()
{
    delete ui;
    qDebug() << "ShowScreen,byebye";
}


/**
 * TODO:光标样式支持不全，大小不能自适应
 */
void ShowScreen::setCursorType(QString cursorType)
{
    qDebug() << cursorType;
    qDebug() << cursorPixmap;
    if(cursorType == "IBEAM") {
        cursorPixmap.load(":/cursor/resource/cursor/cursor-ibeam.png");
    } else {
        cursorPixmap.load(":/cursor/resource/cursor/cursor-arrow.png");
    }
    cursorLabel->setPixmap(cursorPixmap);
}


void ShowScreen::drawCursor(QSize sourceScreenSize, QString cursorType, QPoint masterCursorPos)
{
    setCursorType(cursorType);
    double masterWidth = sourceScreenSize.width();
    double masterHeight = sourceScreenSize.height();
    double labelWidth = ui->label->width();
    double labelHeight = ui->label->height();
    double pixmapWidth = pixmap.width();
    double pixmapHeight = pixmap.height();

    // 这里没法直接获取pixmap的坐标，但是pixmap是居中显示的，所以可以换算一下
    double pixmapX = (labelWidth - pixmapWidth) / 2;
    double pixmapY = (labelHeight - pixmapHeight) / 2;
    double labelX = ui->label->pos().x();
    double labelY = ui->label->pos().y();
    // 这里的5和2是鼠标图片的空白区域
    // 如果鼠标图片的大小有改变，这里也要一起调整
    double nowX = masterCursorPos.x() * (pixmapWidth / masterWidth) + labelX + pixmapX - 5;
    double nowY = masterCursorPos.y() * (pixmapHeight / masterHeight) + labelY + pixmapY - 2;

    // 方法1，直接一个新label
    cursorLabel->setGeometry(nowX, nowY, 25, 25);

    // 方法2，直接在图片上绘制
//    QPainter painter(&pixmap);
//    QRect cursorRect;
//    cursorRect.setRect(nowX,nowY,25,25);
//    painter.drawPixmap(cursorRect, cursorPixmap);
//    ui->label->setPixmap(pixmap);
}

void ShowScreen::screenRecv(QByteArray qba, int part)
{
    //组包开始+++++++++++++++++++++++++++
    static int num = 1;
    static uint size = 0;

    PackageHeader *packageHead = (PackageHeader *)qba.data();
    if (packageHead->uDataPackageCurrIndex == num) {
        num++;
        size += packageHead->uTransPackageSize-packageHead->uTransPackageHdrSize;
//        qDebug() << "size:" << size;
        if (size > 1024*1000) {
            qDebug()<<"image too big";
            num = 1;
            size = 0;
            return;
        }
        if (packageHead->uDataPackageOffset > 1024*1000) {
            qDebug()<<"image too big";
            packageHead->uDataPackageOffset = 0;
            num = 1;
            size = 0;
            return;
        }

        memcpy(imageData.data+packageHead->uDataPackageOffset, qba.data()+packageHead->uTransPackageHdrSize,
               packageHead->uTransPackageSize-packageHead->uTransPackageHdrSize);
        if ((packageHead->uDataPackageNum == packageHead->uDataPackageCurrIndex)
                && (size == packageHead->uDataSize)) {
            imageData.length = packageHead->uDataSize;
            // image是原始图片，长宽是原始图片的长宽，不会改变
            // image的长宽也就是master主机的分辨率
            image.loadFromData((uchar *)imageData.data,imageData.length,"jpg");

            pixmap = QPixmap::fromImage(image);

            int labelWidth = ui->label->width();
            int labelHeight = ui->label->height();

            // 如果传过来的图片比当前屏幕大，则等比例缩放
            if(image.height() > labelHeight) {
                pixmap = pixmap.scaled(labelWidth, labelHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            }
            ui->label->setPixmap(pixmap);
//            ui->label->setScaledContents(true);
            recvImageNum++;
            memset(&imageData,0,sizeof(UdpUnpackData));
            num = 1;
            size = 0;


        }
    } else {
        num = 1;
        size = 0;
        memset(&imageData,0,sizeof(UdpUnpackData));
    }
    //组包结束+++++++++++++++++++++++++++
}

void ShowScreen::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
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
