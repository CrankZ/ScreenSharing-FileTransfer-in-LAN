#include "examtest.h"
#include "ui_examtest.h"

ExamTest::ExamTest(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExamTest)
{
    ui->setupUi(this);
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime())+ qrand());

//    setWindowFlags (Qt::FramelessWindowHint);
//    setWindowFlags (Qt::WindowStaysOnTopHint);

//    setWindowFlags (Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);//置顶，无边框


    vbox = new QVBoxLayout();
    wgNothing = new QWidget();
    wgNothing->setLayout(vbox);

    scrollArea = new QScrollArea();
    scrollArea->setWidget(wgNothing);
    scrollArea->setWidgetResizable(false);
//    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    timeStartNum = 8*3600+8*60+8;    //计数时间，单位秒
    startExamTimer = new QTimer;
    connect(startExamTimer,&QTimer::timeout,
            this,&ExamTest::startExamTimerSlot);


    startExamTimer->start(1*1000); //start为QTIME自己的函数

    udp = new QUdpSocket;
    //    this->setLayout(ui->verticalLayout);
    this->centralWidget()->setLayout(ui->verticalLayout);

    ui->label_warning->setText("考试即将开始\n请填写个人信息");
    ui->verticalLayout->addWidget(ui->label_warning);
    scrollArea->hide();
    ui->label_warning->adjustSize();
    ui->label_warning->show();

    ui->verticalLayout->addWidget(ui->label_stuInfoIsEmpty);
    ui->label_stuInfoIsEmpty->hide();


    ui->verticalLayout->addWidget(scrollArea);
    vbox->setAlignment(Qt::AlignTop);
    //    isAnsMode();
    //    selectRightAns();
    //    this->showMaximized();

//    baseInfo();

    //    completion();




    examInfo = new ExamInfo;
    prohibitor = new Prohibitor;
    //    setAttribute(Qt::WA_DeleteOnClose);
    prohibitor->hook(false);
    prohibitor->disableTaskManager(false);
    prohibitor->hideTaskWindow(false);

//    setStuInfo();
//    getStuInfo();
    skinLoad();

    ui->pushButton_submit->setDisabled(true);
    ui->lineEdit_score->hide();

//    randomNumList(100);


//    loadChoice();
//    loadChoice();
//    selectRigthAnsChoice();
//    checkAnsIsRightAll();

//    QString type = "选择题";
//    timeStartNum = 3;
//    QString c = ui->plainTextEdit_c->toPlainText();
////    si->setPaperChoice(c);//把题目文件保存在sendInfo中
//    loadChoice(type, c);
//    selectRigthAnsChoice(type);


//    type = "听力";
//    QString tl = ui->plainTextEdit_tl->toPlainText();
////    si->setPaperChoice(c);//把题目文件保存在sendInfo中
//    loadChoice(type, tl);
//    selectRigthAnsChoice(type);

////    randomSelect();


//    type = "完形填空";
//    QString comp = ui->plainTextEdit_comp->toPlainText();
////    si->setpaperComprehension(comp);
//    loadChoice(type, comp);
//    selectRigthAnsChoice(type);

//    type = "选词填空";
//    QString xctk = ui->plainTextEdit_xctk->toPlainText();
////    si->setpaperComprehension(xctk);
//    loadChoice(type, xctk);
//    selectRigthAnsChoice(type);


//    type = "填空题";
//    QString tk = ui->plainTextEdit_tk->toPlainText();
//    loadCompletion(type, tk);
//    selectRightAnsCom(type);

//    type = "XXXXX";
////    QString tk = ui->plainTextEdit_tk->toPlainText();
//    loadCompletion(type, tk);
//    selectRightAnsCom(type);


//    randomSelect();

//    checkAnsIsRightAll();



}

ExamTest::~ExamTest()
{
    delete ui;
    prohibitor->hook(false);
    prohibitor->disableTaskManager(false);
    prohibitor->hideTaskWindow(false);
}

void ExamTest::skinLoad()
{
    // 加载CSS文件
    QString strPath = QCoreApplication::applicationDirPath();
    //    qDebug()<<strPath;
    QString strCssFile = ":/resource/qss/cm.css";
    QFile fCss(strCssFile);
    if( !fCss.open(QFile::ReadOnly))
    {
        //        qDebug() << tr("css File %1 load false").arg(strCssFile);
        return;
    }
    QString strCssContent(fCss.readAll());
    setStyleSheet(strCssContent);
    fCss.close();
}

/**
 * 防止学生作弊取词
 * 关闭常见取词软件
 */
void ExamTest::killProcess()
{
    QProcess *process=new QProcess;
    process->execute("TASKKILL /IM PowerWord.exe /F");//金山
    process->execute("TASKKILL /IM YoudaoDict.exe /F");//有道
    process->execute("TASKKILL /IM Lingoes.exe /F");//Lingoes
}



void ExamTest::baseInfo()
{
//    si->setMAC(getMACAdress());
//    si->setStuName("朱松梁XX");
//    si->setStuClass("软件1508");
}

void ExamTest::DealWithExamInfo(ExamInfo *examInfo)
{
    isReceived = false;
    startWaitingConfirm = false;
    // 先把接收到的试卷内容赋值给这个类的全局变量
    // 等待后面增加学生考试相关数据之后，再发送给教师端
    this->examInfo = examInfo;


    timeStartNum = examInfo->time;
    showTime(timeStartNum);
    for(int i = 0; i < examInfo->className.count(); i++)
    {
        QString className = examInfo->className.at(i);
        qDebug() << "className:" << className;
        ui->classComboBox_stuClass->addItem(className);
    }

    QHash<QString, QMap<int, QString>> paper;
    paper = examInfo->paper;
    for(int i = 0; i < paper.count(); i++) {
        QString typeName = paper.keys().at(i);
        QMap<int, QString> typeInfo = paper.values().at(i);
        // 后面一个map没必要循环，只有一个
        int perScore = typeInfo.keys().at(0);
        QString typePaper = typeInfo.values().at(0);
        loadAuto(typeName, typePaper);
    }
}


void ExamTest::reformStr(QString split, QString &paperEnd)
{
    if(split == "A)")
    {
        paperEnd.replace("A)","[A]");
        paperEnd.replace("B)","[B]");
        paperEnd.replace("C)","[C]");
        paperEnd.replace("D)","[D]");
        paperEnd.replace("E)","[E]");
        paperEnd.replace("F)","[F]");
        paperEnd.replace("G)","[G]");
        paperEnd.replace("H)","[H]");
        paperEnd.replace("I)","[I]");
        paperEnd.replace("J)","[J]");
        paperEnd.replace("K)","[K]");
        paperEnd.replace("L)","[L]");
        paperEnd.replace("M)","[M]");
        paperEnd.replace("N)","[N]");
        paperEnd.replace("O)","[O]");
    }
    else if(split == "A.")
    {
        paperEnd.replace("A.","[A]");
        paperEnd.replace("B.","[B]");
        paperEnd.replace("C.","[C]");
        paperEnd.replace("D.","[D]");
        paperEnd.replace("E.","[E]");
        paperEnd.replace("F.","[F]");
        paperEnd.replace("G.","[G]");
        paperEnd.replace("H.","[H]");
        paperEnd.replace("I.","[I]");
        paperEnd.replace("J.","[J]");
        paperEnd.replace("K.","[K]");
        paperEnd.replace("L.","[L]");
        paperEnd.replace("M.","[M]");
        paperEnd.replace("N.","[N]");
        paperEnd.replace("O.","[O]");
    }

}

bool ExamTest::savePaper(QString saveName,QString paper)
{
    QString strSaveName = saveName;

    QFile fileOut(strSaveName);
    if(! fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {

        return false;
    }
    QByteArray baTemp = "";
    if(paper.isEmpty())
    {
        qDebug() << "试卷内容为空，无法保存";
    }
    else
    {
        baTemp += paper.toLocal8Bit();
        fileOut.write(baTemp);
        return true;
    }
    fileOut.close();
    return false;
}





/**
 * 把正确答案存入的在rightAns这个map中，仅仅用于单独存放的答案
 */
void ExamTest::addChoiceRightAns(QString type, QString str)
{
    QString strAll = str;
    if(strAll.contains("-答案-"))
    {
        QStringList strAllSplit = strAll.split("答案");
        int last = strAllSplit.count() - 1;//因为这里可能在题目里也有"答案"这个字样
        strAll = strAllSplit[last];
    }

    QMap<int, QString> rightAnsChoice;
    QString strLine = strAll.trimmed();
    strLine = strLine.remove(QRegExp("\\s"));//去掉所有空格
    strLine = strLine.remove(QRegExp("[0-9]"));//去掉所有数字
    strLine = strLine.remove("(");//去掉所有(符号
    //            ////qDebug() << "x:" << strLine;
    if(strLine.contains(")"))
    {
        //                ////qDebug() << strLine;
        QStringList strList = strLine.split(")");
        int ansCount = 0;//这个是题目序号
        for(int i = 0;i < strList.count(); i++)
        {
            QString strRightAns = strList[i];
            // 排除非选项，比如中文题目等等
            // 也许可能通过奇偶数来判断，选项应该只是偶数
            if(!strRightAns.isEmpty() &&
                    regExIsContainsEnglish(strRightAns) &&
                    strRightAns.toLocal8Bit().length() == 1)//答案必须为字母，而且必须是单个字符，当然不能为空
            {
                int rightAnsIdx = 0;

                if(compTitleNum > 0)
                {
                    rightAnsIdx = ansCount + choiceTitleNum;
                }
                else
                {
                    rightAnsIdx = ansCount;
                }
                rightAnsChoice.insert(rightAnsChoice.count(),strRightAns);//把正确结果写入realAns
                qDebug() << "rightAnsIdx:" << rightAnsIdx << ":" << rightAnsChoice;
                ansCount++;
            }
        }
    }
    qDebug() << "type:" << type << "rightAnsChoice.count():" << rightAnsChoice.count();
    rightAnsChoiceMap.insert(type, rightAnsChoice);
}

// 检查所有答案
void ExamTest::checkAnsIsRightAll()
{
    checkAnsIsRightChoiceAll();
    checkAnsIsRightCompletionAll();
}
/**
 * 检查所有题型
 */
void ExamTest::checkAnsIsRightChoiceAll()
{
    for(int i = 0; i < rightAnsChoiceMap.count(); i++)
    {
        QString type = rightAnsChoiceMap.keys().at(i);
        checkAnsIsRightChoice(type);
    }
}

void ExamTest::selectRightAnsAll()
{
    selectRigthAnsChoiceAll();
    selectRightAnsComAll();
}

void ExamTest::selectRigthAnsChoiceAll()
{
    for (int i = 0;i < rightAnsChoiceMap.count(); i++) {
        QString type = rightAnsChoiceMap.keys().at(i);
        selectRigthAnsChoice(type);
    }
}
/**
 * 选中正确选项，仅供测试使用
 */
void ExamTest::selectRigthAnsChoice(QString type)
{
    QMap<int, QString> rightAns;
    QList<QSplitter*> lSp;
    rightAns = rightAnsChoiceMap.value(type);
    lSp = lSpMap.value(type);

    //可以把学生正确与否的结果也保存为map，然后序列化之后发送
    QMapIterator<int, QString> it(rightAns);//迭代器
    int titleNum = 0;
    while (it.hasNext())
    {
        it.next();
        // 从1开始是因为0是题目Label
        for(int choiceNum = 1; choiceNum < lSp.at(titleNum)->count();choiceNum++)
        {

            //choice为当前遍历的radio序号，1234.
            QRadioButton *radioB = (QRadioButton*)lSp.at(titleNum)->widget(choiceNum);
            if(it.key() == titleNum) {//如果当前的题目和保存的rightAns一样，就是题目的序号
                QStringList c =
                {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O"};


                //正确答案
                int py;//正确的结果
                for(int i = 0;i < c.count();i++) {
                    if(it.value() == c.at(i)) {
                        py = i;
                    }
                }

                if(choiceNum - 1 == py) {
                    radioB->setChecked(true);
                }
            }
        }
        titleNum++;
    }
}

void ExamTest::selectRightAnsComAll()
{
    for (int i = 0; i < rightAnsComMap.count(); i++)
    {
        QString type = rightAnsComMap.keys().at(i);
        selectRightAnsCom(type);
    }
}
void ExamTest::selectRightAnsCom(QString type)
{
    QMap<int, QMap<int, QString>> rightAnsCom;
    rightAnsCom = rightAnsComMap.value(type);
    QList<QSplitter*> lSpCompletion;
    lSpCompletion = lSpCompletionMap.value(type);
    for (int questionNum = 0; questionNum < rightAnsCom.count(); questionNum++)
    {
        QMap<int, QString> rightAnsComOneBlank = rightAnsCom.value(questionNum);
        int lineEditNum = 1;
        for(int blankNum = 0;blankNum < rightAnsComOneBlank.count(); blankNum++)
        {
            QLineEdit *le = (QLineEdit*)lSpCompletion.at(questionNum)->widget(lineEditNum);
            le->setText(rightAnsComOneBlank.value(blankNum));
            lineEditNum = lineEditNum + 3;
        }
    }
}

/**
 * 填空题
 */
void ExamTest::loadCompletion(QString type, QString str)
{
    QLabel *labelType = new QLabel;
    QVBoxLayout *vboxTemp = new QVBoxLayout;
    labelType->setText(type);
    labelType->setStyleSheet("font: 16pt;");
//    vbox->addWidget(labelType);
    vboxTemp->addWidget(labelType);

    QStringList lineSplit = str.split("\n");
    // 一道填空题里面可能又多个填空的问题
    QMap<int, QString> rightAnsComOneBlank;

    QList<QSplitter*> lSpCompletion;
    int realTitleNum = 0;
    for(int titleNumCom = 0;titleNumCom < lineSplit.count();titleNumCom++) {
        QString strLine = lineSplit[titleNumCom];//每次从文件中读取一行
        if(!strLine.contains("<completion>")) {
            continue;
        }
        QStringList splitCount = strLine.split("<completion>");
        QStringList splitA = strLine.split("<completion>");
        for(int i = 0; i < splitCount.count() - 1; i++) {
            lSpCompletion.append(new QSplitter);
            QStringList splitB = splitA[i+1].split("</completion>");
            QString completionRightAns  = splitB[0];
            rightAnsComOneBlank.insert(i,completionRightAns);
            QLabel *lb = new QLabel(this);
            QLabel *lb2 = new QLabel(this);
            QLineEdit *le = new QLineEdit(this);
            QString headText = splitA[0];// 答案前面的内容
            splitA[0] = ""; //head只有一次使用，其他的都放在end里面了
            if(i == 0) {
                headText = tr("%1.%2").arg(++globalTitleNum).arg(headText);
            }
            lb->setText(headText);
//            lb->setStyleSheet("background-color: rgb(255, 85, 0);");
//            lb->adjustSize();
            // 模式sizePolicy是prefer，会自动伸展。
            // 这里有多长就显示多长就行了
            lb2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            lb2->sizePolicy();
            lSpCompletion.at(realTitleNum)->addWidget(lb);

            le->setText("");
            le->adjustSize();
            lSpCompletion.at(realTitleNum)->addWidget(le);
            QString endText = splitB[1];    //答案后面的内容
            lb2->setText(endText);
//            lb2->setStyleSheet("background-color: rgb(0, 255, 127);");
//            lb2->adjustSize();
            lb2->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            lSpCompletion.at(realTitleNum)->addWidget(lb2);
            lSpCompletion.at(realTitleNum)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        }
        vboxTemp->addWidget(lSpCompletion.at(realTitleNum));

        rightAnsCom.insert(realTitleNum, rightAnsComOneBlank);
        rightAnsComOneBlank.clear();
        realTitleNum++;
    }
    lSpCompletionMap.insert(type, lSpCompletion);
    vbox->addLayout(vboxTemp);

    wgNothing->adjustSize();
    qDebug() << "rightAnsCom:" << rightAnsCom;
    rightAnsComMap.insert(type, rightAnsCom);
    rightAnsCom.clear();
//    totalScoreCompletion = titleNumCom * examInfo->perScoreCompletion;
//    int totalScore = totalScoreCompletion + totalScoreChoice;
//    si->setTotalScore(totalScore);
//    qDebug() << "totalSocre:" << totalScore;
}

void ExamTest::checkAnsIsRightCompletionAll()
{
    for(int i = 0;i < rightAnsComMap.count(); i++)
    {
        QString type = rightAnsComMap.keys().at(i);
        checkAnsIsRightCompletion(type);
    }
}

//检查答案是否正确，并且标记起来
void ExamTest::checkAnsIsRightChoice(QString type)
{
    QList<QSplitter*> lSp = lSpMap.value(type);
    QMap<int, QString> rightAns = rightAnsChoiceMap.value(type);
    QMap<int, QString> eachTypeRealAns;

    // 当前题型的每题的分数
    QMap<int, QString> typeInfo = examInfo->paper.value(type);
    int perTypeScore = typeInfo.keys().at(0);

    //可以把学生正确与否的结果也保存为map，然后序列化之后发送
    QMapIterator<int, QString> it(rightAns);//迭代器
    int titleNum = 0;
    int typeScore = 0;
    QMap<int ,bool> isRightChoice;
    examInfo->stuEachTypeScore.insert(type, 0);
    while (it.hasNext()) {

        it.next();
        //qDebug() << tr("题目序号：%1,答案：%2" ).arg(it.key()).arg(it.value());

        // 从1开始是因为0是题目
        for(int choiceNum = 1; choiceNum < lSp.at(titleNum)->count();choiceNum++) {
            //choice为当前遍历的radio序号，1234.
            QRadioButton *radioB = (QRadioButton*)lSp.at(titleNum)->widget(choiceNum);

            if(it.key() == titleNum) {//如果当前的题目和保存的rightAns一样，就是题目的序号
                // 题目对错默认为false，因为可能会有没填的情况
                isRightChoice.insert(titleNum, false);
                //默认为单选，因为map里的QString暂时只存了一个字符
                //自动选择正确答案

                //                        radioB->setChecked(true);

                //正确答案
                QStringList c =
                {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O"};

                int py;//正确的结果
                for(int i = 0;i < c.count();i++) {
                    if(it.value() == c.at(i)) {
                        py = i;
                    }
                }

                // 先把正确的题目弄成绿色的
                if(choiceNum - 1 == py) {
                    //                    radioB->setChecked(true);
                    // 如果是对的，就统一设置成绿色
                    radioB->setStyleSheet("background-color: rgb(0, 255, 0);");
                }

                //                ////qDebug() << choiceNum;

                // 如果按钮被选择了
                if(radioB->isChecked()) {
                    for(int i = 0;i <= c.count();i++) {
                        // 把学生选择的结果保存起来
                        if(choiceNum == i) {
                            eachTypeRealAns.insert(titleNum, c.at(i-1));
                            examInfo->stuRealAnsChoice.insert(type, eachTypeRealAns);
                        }
                    }
                    // 保存学生的对错
                    // 如果对了
                    if(choiceNum - 1 == py) {
//                        si->score = si->score + examInfo->perScoreChoice;
//                        si->setRealAnsIsRight(titleNum,true);//这里面只存题号与是否正确

                        // 后期学生端也可能需要单独题型的分数
                        typeScore += perTypeScore;
                        examInfo->stuEachTypeScore.insert(type, typeScore);

                        isRightChoice.insert(titleNum, true);

                    } else {
                        // 这里，如果他没选就进不来
//                        si->setRealAnsIsRight(titleNum,false);
                        // 如果是错的，就弄成红色
                        radioB->setStyleSheet("background-color: rgb(255, 0, 0);");

                    }
                }
            }
        }
        titleNum++;
    }
    examInfo->stuAnsIsRight.insert(type, isRightChoice);
}

void ExamTest::checkAnsIsRightCompletion(QString type)
{
    QMap<int, QMap<int, QString>> rightAnsCom;
    QMap<int, QMap<int, QString>> stuRealAnsCom;
    rightAnsCom = rightAnsComMap.value(type);
    QList<QSplitter*> lSpCompletion;
    lSpCompletion = lSpCompletionMap.value(type);
    QMap<int, QString> typeInfo = examInfo->paper.value(type);
    int perTypeScore = typeInfo.keys().at(0);
    int typeScore = 0;
    QMap<int, bool> isRightCom;
    for (int questionNum = 0; questionNum < rightAnsCom.count(); questionNum++) {
        QMap<int, QString> rightAnsComOneBlank = rightAnsCom.value(questionNum);
        QMap<int, QString> realAnsComOneBlank;
        int lineEditNum = 1;
        for(int blankNum = 0;blankNum < rightAnsComOneBlank.count(); blankNum++) {
            QLineEdit *le = (QLineEdit*)lSpCompletion.at(questionNum)->widget(lineEditNum);

            if(le->text() == rightAnsComOneBlank.value(blankNum)) {
                typeScore += perTypeScore;
                examInfo->stuEachTypeScore.insert(type, typeScore);
                isRightCom.insert(questionNum, true);

                le->setStyleSheet("background-color: rgb(0, 255, 0);");
            } else {
                // 一题中的选项有一个错的，这个题就是错的
                isRightCom.insert(questionNum, false);
                le->setStyleSheet("background-color: rgb(255, 0, 0);");
            }

            lineEditNum = lineEditNum + 3;

            realAnsComOneBlank.insert(blankNum, le->text());
        }
        stuRealAnsCom.insert(questionNum, realAnsComOneBlank);
    }
    examInfo->stuAnsIsRight.insert(type, isRightCom);
    examInfo->stuRealAnsCom.insert(type, stuRealAnsCom);
}

/**
 * 从本地文件读取学生信息
 *
 * 暂时不需要这个功能，防止XX
 */
void ExamTest::getStuInfo()
{
    QString readName = "stuInfo.txt";

    //定义文件对象
    QFile fileIn(readName);
    if(!fileIn.open(QIODevice::ReadOnly)) {
        return;
    } else {
        QByteArray ba = fileIn.readAll();
        QString sAll = QString::fromUtf8(ba);

        sAll = decodeStr(sAll);//解密字符串

        if(sAll.contains("$"))
        {
            QStringList sList = sAll.split("$");

            //            qDebug() << "sList:" << sList;
            QString stuNo = sList[0];
            QString stuName = sList[1];
            QString stuClassName = sList[2];
            if(!stuNo.isEmpty()) {
                ui->lineEdit_stuNo->setText(stuNo);
            }
            if(!stuName.isEmpty())  {
                ui->lineEdit_stuName->setText(stuName);
            }
            if(!stuClassName.isEmpty()) {
                ui->classComboBox_stuClass->addItem(stuClassName);
                ui->classComboBox_stuClass->setCurrentText(stuClassName);
            }
        }
    }
}

void ExamTest::makeJustRadio(QString strAll)
{
    QStringList xx = strAll.split("<content>");
    QStringList contentSplit = xx[1].split("</content>");//这个是content

    JustRadio = contentSplit[1];
//    //qDebug() << "JustRadio:" << JustRadio;

    QStringList JustNextContentSplit = strAll.split(JustRadio);
    qDebug() << "JustNextContentSplit.count():" << JustNextContentSplit.count();
    justNextContent = "";

    if(JustNextContentSplit.count() > 2)
    {
        justNextContent += JustNextContentSplit[JustNextContentSplit.count() - 2];
        justNextContent += JustRadio;
        for(int i = 0;i < JustNextContentSplit.count() - 2;i++)
        {
            justNextContent += JustNextContentSplit[i+2];
        }
    }
    else
    {
        for(int i = 0;i < JustNextContentSplit.count() - 1;i++)
        {
            justNextContent += JustNextContentSplit[i+1];
        }
    }

    wgNothing->adjustSize();
}

void ExamTest::makeContent(QString type, int i, QString strAll)
{

    QStringList xx = strAll.split("<content>");
    QStringList contentSplit = xx[1].split("</content>");//这个是content
    QString content = contentSplit[0];
//    qDebug() << "i:" << i <<"content:" << content;

    lLbContent.append(new QLabel);

//    lLbContent.at(i)->setAlignment(Qt::AlignTop);
    lLbContent.at(i)->setWordWrap(true);
    lLbContent.at(i)->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    lLbContent.at(i)->setText(content);
    lLbContent.at(i)->adjustSize();
    lLbContentMap.insert(type, lLbContent);
    qDebug() << "lLbContent.count():" << lLbContent.count();
    qDebug() << "lLbContentMap.count():" << lLbContentMap.count();
    wgNothing->adjustSize();
}

void ExamTest::loadAuto(QString type, QString str)
{
    if(str.contains("</completion>")) {
        loadCompletion(type, str);
    } else {
        loadChoice(type, str);
    }
}

/**
 * 选择题题型，基本通杀所有选择题题型
 * 单选，阅读理解，选词填空等
 * TODO: 不支持多选
 */
void ExamTest::loadChoice(QString type,QString str)
{

    QString strAll = str;
    // 没有<content>就加一个，方便统一的处理
    // 后期进行判断，如果<content>标签里面的内容为空，就不添加
    if(!strAll.contains("<content>")) {
        strAll = "<content></content>" + strAll;
    }
    if(strAll.contains("<content>")) {
        QStringList strAllSplit = strAll.split("</content>");
        for(int i = 0;i < strAllSplit.count() - 1;i++) {
            makeJustRadio(strAll);
            makeContent(type, i, strAll);

            strAll = justNextContent;
            QString split;
            if(JustRadio.contains("A)") &&
                    JustRadio.contains("B)") &&
                    JustRadio.contains("C)")) {
                split = "A)";
            } else if (JustRadio.contains("A.") &&
                     JustRadio.contains("B.") &&
                     JustRadio.contains("C.")) {
                split = "A.";
            }

            if(JustRadio.contains(split)) {
                // 暂时还没法有多重换行符\n

                QStringList strAllSplit = JustRadio.split(split);
//                qDebug() << "JustRadio:" << endl << JustRadio;
                QStringList temp = strAllSplit[0].split("\n");
                QString firstQuestion = temp[temp.count() - 2];

                QString paperStart = firstQuestion + "\r\n";
                for(int i = 1; i < strAllSplit.count(); i++) {
                    //            ////qDebug() << strAllSplit[i];
                    paperStart += split + strAllSplit[i];//因为以"A)"为分界线的话，要切割很多块，所以要组合起来
                }
                ////qDebug() << "paperStart:" << paperStart;


                QString paperEnd;
                if(paperStart.contains("-答案-")) {
                    QStringList paperStartSplit = paperStart.split("-答案-");
                    int index = paperStartSplit.count() - 2;//确保是最后一个含有"答案"字符串
                    paperEnd = paperStartSplit[index];
                } else {
                    paperEnd = paperStart;
                }

                reformStr(split, paperEnd);
                paperEnd.remove("-");
                paperEnd.replace("\n\r","");
                paperEnd.replace("\r","");
                choiceNum2(paperEnd);
                makeWidget(type, paperEnd);
            }
        }
    }
    wgNothing->adjustSize();
    addChoiceRightAns(type, str);

    lLbContent.clear();
    lSp.clear();
    titleNum = 0;
}

/**
 * 负责题目的展示，包括题目和选项
 */
void ExamTest::makeWidget(QString type, QString str)
{
    str = str.replace("\n\n", "\n");
    QStringList splitByLine = str.split("\n");
    for(int i = 0;i < splitByLine.count();i++)
    {
        QString strLine = splitByLine[i];
        //                strLine = strLine.remove(QRegExp("\\s"));//去掉所有空格，去掉空格的话，英文题目会有问题
        strLine = strLine.remove("\n");//去掉题目中的回车
//                strLine = strLine.remove(QRegExp("[0-9]"));//去掉所有数字，可能题目中有数字
        strLine = strLine.remove(".");//去掉所有.
        strLine = strLine.remove("-");//去掉所有-
        //                isAnsMode();//这里由addChoiceRightAns()这个函数添加标准答案

        // 防止有空白
        if(strLine.isEmpty()){
            continue;
        }
        // 正常的题型，每题四个选项
        if(offset == 0)
        {
            if(i % (5+offset) == 0)//题目
            {
                lSp.append(new QSplitter(Qt::Vertical,0));
                QLabel *lbTitle = new QLabel(this);
                QString temp = strLine.mid(0,4);
                //如果本身有序号，就不自动填写了
                if(regExIsContainsNumber(temp)) {
//                    lbTitle->setText(strLine);
                    lbTitle->setText(tr("%1.%2").arg(++globalTitleNum).arg(strLine));
                } else {
                    lbTitle->setText(tr("%1.%2").arg(++globalTitleNum).arg(strLine));
                }

                lbTitle->adjustSize();
                lSp.at(titleNum)->addWidget(lbTitle);
                titleNum++;
            } else {
                //以下是选项
                QRadioButton *radioB = new QRadioButton(this);
                QString choiceText;
                if(strLine.trimmed().isEmpty()) {
                    choiceText = "";
                } else {
                    choiceText = strLine.trimmed();
                }
                radioB->setText(choiceText);
                radioB->adjustSize();
                int spNum = titleNum - 1;//sp的数量和title数量一致
                lSp.at(spNum)->addWidget(radioB);


                //一个title四个choice
                if(lSp.at(spNum)->count() == (5+offset)) {
                    //禁止splitter调节大小
                    for(int q = 0;q < (4+offset); q++) {
                        QSplitterHandle *splitterHandle = lSp.at(spNum)->handle(q);//使splitter大小不可调节
                        if(splitterHandle) {
                            splitterHandle->setDisabled(true);
                        }
                    }
                    lSp.at(spNum)->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
                    lSp.at(spNum)->adjustSize();
                }
            }
        }
        else if(offset > 0) {

            // 如果offset>0，也就是说总题目数>4
            // 那么就是英语中的选词填空

            // 题目格式如下，每两行一个选项

            //1.
            //A)B)C)D)E)F)G)H)I)J)K)L)M)N)O)
            //2.
            //A)B)C)D)E)F)G)H)I)J)K)L)M)N)O)
            //3.
            //A)B)C)D)E)F)G)H)I)J)K)L)M)N)O)

            //题目
            if(i % 2 == 0) {
                lSp.append(new QSplitter(Qt::Horizontal,0));
                QLabel *lbTitle = new QLabel(this);
                QString temp = strLine.mid(0,4);
                //如果本身有标题，就不自动填写了
                if(regExIsContainsNumber(temp)) {
//                    lbTitle->setText(strLine);
                    lbTitle->setText(tr("%1.%2").arg(++globalTitleNum).arg(strLine));
                } else {
                    lbTitle->setText(tr("%1.%2").arg(++globalTitleNum).arg(strLine));
                }

                lbTitle->adjustSize();
                //                    //qDebug() << "titleNumtitleNumtitleNum:" << titleNum;
                lSp.at(titleNum)->addWidget(lbTitle);
                titleNum++;
                qDebug() << "WXTK,titleNum:" << titleNum;
            } else {
                // 选项都被改成了[]，这里把选项提取出来
                QStringList strLineSplit = strLine.split("][");
//                //qDebug() << "strLineSplit.count():" << strLineSplit.count();
                //qDebug() << offset + 5;
                QString choice = "";
                int spNum = lSp.count() - 1;//sp的数量和title数量一致
//                qDebug() << "compTitleNum:" << compTitleNum << ",lSp.count():" << lSp.count();
                for(int i = 0;i < strLineSplit.count();i++) {
                    QRadioButton *radioB = new QRadioButton(this);
                    choice = strLineSplit[i];
                    if(choice.contains("[")) {
                        QStringList strLinesplitB = choice.split("[");
                        choice = strLinesplitB[1];
                    }
                    if(choice.contains("]")) {
                        QStringList strLinesplitB = choice.split("]");
                        choice = strLinesplitB[0];
                    }
//                    //qDebug() << "choice:" << choice;

                    QString choiceText = tr("[%1]").arg(choice);
                    radioB->setText(choiceText);
                    radioB->adjustSize();
                    lSp.at(spNum)->addWidget(radioB);
                    lSp.at(spNum)->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
                }
            }
        }
    }
    titleNumMap.insert(type, titleNum);
    lSpMap.insert(type, lSp);


    // 是不是随机模式
    bool isRandom = examInfo->isRandomMap.value(type);
    if(isRandom) {
        randomAddWidget(type);
    } else {
        normalAddWidget(type);
    }
}

// 都是选择题，抽取出来
void ExamTest::normalAddWidget(QString type)
{
    int titleNum = 0;

    titleNum = titleNumMap.value(type);

    QList<QLabel*> lLbContent =
            lLbContentMap.value(type);

    QVBoxLayout *vboxTemp = new QVBoxLayout;
    QLabel *labelType = new QLabel;
    labelType->setText(type);
    labelType->setStyleSheet("font: 16pt;");

    if(lLbContent.count() <= 1) {
        vboxTemp->addWidget(labelType);
    }

    QList<QSplitter*> lSp = lSpMap.value(type);
    // 根据题目数量来判断该插哪里
    for(int i = 0;i < lSp.count();i++) {
        // 如果有题目的话
        if(lLbContent.count() > 0) {
            int temp = (5+offset);
            temp = 5;   // 选项数量
            // 所有选项个数 除以 题目个数
            temp = titleNum / lLbContent.count();
            if(i % (5) == 0) {
                int contentIdx = i / (temp);
                if(contentIdx < lLbContent.count() &&
                        !lLbContent.at(contentIdx)->text().isEmpty()) {
                    vboxTemp->addWidget(lLbContent.at(contentIdx));
                }
            }
        }
        vboxTemp->addWidget(lSp.at(i));
    }
    vbox->addLayout(vboxTemp);
}

void ExamTest::randomAddWidget(QString type)
{
    int titleNum = 0;

    titleNum = titleNumMap.value(type);

    QList<QLabel*> lLbContent =
            lLbContentMap.value(type);

    for(int i = 0;i < lLbContent.count(); i++) {
        QString content = lLbContent.at(i)->text();
        if(!content.isEmpty()) {
//            QMessageBox::warning(this, "警告", "不是单选题，禁止随机！\n已切换到非随机模式");
            normalAddWidget(type);
            return;
        }
    }

    QVBoxLayout *vboxTemp = new QVBoxLayout;
    QLabel *labelType = new QLabel;
    labelType->setText(type);
    labelType->setStyleSheet("font: 16pt;");

    if(lLbContent.count() <= 1) {
        vboxTemp->addWidget(labelType);
    }

    QList<QSplitter*> lSp = lSpMap.value(type);
    // 根据题目数量来判断该插哪里
    for(int i = 0;i < lSp.count();i++) {
        // 如果有题目的话
        if(lLbContent.count() > 0) {
            int temp = (5+offset);
            temp = 5;   // 选项数量
            // 所有选项个数 除以 题目个数
            temp = titleNum / lLbContent.count();
            if(i % (5) == 0) {
                int contentIdx = i / (temp);
                if(contentIdx < lLbContent.count() &&
                        !lLbContent.at(contentIdx)->text().isEmpty()) {
                    vboxTemp->addWidget(lLbContent.at(contentIdx));
                }
            }
        }
    }

    QList<int> randomList;
    randomList = randomNumList(lSp.count());
    for(int i = 0;i < randomList.count();i++) {
        int randomIdx = randomList.at(i);
        vboxTemp->addWidget(lSp.at(randomIdx));
    }
    vbox->addLayout(vboxTemp);
}

QString ExamTest::getHostIPV4Address()
{
    QString localHostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(localHostName);

    foreach(QHostAddress address,info.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol) {
            qDebug() << address.toString();
            return address.toString();
        }
    }
}

QList<int> ExamTest::randomNumList(int maxNum)
{
    //一定范围，且不重复的随机数
    //从0开始，所以到不了maxNum
    int i,j;
    QList<int> numbersList;

    QString MAC = getMACAdress();
    MAC = MAC.remove(QRegExp("[A-Z]"));
    MAC = MAC.remove(QRegExp(":"));
    MAC = MAC.mid(MAC.length()-4,MAC.length());
    int randomTime = QTime(0,0,0).secsTo(QTime::currentTime());
    int randomSeed = MAC.toInt() * (randomTime % 1000);
    qDebug() << "randomSeed:" << randomSeed;
    qsrand(randomSeed);
    for(i=0;i<maxNum;i++) {
        numbersList.append(qrand()%maxNum);
        bool flag=true;
        while(flag) {
            for(j=0;j<i;j++) {
                if(numbersList[i]==numbersList[j]){
                    break;
                }
            }
            if(j<i) {
                numbersList[i]=rand()%maxNum;
            }
            if(j==i) {
                flag=!flag;
            }
        }
    }
//    for(i=0;i<maxNum;i++)
//    {
//        qDebug()<<numbersList[i];
//    }
    return numbersList;
}

void ExamTest::sendExamInfo()
{
    if(isReceived){
       return;
    }
    if(ui->lineEdit_stuNo->text().isEmpty() ||
            ui->lineEdit_stuName->text().isEmpty() ||
            ui->classComboBox_stuClass->currentText().isEmpty()) {
        QMessageBox::about(this,"无法提交","个人信息不完成，无法交卷！");
        return;
    }
    examInfo->stuMAC = getMACAdress();
    examInfo->stuNo = ui->lineEdit_stuNo->text();
    examInfo->stuName = ui->lineEdit_stuName->text();
    examInfo->stuClass = ui->classComboBox_stuClass->currentText();
    examInfo->stuRealScore = figureRealScore();
    sender->sendExamInfo(CM_sExam, examInfo);
//    saveStuInfo();
    startWaitingConfirm = true;
}

/**
 * 教师端只有在考试结束之后才会发送XX
 * 也不一定，假如考试没结束，但是学生提交了，而且提交的教师端没收到怎么办
 */
void ExamTest::tExamConfirmSlot(QMap<QString, bool> stuConfirmMap)
{
//    qDebug() << "收到startWaitingConfirm:" << startWaitingConfirm;
    if(startWaitingConfirm) {
//        qDebug() << "stuConfirmMap.count():" << stuConfirmMap.count();
        if(stuConfirmMap.count() <= 0) {
            return;
        }
        for (int i = 0;i < stuConfirmMap.count(); i++) {
            QString stuNo = stuConfirmMap.keys().at(i);
    //        examInfo->stuNo = ui->lineEdit_stuNo->text();
            // 如果map里有这个学生，就说明已经确认收到，则停止等待确认
            if (stuNo == examInfo->stuNo) {
                qDebug() << "tExamConfirmSlot收到";
                isReceived = true;
                startWaitingConfirm = false;
                ui->pushButton_submit->setDisabled(true);
                return;
            }
        }
        qDebug() << "确实没收到，重新发送";
        sendExamInfo();
    }
}

int ExamTest::figureRealScore()
{
    int realScore = 0;
    QMap<QString, int> stuEachTypeScore = examInfo->stuEachTypeScore;
    qDebug() << "stuEachTypeScore.count():" << stuEachTypeScore.count();
    for (int i = 0; i < stuEachTypeScore.count(); i++) {
        QString typeName = stuEachTypeScore.keys().at(i);
        int typeScore = stuEachTypeScore.values().at(i);
        qDebug() << "typeName:" << typeName << "typeScore:" << typeScore;
        realScore += typeScore;
    }
    qDebug() << "总分：" << realScore;
    return realScore;
}

// 用来在主界显示时间
// 主要是把秒转换为 小时:分钟:秒 的形式
void ExamTest::showTime(int timeStartNum)
{
    hour = timeStartNum / 3600;
    min = (timeStartNum - (hour * 3600)) / 60;
    sec = timeStartNum - (hour * 3600) - (min * 60);

    QString hourStr = tr("%1").arg(hour);
    QString minStr = tr(":%1").arg(min);
    QString secStr = tr(":%1").arg(sec);
    if(hour < 10) {
        hourStr = tr("0%1").arg(hour);
    }
    if(min < 10) {
        minStr = tr(":0%1").arg(min);
    }
    if(sec < 10) {
        secStr = tr(":0%1").arg(sec);
    }

    QString timeStr = (hourStr + minStr + secStr);

    ui->lcdNumber->display(timeStr);
    //    ui->lcdNumber->adjustSize();  //加上这个就太小了

}

void ExamTest::startExamTimerSlot()
{
    if (isStartExam && !isTimeOver()) {
        showTime(--timeStartNum);

        // 如果学号或姓名或班级为空，则无法答题
        if(ui->lineEdit_stuNo->text().isEmpty() ||
                ui->lineEdit_stuName->text().isEmpty() ||
                ui->classComboBox_stuClass->currentText().isEmpty()) {
            scrollArea->setDisabled(true);
            ui->label_stuInfoIsEmpty->show();
        } else {
            scrollArea->setEnabled(true);
            ui->label_stuInfoIsEmpty->hide();
        }
    }
}

bool ExamTest::isTimeOver()
{
    if(hour == 0 && min == 0 && sec == 0) {
        startExamTimer->stop();
        startExamTimer->deleteLater();
        timeOverTodo();
        return true;
    }
    return false;
}

void ExamTest::timeOverTodo()
{
    prohibitor->hook(false);
    prohibitor->disableTaskManager(false);
    prohibitor->hideTaskWindow(false);

    if(!isReceived){
        checkAnsIsRightAll();//因为里面有操纵的分数，所以要先执行这个，不然分数出不来
        sendExamInfo();//发送学生信息和答案
    }

    int stuRealScore = examInfo->stuRealScore;
    ui->label_score->setText(tr("%1分").arg(stuRealScore));
    setSpDisable();
    ui->label_warning->hide();
    scrollArea->show();
//    ui->verticalLayout->removeWidget(ui->label_warning);
    ui->splitter_stuNo->setDisabled(true);
    ui->splitter_stuName->setDisabled(true);
    ui->splitter_stuClass->setDisabled(true);
    //        ui->pushButton_submit->setDisabled(true);

    // 最大化，有标题栏
    setWindowState(Qt::WindowMaximized);
    backupExamInfo();
    QMessageBox::about(this,"时间到","考试已经结束\n试卷提交成功！");
}

/**
 * 防止学生数据丢失
 * 考试结束后自动备份
 */
void ExamTest::backupExamInfo()
{
    //程序目录下新建data文件夹，把数据都存进去
    QString currentPath =  QDir::currentPath();
    QString fullPath = currentPath + "/data";
    qDebug() << "fullPath:" << fullPath;
    DelDir(fullPath);//删除目录
    isDirExist(fullPath);//创建当前文件夹

    // 根据学生姓名和当前时间命名

    QString filename = tr("data//0.dat");
    qDebug() << filename;
    QFile myFile(filename);
    if (myFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&myFile);
        out << examInfo;
    }
    myFile.flush();
    myFile.close();
    qDebug() << "myFile.size():" << myFile.size();

    // 所有文件保存到data文件夹后，统一打包成zip文件
    QString stuName = ui->lineEdit_stuName->text();
    QDateTime currentDateTime =QDateTime::currentDateTime();
    QString currentDateTimeStr = currentDateTime.toString("yyyyMMddhhmmss");

    QString saveName = tr("考试记录//%1%2.zip").arg(stuName).arg(currentDateTimeStr);
    compressDir(fullPath, saveName);
    DelDir(fullPath);//删除目录
}

/**
 * 压缩指定文件夹里面的所有内容
 */
void ExamTest::compressDir(QString dir,QString saveName)
{
    //选择压缩的文件夹，然后在这个文件夹上一层保存压缩的文件
    QStringList dirSplit = dir.split("/");
    QString lastDir = dirSplit[dirSplit.length() - 1];//得到最后一个目录

    QStringList dirSplit2 = dir.split(lastDir);//然后按照最后一个目录来分割
    QString savePath = dirSplit2[0];
    QString saveFile;
    if(saveName.isEmpty()) {
        saveFile = tr("%1/stuRes.dat").arg(savePath);
    } else {
        saveFile = saveName;
    }

    if (false==JlCompress::compressDir(saveFile,dir)) {
        qDebug()<<"compress failed";
    } else {
        qDebug()<<"compress successed";
    }
}

bool ExamTest::DelDir(QString path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        } else { // 递归删除
            DelDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}


bool ExamTest::isDirExist(QString fullPath)
{
    QDir dir(fullPath);
    if(dir.exists()){
        return true;
    } else {
        bool ok = dir.mkdir(fullPath);//只创建一级子目录，即必须保证上级目录存在
        return ok;
    }
}

int ExamTest::choiceNum2(QString strAll)
{
    QStringList c =
    {"[A]","[B]","[C]","[D]","[E]","[F]","[G]","[H]","[I]","[J]","[K]","[L]","[M]","[N]","[O]"};

    for(int i = c.count() - 1;i >= 0 ;i--) {
        if(strAll.contains(c.at(i))) {
            offset = i - 3;
            //qDebug() << tr("i:%1,offset:%2,c:%3").arg(i).arg(offset).arg(c.at(i));
            return offset;
        }
    }
    return 0;
}


// 设置所有选项不可选
void ExamTest::setSpDisable()
{
    for(int i = 0; i < lSp.count();i++) {
        lSp.at(i)->setDisabled(true);
    }
}

void ExamTest::startExam()
{
    isStartExam = true;
    startExamTimer->start(1*1000);
    scrollArea->show();
    ui->label_warning->hide();

    // 禁止任何操作
    prohibitor->hook(true);
    prohibitor->disableTaskManager(true);
    prohibitor->hideTaskWindow(true);

    ui->pushButton_submit->setEnabled(true);
//    randomSelect();
}

void ExamTest::on_pushButton_submit_clicked()
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("确认交卷"),
                                   QString(tr("是否确认交卷？")),
                                   QMessageBox::Yes | QMessageBox::No);

    if (button == QMessageBox::No) {

    }
    else if (button == QMessageBox::Yes) {
        ui->label_warning->setText("试卷提交成功\n请等待考试结束");
//        ui->verticalLayout->addWidget(ui->label_warning);
        // 如果考试还没结束，不允许提交答案
        // 就显示请等待考试结束
        if(!isTimeOver()) {
            ui->label_warning->show();
            ui->label_stuInfoIsEmpty->hide();
            scrollArea->hide();
        }

        // 只能时间到了才检查，因为检查一次就加一次分数，检查答案是否正确
        checkAnsIsRightAll();
        // 发送学生信息和答案
        sendExamInfo();
        // 应该是时间结束后才能看到分数
        //        int stuRealScore = examInfo->stuRealScore;
        //        ui->label_score->setText(tr("%1分").arg(stuRealScore));
        ui->splitter_stuNo->setDisabled(true);
        ui->splitter_stuName->setDisabled(true);
        ui->splitter_stuClass->setDisabled(true);
//        ui->pushButton_submit->setDisabled(true);
    }
}

/**
 * 保存学生姓名班级等信息
 */
void ExamTest::saveStuInfo()
{
    QString saveName = "stuInfo.txt";

    QFile fileOut(saveName);
    if(! fileOut.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        return;
    }

    QString stuNo;
    QString stuName;
    QString className;

    if(ui->lineEdit_stuName->text().isEmpty()) {
        QMessageBox::warning(this,tr("错误"),tr("姓名禁止为空！"));
    } else {
        stuNo = ui->lineEdit_stuNo->text();
        stuName = ui->lineEdit_stuName->text();
        className = ui->classComboBox_stuClass->currentText();

        QString temp = tr("%1$%2$%3").arg(stuNo).arg(stuName).arg(className);
        temp = temp.remove(" ");
        temp = encodeStr(temp);//加密字符串
        QByteArray baTemp = "";
        baTemp += temp.toUtf8();
        fileOut.write(baTemp);
    }
}

bool ExamTest::regExIsContainsChinese(QString str)
{
    //如果包含汉字，则说明是题目的选项
    QRegExp rx("[\u4e00-\u9fa5]");
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        list << rx.cap(0);
        if(!rx.cap(0).isEmpty()) {
            return true;
        }
        //        qDebug() << list;
        pos += rx.matchedLength();
    }
    return false;
}

bool ExamTest::regExIsContainsEnglish(QString str)
{
    //如果包含汉字，则说明是题目的选项
    QRegExp rx("[A-Z]");
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        list << rx.cap(0);
        if(!rx.cap(0).isEmpty()) {
            return true;
        }
        //        qDebug() << list;
        pos += rx.matchedLength();
    }
    return false;
}

bool ExamTest::regExIsContainsNumber(QString str)
{
    //如果包含汉字，则说明是题目的选项
    QRegExp rx("[0-9]");
    QStringList list;
    int pos = 0;
    while ((pos = rx.indexIn(str, pos)) != -1) {
        list << rx.cap(0);
        if(!rx.cap(0).isEmpty()) {
            return true;
        }
        //        qDebug() << list;
        pos += rx.matchedLength();
    }
    return false;
}

//获得MAC地址
QString ExamTest::getMACAdress()
{
    QList<QNetworkInterface> NetList;//网卡链表
    int NetCount = 0;//网卡个数
    int Neti=0;
    QNetworkInterface thisNet;//所要使用的网卡

    NetList = QNetworkInterface::allInterfaces();//获取所有网卡信息
    NetCount = NetList.count();//统计网卡个数

    for(Neti = 0; Neti < NetCount; Neti++){//遍历所有网卡
        if( NetList[Neti].isValid() ){//判断该网卡是否是合法
            thisNet = NetList[Neti];//将该网卡置为当前网卡
            break;
        }
    }
    return(thisNet.hardwareAddress());//获取该网卡的MAC
}

void ExamTest::closeEvent(QCloseEvent *event)
{
    bool isTimeOver = (hour == 0 && min == 0 && sec == 0);
    if(isStartExam && !isTimeOver) {
        QMessageBox::warning(this,tr("禁止关闭"),tr("考试未结束，禁止关闭程序！"));
        event->ignore();
    } else {
        QMessageBox::StandardButton button;
        button = QMessageBox::question(this, tr("退出程序"),
                                       QString(tr("确认退出？")),
                                       QMessageBox::Yes | QMessageBox::No);

        if (button == QMessageBox::No) {
            event->ignore();  //忽略退出信号，程序继续运行
        }
        else if (button == QMessageBox::Yes) {
            event->accept();  //接受退出信号，程序退出
        }
    }
}

QByteArray ExamTest::encodeStr(QString strSource)
{
    QByteArray baEncode = strSource.toUtf8();
    baEncode = baEncode.toBase64();
    qDebug() << "baEncode:" << baEncode;
    return baEncode;

}

QString ExamTest::decodeBa(QByteArray baEncode)
{
    QString strDecode = QString::fromUtf8(QByteArray::fromBase64(baEncode));
    qDebug() << "strDecode:" << strDecode;
    return strDecode;
}

QString ExamTest::decodeStr(QString strEncode)
{
    QByteArray ba = strEncode.toUtf8();
    QString strDecode = QString::fromUtf8(QByteArray::fromBase64(ba));
    qDebug() << "strDecode:" << strDecode;
    return strDecode;
}


// 仅供测试使用
void ExamTest::setStuInfo()
{
    QString MAC = getMACAdress();
    MAC = MAC.remove(":");
    qDebug() << "MACMACMAC:" << MAC;
    ui->lineEdit_stuNo->setText(MAC);
    ui->lineEdit_stuName->setText(MAC);
}

void ExamTest::randomSelect()
{
    qDebug() << "lSpMap.count():" << lSpMap.count();
    for(int i = 0;i < lSpMap.count(); i++) {
        QList<QSplitter*> lSp = lSpMap.values().at(i);
        qDebug() << "key:" << lSpMap.keys().at(i) << "count:" << lSp.count();
        for(int i = 0;i < lSp.count();i++) {
            //包含randomMax
            int randomMax = 5;
            int randomMin = 1;
            int random = randomMin+qrand()/(double)(RAND_MAX/(randomMax+1-randomMin));
            qDebug() << "random:" << random;
            if(!(random == 5)) {
                QRadioButton *rb = (QRadioButton*)lSp.at(i)->widget(random);
                rb->setChecked(true);
            }
        }
    }
}

