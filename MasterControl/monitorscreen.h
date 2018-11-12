#ifndef MONITORSCREEN_H
#define MONITORSCREEN_H

#include <QWidget>
#include <QKeyEvent>

#include "showscreen.h"
#include "sender.h"

namespace Ui {
class MonitorScreen;
}

class MonitorScreen : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorScreen(QWidget *parent = 0);
    ~MonitorScreen();

private:
    Ui::MonitorScreen *ui;

private:
    QObject *obj;
    QAction* actionWatch;
    
private:

    void pauseMonitor();

public:
    ShowScreen *ss1;
    ShowScreen *ss2;
    ShowScreen *ss3;
    ShowScreen *ss4;
    void setName(QString name,int fourPart);
    void setObjName(QString name,int fourPart);

    bool isWatching;

signals:
    void innerSendData(bool);
    void innerSendMonitorPause(bool);
    void startWatchingSignal();
    void stopWatchingSignal();
    void watchSingleStuScreenSignal(QString);

private slots:
    void closeEvent(QCloseEvent *event);
    void watchSingleStuScreen();

protected:
    void resizeEvent(QResizeEvent* event);
    bool eventFilter(QObject * obj, QEvent * event);
    void keyReleaseEvent(QKeyEvent *ev);
};

#endif // MONITORSCREEN_H
