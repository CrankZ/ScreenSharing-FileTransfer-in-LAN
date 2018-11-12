#include "screensharing.h"

ScreenSharing::ScreenSharing(int num=1)
{
    quality = 60;
    mutex = new QMutex;
    mutex->lock();
    flag = 0;
    lock = true;
    readSettings();
    sender = new Sender();


    QScreen *src = QApplication::primaryScreen();
    QPixmap firstPixmap1 = src->grabWindow(QApplication::desktop()->winId());
    img1 = firstPixmap1.toImage();
}

ScreenSharing::~ScreenSharing()
{
}

void ScreenSharing::readSettings()
{
    QString readName = "settings.ini";
    if(readName.isEmpty()) {
        //空字符串不处理，返回
        qDebug() << "文件名不能为空！";
        return;
    } else {
        //定义文件对象
        QFile fileIn(readName);
        if(!fileIn.open(QIODevice::ReadOnly)) {
            //            qDebug() << "文件不存在！";
            return; //不处理文件
        }
        //读取并解析文件
        while(!fileIn.atEnd()) {
            //读取一行
            QByteArray ba = fileIn.readAll();
            QString Settings = QString::fromUtf8(ba);
            if(Settings.isEmpty()) {
                qDebug() << "文件为空，不读取";
            } else {
                QStringList SettingsList = Settings.split("quality:");
                QStringList temp = SettingsList[1].split("\n");
                QString strQuality = temp[0];
                if(strQuality.toInt() > 100) {
                    quality = 100;
                } else if (strQuality.toInt() <= 0) {
                    quality = 1;
                } else {
                    quality = strQuality.toInt();
                }
                qDebug() << "quality:" << quality;
            }
        }
    }
}


/**
 * 图片差异对比
 * 对比两张图片的所有像素点x
 * @todo 关于两个像素点之间的距离问题，应该还有优化空间
 */
bool ScreenSharing::compareImg(QImage img1,QImage img2)
{
    int w = img1.width();
    int h = img1.height();
    // 两个像素点之间的距离
    int distance = 30;

    if(!img2.isNull() && !img1.isNull()) {
        for(int x = 0;x < w;x += distance) {
            for(int y = 0;y < h;y += distance) {
                QRgb *rgb1 = (QRgb*)img1.pixel(x,y);

                QRgb *rgb2 = (QRgb*)img2.pixel(x,y);
                // 如果两张图片，有一个像素点不一样，就说明这两张图片肯定不一样
                // 如果两张图片，有一个像素点一样，不一定说明这两个图片相同
                if(rgb1 != rgb2) {
                    qDebug() << "不一样";
                    return false;
                }
            }
        }
    }
    // 只有等两张图片，对比完，所有像素点都没有不相同时，才算这两张图片一样
//    qDebug() << "一样";
    return true;
}
void ScreenSharing::run()
{
    while (1) {
        if (0 == flag)
            mutex->lock();
        if (2 == flag)
            break;

        CURSORINFO cursorInfo;
        cursorInfo.cbSize = sizeof(cursorInfo);
        GetCursorInfo(&cursorInfo);

        // 下面是各种光标的信息
        HCURSOR APPSTARTING = LoadCursor(NULL, IDC_APPSTARTING);
        HCURSOR ARROW = LoadCursor(NULL, IDC_ARROW);
        HCURSOR CROSS = LoadCursor(NULL, IDC_CROSS );
        HCURSOR HAND = LoadCursor(NULL, IDC_HAND );
        HCURSOR HELP = LoadCursor(NULL, IDC_HELP );
        HCURSOR IBEAM = LoadCursor(NULL, IDC_IBEAM );
        HCURSOR ICON = LoadCursor(NULL, IDC_ICON );
        HCURSOR NO = LoadCursor(NULL, IDC_NO );
        HCURSOR SIZE = LoadCursor(NULL, IDC_SIZE );
        HCURSOR SIZEALL = LoadCursor(NULL, IDC_SIZEALL);
        HCURSOR SIZENESW = LoadCursor(NULL, IDC_SIZENESW);
        HCURSOR SIZENS = LoadCursor(NULL, IDC_SIZENS);
        HCURSOR SIZENWSE = LoadCursor(NULL, IDC_SIZENWSE);
        HCURSOR SIZEWE = LoadCursor(NULL, IDC_SIZEWE);
        HCURSOR UPARROW = LoadCursor(NULL, IDC_UPARROW);
        HCURSOR WAIT = LoadCursor(NULL, IDC_WAIT);

        HCURSOR currentCursorType = cursorInfo.hCursor;

        if(currentCursorType == IBEAM) {
            cursorType = "IBEAM";

        } else {
            //其他所有的都用这个来演示
            cursorType = "ELSE";
        }
        QPixmap tempPixmap1;

            QScreen *src = QApplication::primaryScreen();
            tempPixmap1 = src->grabWindow(QApplication::desktop()->winId());
            //                QScreen *src = QApplication::screens().last();
            //                tempPixmap1 = src->grabWindow(0, src->geometry().x(), src->geometry().y(), src->size().width(), src->size().height());

        QImage img2 = tempPixmap1.toImage();
        // 如果不一样，则发送
        // 差异比较
        if(!compareImg(img1, img2) || firstScreen) {
            // 刚开始的时候，不管有没有变化，都要发一帧。
            firstScreen = false;
            //    matrix.rotate(90);
            //    QPixmap tempPixmap = pixmap.transformed(matrix);
            QBuffer buffer;
            tempPixmap1.save(&buffer,"jpg",quality);
            //不分包
            //    udp->writeDatagram(buffer.data().data(), buffer.data().size(),
            //        QHostAddress(ui->lineEditIP->text()), ui->lineEditPort->text().toInt());
            //分包开始+++++++++++++++++++++++++++
            // 必须分包，因为UDP对传输大小有限制，不能直接传输整个文件
            int dataLength=buffer.data().size();

            unsigned char *dataBuffer=(unsigned char *)buffer.data().data();
            buffer.close();

            int packetNum = 0;
            int lastPaketSize = 0;
            packetNum = dataLength / UDP_MAX_SIZE;
            lastPaketSize = dataLength % UDP_MAX_SIZE;
            int currentPacketIndex = 0;
            if (lastPaketSize != 0) {
                packetNum = packetNum + 1;
            }

            PackageHeader packageHead;
            packageHead.uTransPackageHdrSize = sizeof(packageHead);
            packageHead.uDataSize = dataLength;
            packageHead.uDataPackageNum = packetNum;
            unsigned char frameBuffer[1024];
            memset(frameBuffer,0,1024);
            QUdpSocket *udp = new QUdpSocket;
            while (currentPacketIndex < packetNum) {
                int udpMaxSizeTemp = UDP_MAX_SIZE;
                if (currentPacketIndex >= (packetNum-1)) {
                    udpMaxSizeTemp = dataLength-currentPacketIndex*UDP_MAX_SIZE;
                }
                packageHead.uTransPackageSize = sizeof(PackageHeader)+
                        udpMaxSizeTemp;
                packageHead.uDataPackageCurrIndex = currentPacketIndex+1;
                packageHead.uDataPackageOffset = currentPacketIndex*UDP_MAX_SIZE;
                memcpy(frameBuffer, &packageHead, sizeof(PackageHeader));
                memcpy(frameBuffer+sizeof(PackageHeader),
                       dataBuffer+packageHead.uDataPackageOffset,
                       udpMaxSizeTemp);
                QByteArray byteArray =
                        QByteArray((const char*)frameBuffer, packageHead.uTransPackageSize);
                //////////
                QByteArray bytes;    //字节数组
                //QDataStream类是将序列化的二进制数据送到io设备，因为其属性为只写
                QDataStream out(&bytes, QIODevice::WriteOnly);//out为待发送
                int msgType = CM_tScreen1;
                out << msgType << byteArray;
                //                    out << getCurrentCursorPos();
                ////////////

                // 压缩level0-9，默认为-1，我也不知道-1啥意思
                QByteArray compressedBytes = qCompress(bytes);


                // 返回发送成功的bytes大小
                int sentSuccessfullyBytesSize =
                        udp->writeDatagram(
                            compressedBytes.data(),
                            compressedBytes.size(),
                            QHostAddress::Broadcast,
                            PORT);
                // 这个关闭必须有，否则传输会出现问题
                udp->close();
                // 如果发送成功的bytes大小不等于压缩后的大小，说明发送失败
                if(sentSuccessfullyBytesSize != compressedBytes.size()) {
                    qDebug()<<"Failed to send image";
                }

                currentPacketIndex++;
            }
            //分包结束+++++++++++++++++++++++++++
        }
        img1 = img2;
        sendCursorInfo();
    }
    sender->sendCommand(CM_tOpenScreen);
}

void ScreenSharing::sendCursorInfo()
{
    sender->sendCursorInfo(CM_tCursorPos, getCurrentScreenSize(), cursorType, getCurrentCursorPos());
}


/**
 * 获取当前屏幕分辨率
 */
QSize ScreenSharing::getCurrentScreenSize()
{
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    int screenWidth = screen.width();
    int screenHeight = screen.height();
    QSize currentScreenSize;
    currentScreenSize.setWidth(screenWidth);
    currentScreenSize.setHeight(screenHeight);
    return currentScreenSize;
}
/**
 * 获得当前鼠标在整个屏幕中的坐标
 */
QPoint ScreenSharing::getCurrentCursorPos()
{
    QCoreApplication::processEvents();
    QPoint globalCursorPos = QCursor::pos();
    int mouseScreen = QApplication::desktop()->screenNumber(globalCursorPos);

    QRect mouseScreenGeometry = QApplication::desktop()->screen(mouseScreen)->geometry();
    QPoint currentCursorPos = globalCursorPos - mouseScreenGeometry.topLeft();
    return currentCursorPos;
}


void ScreenSharing::pause()
{
    flag = 0;
}

void ScreenSharing::goOn()
{
    readSettings();
    flag = 1;
    mutex->unlock();
}

void ScreenSharing::down()
{
    flag = 2;
    mutex->unlock();
}
