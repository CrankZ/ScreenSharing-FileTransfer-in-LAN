#ifndef StuCheckIn_H
#define StuCheckIn_H

#include <QWidget>
#include <QMessageBox>

#include "sender.h"

namespace Ui {
class StuCheckIn;
}

class StuCheckIn : public QWidget
{
    Q_OBJECT

public:
    explicit StuCheckIn(QWidget *parent = 0);
    ~StuCheckIn();

private slots:
    void on_addClassNameButton_clicked();

    void on_pushButton_checkIn_clicked();

    void on_pushButton_removeStuInfo_clicked();

private:
    Ui::StuCheckIn *ui;

    Sender *sender = new Sender;
    QList<QString> className;
    bool classCheck();
    bool classAdd();
};

#endif // StuCheckIn_H
