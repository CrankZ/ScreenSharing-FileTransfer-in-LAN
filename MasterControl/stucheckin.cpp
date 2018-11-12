#include "stucheckin.h"
#include "ui_stucheckin.h"

StuCheckIn::StuCheckIn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StuCheckIn)
{
    ui->setupUi(this);

    this->setLayout(ui->verticalLayout);
}

StuCheckIn::~StuCheckIn()
{
    delete ui;
}

void StuCheckIn::on_addClassNameButton_clicked()
{
    if(ui->classNameLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this,"班级错误","班级不能为空！");
    }
    else
    {
        for(int i = 0;i <= ui->classNameComboBox->count();i++)//这里有等于号是因为要包括0的情况，不然第一次没法添加
        {
            if(ui->classNameComboBox->itemText(i) ==  ui->classNameLineEdit->text())
            {
                QMessageBox::about(this,"重复添加","班级已存在，不可重复添加！");
                break;
            }
            else
            {
                QMessageBox::about(this,"添加成功","添加成功！");
                ui->classNameComboBox->addItem(ui->classNameLineEdit->text());
                break;
            }
        }
    }
    ui->classNameLineEdit->clear();
}

bool StuCheckIn::classCheck()
{
    if(ui->classNameComboBox->count() <= 0)
    {
        QMessageBox::warning(this,"班级错误","班级不可为空，请添加班级");
        return false;
    }
    else
    {
        return true;
    }
    return false;
}

bool StuCheckIn::classAdd()
{
    if(classCheck())
    {
        className.clear();
        for(int i = 0; i < ui->classNameComboBox->count() ;i++)
        {
            QString strClassName = ui->classNameComboBox->itemText(i);
            className.append(strClassName);
        }
        return true;
    }
    return false;
}

void StuCheckIn::on_pushButton_checkIn_clicked()
{
    //发送一个命令，就是一个学生端弹出窗口
    //然后等待接受
    if(classAdd())
    {
        sender->sendList(CM_tCheckIn,className);
        QMessageBox::about(NULL, "温馨提示", "发送成功！！\n 请学生填写个人信息");
        this->hide();
    }
}

void StuCheckIn::on_pushButton_removeStuInfo_clicked()
{
    sender->sendCommand(CM_tRemoveStuInfo);
}
