#ifndef Prohibitor_H
#define Prohibitor_H
#include <Windows.h>
#include <QSettings>

/**
 * 用来锁定学生电脑的相关功能
 */
class Prohibitor
{
public:
    Prohibitor();

    void hook(bool flag); //功能键
    void hideTaskWindow(bool flag); //显示任务栏
    void disableTaskManager(bool flag); //任务管理器

private:
    HWND task;
};

#endif // Prohibitor_H
