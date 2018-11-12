#include "iconhelper.h"

IconHelper *IconHelper::self = 0;
IconHelper *IconHelper::Instance()
{
    if (!self) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (!self) {
            self = new IconHelper;
        }
    }

    return self;
}

IconHelper::IconHelper(QObject *) : QObject(qApp)
{
    int fontId = QFontDatabase::addApplicationFont(":/qss/resource/fontawesome-webfont.ttf");
    QStringList fontName = QFontDatabase::applicationFontFamilies(fontId);

    if (fontName.count() > 0) {
        iconFont = QFont(fontName.at(0));
    } else {
        qDebug() << "load fontawesome-webfont.ttf error";
    }
}

void IconHelper::setIcon(QLabel *lab, QChar c, quint32 size)
{
    iconFont.setPixelSize(size);
    lab->setFont(iconFont);
    lab->setText(c);
}

void IconHelper::setIcon(QAbstractButton *btn, QChar c, quint32 size)
{
    iconFont.setPixelSize(size);
    btn->setFont(iconFont);
    btn->setText(c);
}

QPixmap IconHelper::getPixmap(const QString &color, QChar c, quint32 size,
                              quint32 pixWidth, quint32 pixHeight)
{
    QPixmap pix(pixWidth, pixHeight);
    pix.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&pix);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.setPen(QColor(color));
    painter.setBrush(QColor(color));

    iconFont.setPixelSize(size);
    painter.setFont(iconFont);
    painter.drawText(pix.rect(), Qt::AlignCenter, c);
    painter.end();

    return pix;
}

QPixmap IconHelper::getPixmap(QToolButton *btn, bool normal)
{
    QPixmap pix;
    int index = btns.indexOf(btn);

    if (index >= 0) {
        if (normal) {
            pix = pixNormal.at(index);
        } else {
            pix = pixDark.at(index);
        }
    }

    return pix;
}

void IconHelper::setStyle(QWidget *widget, const QString &type, int borderWidth, const QString &borderColor,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding:%1px %2px %2px %2px;")
                .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding:%2px %1px %2px %2px;")
                .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding:%2px %2px %1px %2px;")
                .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding:%2px %2px %2px %1px;")
                .arg(borderWidth).arg(borderWidth * 2);
    }

    QStringList qss;
    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;color:%2;background:%3;}")
               .arg(type).arg(normalTextColor).arg(normalBgColor));

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover,"
                       "QWidget[flag=\"%1\"] QAbstractButton:pressed,"
                       "QWidget[flag=\"%1\"] QAbstractButton:checked{"
                       "border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(borderColor).arg(darkTextColor).arg(darkBgColor));

    widget->setStyleSheet(qss.join(""));
}


// 左侧用到的
void IconHelper::setStyle(QWidget *widget, QList<QToolButton *> btns, QList<int> pixChar,
                          quint32 iconSize, quint32 iconWidth, quint32 iconHeight,
                          const QString &type, int borderWidth, const QString &borderColor,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    int btnCount = btns.count();
    int charCount = pixChar.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    QString strBorder;
    if (type == "top") {
        strBorder = QString("border-width:%1px 0px 0px 0px;padding:%1px %2px %2px %2px;")
                .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "right") {
        strBorder = QString("border-width:0px %1px 0px 0px;padding:%2px %1px %2px %2px;")
                .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "bottom") {
        strBorder = QString("border-width:0px 0px %1px 0px;padding:%2px %2px %1px %2px;")
                .arg(borderWidth).arg(borderWidth * 2);
    } else if (type == "left") {
        strBorder = QString("border-width:0px 0px 0px %1px;padding:%2px %2px %2px %1px;")
                .arg(borderWidth).arg(borderWidth * 2);
    }

    //如果图标是左侧显示则需要让没有选中的按钮左侧也有加深的边框,颜色为背景颜色
    QStringList qss;
    if (btns.at(0)->toolButtonStyle() == Qt::ToolButtonTextBesideIcon) {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:solid;border-radius:0px;%2border-color:%3;color:%4;background:%5;}")
                   .arg(type).arg(strBorder).arg(normalBgColor).arg(normalTextColor).arg(normalBgColor));
    } else {
        qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton{border-style:none;border-radius:0px;padding:5px;color:%2;background:%3;}")
                   .arg(type).arg(normalTextColor).arg(normalBgColor));
    }

    qss.append(QString("QWidget[flag=\"%1\"] QAbstractButton:hover,"
                       "QWidget[flag=\"%1\"] QAbstractButton:pressed,"
                       "QWidget[flag=\"%1\"] QAbstractButton:checked{"
                       "border-style:solid;%2border-color:%3;color:%4;background:%5;}")
               .arg(type).arg(strBorder).arg(borderColor).arg(darkTextColor).arg(darkBgColor));

    // 左侧主控件背景颜色
    qss.append(QString("QWidget#%1{background:%2;}").arg(widget->objectName()).arg(normalBgColor));

    qss.append(QString("QWidget>QToolButton{border-width:0px;}"));
    qss.append(QString("QWidget>QToolButton{background-color:%1;color:%2;}")
               .arg(normalBgColor).arg(normalTextColor));
    qss.append(QString("QWidget>QToolButton:hover,QWidget>QToolButton:pressed,QWidget>QToolButton:checked{background-color:%1;color:%2;}")
               .arg(darkBgColor).arg(darkTextColor));

    widget->setStyleSheet(qss.join(""));

    qDebug() << qss << endl;

    for (int i = 0; i < btnCount; i++) {
        //存储对应按钮对象,方便鼠标移上去的时候切换图片
        QPixmap pixNormal = getPixmap(normalTextColor, QChar(pixChar.at(i)), iconSize, iconWidth, iconHeight);
        QPixmap pixDark = getPixmap(darkTextColor, QChar(pixChar.at(i)), iconSize, iconWidth, iconHeight);

        btns.at(i)->setIcon(QIcon(pixNormal));
        btns.at(i)->setIconSize(QSize(iconWidth, iconHeight));
        btns.at(i)->installEventFilter(this);

        this->btns.append(btns.at(i));
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixDark);
    }
}

void IconHelper::setStyle(QFrame *frame, QList<QToolButton *> btns, QList<int> pixChar,
                          quint32 iconSize, quint32 iconWidth, quint32 iconHeight,
                          const QString &normalBgColor, const QString &darkBgColor,
                          const QString &normalTextColor, const QString &darkTextColor)
{
    int btnCount = btns.count();
    int charCount = pixChar.count();
    if (btnCount <= 0 || charCount <= 0 || btnCount != charCount) {
        return;
    }

    QStringList qss;
    qss.append(QString("QFrame>QToolButton{border-style:none;border-width:0px;}"));
    qss.append(QString("QFrame>QToolButton{background-color:%1;color:%2;}")
               .arg(normalBgColor).arg(normalTextColor));
    qss.append(QString("QFrame>QToolButton:hover,QFrame>QToolButton:pressed,QFrame>QToolButton:checked{background-color:%1;color:%2;}")
               .arg(darkBgColor).arg(darkTextColor));

    frame->setStyleSheet(qss.join(""));

    for (int i = 0; i < btnCount; i++) {
        //存储对应按钮对象,方便鼠标移上去的时候切换图片
        QPixmap pixNormal = getPixmap(normalTextColor, QChar(pixChar.at(i)), iconSize, iconWidth, iconHeight);
        QPixmap pixDark = getPixmap(darkTextColor, QChar(pixChar.at(i)), iconSize, iconWidth, iconHeight);

        btns.at(i)->setIcon(QIcon(pixNormal));
        btns.at(i)->setIconSize(QSize(iconWidth, iconHeight));
        btns.at(i)->installEventFilter(this);

        this->btns.append(btns.at(i));
        this->pixNormal.append(pixNormal);
        this->pixDark.append(pixDark);
    }
}

bool IconHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (watched->inherits("QToolButton")) {
        QToolButton *btn = (QToolButton *)watched;
        int index = btns.indexOf(btn);
        if (index >= 0) {
            if (event->type() == QEvent::Enter) {
                btn->setIcon(QIcon(pixDark.at(index)));
            } else if (event->type() == QEvent::Leave) {
                if (btn->isChecked()) {
                    btn->setIcon(QIcon(pixDark.at(index)));
                } else {
                    btn->setIcon(QIcon(pixNormal.at(index)));
                }
            }
        }
    }

    return QObject::eventFilter(watched, event);
}
