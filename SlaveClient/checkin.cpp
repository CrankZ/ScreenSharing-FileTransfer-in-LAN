#include "checkin.h"
#include "ui_checkin.h"

CheckIn::CheckIn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CheckIn)
{
    ui->setupUi(this);

    setWindowFlags (Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);//置顶，无边框

    this->setLayout(ui->verticalLayout);
    ui->verticalLayout->setAlignment(Qt::AlignCenter);
    ui->pushButton_readStuName->hide();
}

CheckIn::~CheckIn()
{
    delete ui;
}

void CheckIn::addClassNameItem(QList<QString> className)
{
    for(int i = 0;i < className.count();i++)
    {
        qDebug() << "进来了" << className.at(i);
        ui->classComboBox_stuClass->addItem(className.at(i));
    }
}

bool CheckIn::isFileExist()
{
    QString saveName = "stuInfo.txt";

    QFile file(saveName);
    if (file.exists())
    {
        qDebug() << "文件存在";
        return true;
    }
    else
    {
        qDebug() << "文件不存在";
        return false;
    }
    return false;
}

void CheckIn::on_pushButton_confirm_clicked()
{
    QString saveName = "stuInfo.txt";

    QFile fileOut(saveName);
    if(! fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        return;
    }

    QString stuNo;
    QString stuName;
    QString className;

    if(ui->lineEdit_stuName->text().isEmpty())
    {
        QMessageBox::warning(this,tr("错误"),tr("姓名禁止为空！"));
    }
    else
    {
        stuNo = ui->lineEdit_stuNo->text();
        stuName = ui->lineEdit_stuName->text();
        className = ui->classComboBox_stuClass->currentText();

        QString temp = tr("%1$%2$%3").arg(stuNo).arg(stuName).arg(className);
        temp = temp.remove(" ");
        temp = encodeStr(temp);//加密字符串
        QByteArray baTemp = "";
        baTemp += temp.toUtf8();

        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("确认"),
                                       QString(tr("是否确认？")),
                                       QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::Yes) {
            QMessageBox::about(this,"成功","设置成功");

            fileOut.write(baTemp);
            this->hide();
        }
        else if (button == QMessageBox::No) {
        }
    }
}

void CheckIn::on_pushButton_readStuName_clicked()
{
    QString readName = "stuInfo.txt";
    if(readName.isEmpty())
    {
        //空字符串不处理，返回
        qDebug() << "文件名不能为空！";
        return;
    }
    else
    {
        //定义文件对象
        QFile fileIn(readName);
        if(!fileIn.open(QIODevice::ReadOnly))
        {
            //            qDebug() << "文件不存在！";
            return; //不处理文件
        }
        //读取并解析文件
        while(!fileIn.atEnd())
        {
            //读取一行
            QByteArray ba = fileIn.readAll();
            QString stuName = QString::fromUtf8(ba);
            if(stuName.size() <= 10)//姓名的字数不能大于10，否则错误，防止恶意破坏
            {
                qDebug() << "read_stuName:" << stuName;
            }
            else
            {
                qDebug() << "姓名错误！";
            }
        }
    }
}

QByteArray CheckIn::encodeStr(QString strSource)
{
    QByteArray baEncode = strSource.toUtf8();
    baEncode = baEncode.toBase64();
    qDebug() << "baEncode:" << baEncode;
    return baEncode;

}

QString CheckIn::decodeBa(QByteArray baEncode)
{
    QString strDecode = QString::fromUtf8(QByteArray::fromBase64(baEncode));
    qDebug() << "strDecode:" << strDecode;
    return strDecode;
}

QString CheckIn::decodeStr(QString strEncode)
{
    QByteArray ba = strEncode.toUtf8();
    QString strDecode = QString::fromUtf8(QByteArray::fromBase64(ba));
    qDebug() << "strDecode:" << strDecode;
    return strDecode;
}
