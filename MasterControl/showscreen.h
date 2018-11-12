#ifndef SHOWSCREEN_H
#define SHOWSCREEN_H

#include <QWidget>
#include <QDebug>
#include "justenum.h"


namespace Ui {
class ShowScreen;
}

class ShowScreen : public QWidget
{
    Q_OBJECT

public:
    explicit ShowScreen(QWidget *parent = 0);
    ~ShowScreen();

private:
    Ui::ShowScreen *ui;

public:
    void screenRecv(QByteArray qba);

private:
    UdpUnpackData imageData;
    int recvImageNum;

signals:
    void showScreenCloseSignal();

private slots:
    void closeEvent(QCloseEvent *event);

};

#endif // SHOWSCREEN_H
