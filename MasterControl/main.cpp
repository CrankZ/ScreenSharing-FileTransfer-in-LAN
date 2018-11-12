#include "mainwindow.h"
#include "singleapplication.h"

int main(int argc, char *argv[])
{
//    // 防止多开
//    QSharedMemory shared_memory;
//    shared_memory.setKey(QString("xczx"));
//    if(shared_memory.attach()) {
//        return 0;
//    }

//    if(shared_memory.create(1)) {
//        QApplication a(argc, argv);
//        MainWindow w;
//        w.show();
//        return a.exec();
//    }


    SingleApplication a(argc, argv);
    if (!a.isRunning()) {
        MainWindow w;
        //传入一个要激活程序的窗口，当多开时会激活已有进程的窗口，且多开失败
        a.mainWindow = &w;
        w.show();

        return a.exec();
    }
    return 0;
}
