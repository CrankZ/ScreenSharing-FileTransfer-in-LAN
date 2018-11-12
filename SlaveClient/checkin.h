#ifndef CHECKIN_H
#define CHECKIN_H

#include <QWidget>
#include <QFile>
#include <QDebug>
#include <QMessageBox>

#include "justenum.h"

namespace Ui {
class CheckIn;
}

class CheckIn : public QWidget
{
    Q_OBJECT

public:
    explicit CheckIn(QWidget *parent = 0);
    ~CheckIn();
private:
    Ui::CheckIn *ui;


public:
    bool isFileExist();
    void addClassNameItem(QList<QString> className);

private:
    QByteArray encodeStr(QString strSource);
    QString decodeBa(QByteArray baEncode);
    QString decodeStr(QString strEncode);

private slots:
    void on_pushButton_confirm_clicked();

    void on_pushButton_readStuName_clicked();
};

#endif // CHECKIN_H
