#ifndef JUSTENUM_H
#define JUSTENUM_H


const int UDP_MAX_SIZE=1024;

#define PORT 555
#define EXAMPORT 667

enum MessageType{
    CM_tScreen1,
    CM_tScreen2,
    CM_tOpenScreen,
    CM_tCloseScreen,
    CM_tHeartBeat,
    CM_tDownload,
    CM_tWatchYou,
    CM_tCheckIn,
    CM_tRemoveStuInfo,
    CM_tExamPaperInfo,
    CM_tExamStart,
    CM_tMasterIP,
    CM_tStopWatch,
    CM_tCursorPos,
    CM_tExamConfirm,


    CM_sDownloadInfo,
    CM_sAlive,
    CM_sStuName,
    CM_sScreen,
    CM_sExam,
    CM_sStuInfo,
};

//包头
struct PackageHeader
{
   //包头大小(sizeof(PackageHeader))
   unsigned int uTransPackageHdrSize;
   //当前包的大小(sizeof(PackageHeader)+数据长度)
   unsigned int uTransPackageSize;
   //数据的总大小
   unsigned int uDataSize;
   //数据被分成包的个数
   unsigned int uDataPackageNum;
   //数据包当前的帧号
   unsigned int uDataPackageCurrIndex;
   //数据包在整个数据中的偏移
   unsigned int uDataPackageOffset;
};
//组包结构体
struct UdpUnpackData
{
   unsigned char data[1024*1000];
//    unsigned char *data;
   int length;
};


#endif // JUSTENUM_H
