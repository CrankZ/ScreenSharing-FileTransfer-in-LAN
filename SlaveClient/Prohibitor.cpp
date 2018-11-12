#include "prohibitor.h"


//在注册表该目录下增加新内容
#define TASKMANAGERSystem "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"
#define TASKMANAGERExplorer "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"

HHOOK keyHook = NULL;
HHOOK mouseHook = NULL;

//键盘钩子过程
LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam )
{
    //在WH_KEYBOARD_LL模式下lParam 是指向KBDLLHOOKSTRUCT类型地址
    KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam;
    //如果nCode等于HC_ACTION则处理该消息，如果小于0，则钩子子程就必须将该消息传递给 CallNextHookEx
    if(nCode == HC_ACTION)
    {
        if((pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL)& 0x8000 && GetAsyncKeyState(VK_SHIFT)&0x8000)|| //"Ctrl+Shift+Esc"
                (pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000) || //"Ctrl+Esc"
                (pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN )|| //"Alt+Tab"
                (pkbhs->vkCode == VK_ESCAPE && pkbhs->flags &LLKHF_ALTDOWN )|| //"Alt+Esc"
                (pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN )|| // "LWIN/RWIN"
                (pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN )) //"Alt+F4"
        {
            return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
        }

        //      if(pkbhs->vkCode == VK_F12)
        //      {
        //          void unHook();
        //          qApp->quit();
        //      }
    }
    return CallNextHookEx(keyHook, nCode, wParam, lParam);
}
//鼠标钩子过程
LRESULT CALLBACK mouseProc(int nCode,WPARAM wParam,LPARAM lParam )
{

    return 1;

}


Prohibitor::Prohibitor()
{
    hook(false);
    disableTaskManager(false);
    hideTaskWindow(false);
}



//安装钩子,调用该函数即安装钩子
void Prohibitor::hook(bool flag)
{
    if (flag)
    {
        //这两个底层钩子,不要DLL就可以全局
        //底层键盘钩子
        keyHook =SetWindowsHookEx( WH_KEYBOARD_LL,keyProc,GetModuleHandle(NULL),0);
        //底层鼠标钩子
        //    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);
    }
    else
    {
        UnhookWindowsHookEx(keyHook);
        //  UnhookWindowsHookEx(mouseHook);
    }
}

//添加注册屏蔽Ctrl+Alt+del
void Prohibitor::disableTaskManager(bool flag)
{
    // 屏蔽ctrl + alt +del 需要修改注册表的值， 取得管理员权限， 关闭360等杀毒软件
    //true的话就执行0x00000001也就是屏蔽
    int value = flag ? 0x00000001 : 0x00000000;
    QSettings *settings = new QSettings(TASKMANAGERSystem, QSettings::NativeFormat);
    settings->setValue("DisableTaskMgr", value); //任务管理器
    settings->setValue("DisableChangePassword", value); //更改密码
    settings->setValue("DisableLockWorkstation", value); //锁定计算机
    settings->setValue("DisableSwitchUserOption", value); //切换用户

    QSettings *settings2 = new QSettings(TASKMANAGERExplorer, QSettings::NativeFormat);
    settings2->setValue("NoLogOff", value); //注销

    delete settings;
    delete settings2;
}

//屏蔽任务栏
void Prohibitor::hideTaskWindow(bool flag)
{
    task=FindWindow(L"Shell_TrayWnd",NULL);
    if(flag)
        ShowWindow(task,SW_HIDE);
    else
        ShowWindow(task,SW_SHOW);
}
