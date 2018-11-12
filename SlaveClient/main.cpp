#include "widget.h"
#include "singleapplication.h"
#include <QSplashScreen>

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);


    //单实例进程，或者说防止程序多开
    SingleApplication a(argc, argv);
    if (!a.isRunning()) {
        QPixmap pix("://resource/img/mate.jpg");
        QSplashScreen splash(pix);
        splash.show();
        splash.setCursor(Qt::BlankCursor);
        //        Sleep(1*oneSec);


        Widget w;
        a.mainWindow = &w;
//        w.show();
        w.hide();
        a.setQuitOnLastWindowClosed(false);

        splash.finish(&w);//程序启动画面结束
        return a.exec();
    }
    return 0;
}
