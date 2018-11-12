#include "SingleApplication.h"
#include <QWidget>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QFileInfo>
#include <QLibrary>


SingleApplication::SingleApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , bRunning(false)
    , localServer(NULL)
    , mainWindow(NULL)
{
    // 取应用程序名作为LocalServer的名字
    // 目前程序的进程名是根据文件名来的，所以有一种可能时用户更改了文件名。
    // 所以通过文件名来判断是不可靠的，应该写死在代码里
    serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    serverName = "CrankZClient";
    //qDebug()<<serverName;
    initLocalConnection();
}


////////////////////////////////////////////////////////////////////////////////
// 说明：
// 检查是否已經有一个实例在运行, true - 有实例运行， false - 没有实例运行
////////////////////////////////////////////////////////////////////////////////
bool SingleApplication::isRunning()
{
    return bRunning;
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 通过socket通讯实现程序单实例运行，监听到新的连接时触发该函数
////////////////////////////////////////////////////////////////////////////////
void SingleApplication::newLocalConnection()
{
    QLocalSocket *socket = localServer->nextPendingConnection();
    if (!socket)
        return;
    socket->waitForReadyRead(1000);
    QTextStream stream(socket);
    //其他处理
    delete socket;
    if (mainWindow != NULL)
    {
//        //激活窗口
        mainWindow->raise();
        mainWindow->activateWindow();
        mainWindow->setWindowState((mainWindow->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
//        mainWindow->show();
        QMessageBox::warning(NULL, "警告", "程序已经启动！");
    }
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 通过socket通讯实现程序单实例运行，
// 初始化本地连接，如果连接不上server，则创建，否则退出
////////////////////////////////////////////////////////////////////////////////
void SingleApplication::initLocalConnection()
{
    bRunning = false;
    QLocalSocket socket;
    socket.connectToServer(serverName);
    if(socket.waitForConnected(500))
    {
        bRunning = true;
        // 其他处理，如：将启动参数发送到服务端
        QTextStream stream(&socket);
        QStringList args = QCoreApplication::arguments();
        if (args.count() > 1)
            stream << args.last();
        else
            stream << QString();
        stream.flush();
        socket.waitForBytesWritten();

        return;
    }

    //连接不上服务器，就创建一个
    newLocalServer();
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 创建LocalServer
////////////////////////////////////////////////////////////////////////////////
void SingleApplication::newLocalServer()
{
    localServer = new QLocalServer(this);
    connect(localServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
    if(!localServer->listen(serverName))
    {
        // 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
        if(localServer->serverError() == QAbstractSocket::AddressInUseError)
        {
            QLocalServer::removeServer(serverName); // <-- 重点
            localServer->listen(serverName); // 再次监听
        }
    }
}
