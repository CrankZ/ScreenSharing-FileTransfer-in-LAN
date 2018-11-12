#include "drawcursor.h"
#include <QDebug>
#include <QThread>

DrawCursor::DrawCursor(QWidget *widget, QObject *parent) : QObject(parent)
{
    showWidget = widget;
    cursorLabel = new QLabel;
    cursorPixmap.load("C:\\Users\\CrankZ\\Desktop\\cursor\\cursor-arrow.png");
    cursorLabel = new QLabel(widget);
    cursorLabel->setPixmap(cursorPixmap);
    cursorLabel->setStyleSheet("background-color: rgba(0, 0, 0, 0);");
    cursorLabel->show();
}


void DrawCursor::drawCursor(QSize sourceScreenSize, QPoint masterCursorPos)
{
//    qDebug() << "width:" << pixmap.width() << ",height:" << pixmap.height();
//    qDebug() << "x:" << pixmap.logicalDpiX();
    double masterWidth = sourceScreenSize.width();
    double masterHeight = sourceScreenSize.height();
    double labelWidth = showWidget->label->width();
    double labelHeight = showWidget->label->height();
    double pixmapWidth = showWidget->label->pixmap.width();
    double pixmapHeight = showWidget->label->pixmap.height();

    // 这里没法直接获取pixmap的坐标，但是pixmap是居中显示的，所以可以换算一下
    double pixmapX = (labelWidth - pixmapWidth) / 2;
    double pixmapY = (labelHeight - pixmapHeight) / 2;
    double labelX = ui->label->pos().x();
    double labelY = ui->label->pos().y();
    // 这里的5和2是鼠标图片的空白区域
    // 如果鼠标图片的大小有改变，这里也要一起调整
    double nowX = masterCursorPos.x() * (pixmapWidth / masterWidth) + labelX + pixmapX - 5;
    double nowY = masterCursorPos.y() * (pixmapHeight / masterHeight) + labelY + pixmapY - 2;

    qDebug() << "nowX:" << nowX << ",nowY:" << nowY;
    // 方法1，直接一个新label
    cursorLabel->setGeometry(nowX, nowY, 25, 25);

    // 方法2，直接在图片上绘制
//    QPainter painter(&pixmap);
//    QRect cursorRect;
//    cursorRect.setRect(nowX,nowY,25,25);
//    painter.drawPixmap(cursorRect, cursorPixmap);
//    ui->label->setPixmap(pixmap);
}
