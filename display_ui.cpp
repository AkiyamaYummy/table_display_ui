#include "display_ui.h"

// 用于生成临时实例的构造函数
// 请在调用该函数后调用reInit()生成真正可用的实例
display_ui::display_ui(){
    ITEM_COUNT = 0; displayStrs = 0;
    MAX_PAGE = 6; BEST_RANK = 3;
    LETTER_WIDTH = 7; PAGE_BUTTON_BLOCK = 4;
    _hasPageNum = 0;
    mainMapper = pageMapper = 0;
    displayStrs = 0;
    ROW_COUNT = 0; COL_COUNT = 0;
    SUB_WIDTHS = 0; ROW_HEIGHT = 0;
    titles = 0;
    mainLayout = 0; pageNumLayout = 0;
    pageNow = 1;
}
// 构造函数
display_ui::display_ui(int r,int c,\
                int *wids,int h,QString *tit){
    reInit(r,c,wids,h,tit);
}
/*
 * 初始化实例
 *
 * 参数说明:
 * r : 展示表格的行数  c : 展示表格的列数
 * wids : 长度为c的整数型数组，规定每列表格的宽度
 * h : 规定每行表格的高度
 * tit : 长度为c的QString数组，规定每列表格的标题
 *
 */
void display_ui::reInit(int r,int c,\
                int *wids,int h,QString *tit){
    ITEM_COUNT = 0; displayStrs = 0;
    MAX_PAGE = 6; BEST_RANK = 3;
    LETTER_WIDTH = 7; PAGE_BUTTON_BLOCK = 4;
    _hasPageNum = 0;
    mainMapper = pageMapper = 0;
    displayStrs = 0;

    ROW_COUNT = r; COL_COUNT = c;
    SUB_WIDTHS = wids; ROW_HEIGHT = h;
    titles = tit;
    mainLayout = getLabelsLayout();
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    pageNumLayout = new QHBoxLayout();
    mainLayout->addLayout(pageNumLayout);
    setLayout(mainLayout);
    pageNow = 1;

    display();
}
// 析构函数，并会释放部分参数内存
display_ui::~display_ui(){
    clearMainLayout();
    mainLayout->deleteLater();
    mainLayout = 0;
    if(mainMapper){
        mainMapper->deleteLater();
        mainMapper = 0;
    }
    if(pageMapper){
        pageMapper->deleteLater();
        pageMapper = 0;
    }
}
// 对表格进行初始化
QVBoxLayout *display_ui::getLabelsLayout(){
    QVBoxLayout *res = new QVBoxLayout();
    QHBoxLayout *row;

    if(mainMapper)mainMapper->deleteLater();
    mainMapper = new QSignalMapper(this);

    labels = new ExQLabel*[ROW_COUNT+1];
    for(int i=0;i<=ROW_COUNT;i++){
        labels[i] = new ExQLabel[COL_COUNT];
        row = new QHBoxLayout();
        row->setMargin(0);
        for(int j=0;j<COL_COUNT;j++){
            labels[i][j].setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
            labels[i][j].setMinimumSize
                    (SUB_WIDTHS[j],ROW_HEIGHT);
            labels[i][j].setText(i?"":titles[j]);
            row->addWidget(&labels[i][j]);
            if(i != 0){
                mainMapper->setMapping(&labels[i][j],i);
                connect(&labels[i][j],SIGNAL(clicked())
                        ,mainMapper,SLOT(map()));
            }
        }
        res->addLayout(row);
    }
    connect(mainMapper,SIGNAL(mapped(int))
            ,this,SLOT(rowClickedSlot(int)));
    return res;
}
// 对表格部分进行析构
void display_ui::clearMainLayout(){
    for(int i=0;i<ROW_COUNT;i++)delete[] labels[i];
    delete[] labels;
    clearPageNumLayouyt();
    pageNumLayout->deleteLater();
    pageNumLayout = 0;
    while(mainLayout->count())
        delete mainLayout->takeAt(0);
}
// 展示指定的页码
void display_ui::display(int page){
    pageNumDisplay(page);
    for(int i=0;i<ROW_COUNT;i++){
        int r = (pageNow-1)*ROW_COUNT+i;
        if(r >= ITEM_COUNT){
            for(int j=i;j<ROW_COUNT;j++)
                for(int k=0;k<COL_COUNT;k++)
                    labels[j+1][k].setText("");
            break;
        }
        for(int j=0;j<COL_COUNT;j++){
            labels[i+1][j].setText(displayStrs[r][j]);
        }
    }
}
/*
 * 设置展示的表格内容
 *
 * 参数说明:
 * dis : QString型的二维数组,第一维长度为itemCon,第二维长度为COL_COUNT
 *       指定了用于展示的表格的内容
 * itemCon : 指定了用于展示的信息的项目数,也就是表格的总行数
 * memoryReleaseFlag : 指定是否要释放displayStrs的内存
 */
void display_ui::setDisplay(QString **dis,int itemCon,bool memoryReleaseFlag){
    if(memoryReleaseFlag && displayStrs){
        for(int i=0;i<ITEM_COUNT;i++){
            delete[] (displayStrs[i]);
            displayStrs[i] = 0;
        }
        delete[] displayStrs;
        displayStrs = 0;
    }
    displayStrs = dis;
    ITEM_COUNT = itemCon;
}
// 换页时,更新底部的页码栏
void display_ui::pageNumDisplay(int page){
    clearPageNumLayouyt();
    setPageNumLayout(page);
}
// 清除底部页码栏
void display_ui::clearPageNumLayouyt(){
    for(int i=0;i<toDelete.size();i++){
        delete toDelete[i];
        toDelete[i] = 0;
    }
    toDelete.clear();
    while(pageNumLayout->count())
        delete pageNumLayout->takeAt(0);
}
// 为pageNumLayout添加相应的页码按键
void display_ui::setPageNumLayout(int page){
    int tot = ITEM_COUNT/ROW_COUNT+(ITEM_COUNT%ROW_COUNT?1:0);
    if(tot < 1)tot = 1;
    if(page < 1)page = 1; if(page > tot)page = tot;
    pageNow = page;
    int sta,end;
    if(tot <= MAX_PAGE){
        sta = 1; end = tot;
    }else{
        sta = pageNow-BEST_RANK+1;
        if(sta+MAX_PAGE-1 > tot)sta = tot-MAX_PAGE+1;
        else if(sta < 1)sta = 1;
        end = sta+MAX_PAGE-1;
    }

    if(pageMapper)pageMapper->deleteLater();
    pageMapper = new QSignalMapper();

    pageNumLayout->addStretch();
    pageNumLayout->addWidget(getPageNumButton(1,pageMapper,"first"));
    pageNumLayout->addWidget(getPageNumButton
               ((1>pageNow-1)?1:(pageNow-1),pageMapper,"prev"));
    for(int i=sta;i<=end;i++){
        pageNumLayout->addWidget(getPageNumButton(i,pageMapper));
    }
    pageNumLayout->addWidget(getPageNumButton
               ((tot<pageNow+1)?tot:(pageNow+1),pageMapper,"next"));
    pageNumLayout->addWidget(getPageNumButton(tot,pageMapper,"last"));
    pageNumLayout->addStretch();
    connect(pageMapper,SIGNAL(mapped(int)),this,SLOT(displaySlot(int)));
}
// 获得指定页码的页码按键
QPushButton *display_ui::getPageNumButton(int page,QSignalMapper *mapper,\
                              QString text){
    if(!text.length())text = QString::number(page);
    QPushButton *res = new QPushButton(this);
    if(page == pageNow)res->setEnabled(false);
    res->setMaximumHeight(ROW_HEIGHT);
    res->setFixedWidth
            (text.length()*LETTER_WIDTH+PAGE_BUTTON_BLOCK);
    res->setText(text);
    mapper->setMapping(res,page);
    connect(res,SIGNAL(clicked()),mapper,SLOT(map()));
    toDelete.push_back(res);
    return res;
}
// 设定页码按键的最大个数,以及当前页码出现的最佳位置
void display_ui::setPageNum(int m,int b){
    MAX_PAGE = m; BEST_RANK = b;
}
// 设定与页码按键宽度相关的参数
void display_ui::setPageButton(int w,int b){
    LETTER_WIDTH = w; PAGE_BUTTON_BLOCK = b;
}
/*
 * 设置边框粗细和颜色
 *
 * 参数介绍 :
 * m : 中间部分的边框粗细程度
 * e : 表格边缘部分的边框粗细程度
 * c : 表格边框颜色
 */
void display_ui::setBorder(int m,int e,QString c){
    MID_BORDER = m; EDGE_BORDER = e;
    BORDER_COLOR = c;
    for(int i=0;i<=ROW_COUNT;i++)
        for(int j=0;j<COL_COUNT;j++){
            QString ss(labels[i][j].styleSheet());
            ss.append(getBorderStr(i,j));
            labels[i][j].setStyleSheet(ss);
        }
}
// 相应位置的label,其边框的部分的styleSheet,用于设置styleSheet
QString display_ui::getBorderStr(int x,int y){
    int l,r,t,b;
    l = t = 0; r = b = MID_BORDER;
    if(x == 0)t = EDGE_BORDER;
    if(y == 0)l = EDGE_BORDER;
    if(x == ROW_COUNT)b = EDGE_BORDER;
    if(y == COL_COUNT-1)r = EDGE_BORDER;
    QString res;
    res.append(QString("border-top : %1px solid %2;")
               .arg(t).arg(BORDER_COLOR));
    res.append(QString("border-bottom : %1px solid %2;")
               .arg(b).arg(BORDER_COLOR));
    res.append(QString("border-left : %1px solid %2;")
               .arg(l).arg(BORDER_COLOR));
    res.append(QString("border-right : %1px solid %2;")
               .arg(r).arg(BORDER_COLOR));
    return res;
}
// 设置某一行的文字字体
void display_ui::setRowFont(int r,QFont &f){
    for(int c=0;c<COL_COUNT;c++){
        labels[r][c].setFont(f);
    }
}
// 设置某一列的文字字体
void display_ui::setColFont(int c,QFont &f){
    for(int r=0;r<=ROW_COUNT;r++){
        labels[r][c].setFont(f);
    }
}
// 设置某一行的背景颜色
void display_ui::setRowBackground(int r,QString color){
    for(int c=0;c<COL_COUNT;c++){
        QString ss = labels[r][c].styleSheet();
        ss.append(QString("background-color: %1 ;").arg(color));
        labels[r][c].setStyleSheet(ss);
    }
}
// 设置某一列的背景颜色
void display_ui::setColBackground(int c,QString color){
    for(int r=0;r<ROW_COUNT;r++){
        QString ss = labels[r][c].styleSheet();
        ss.append(QString("background-color: %1 ;").arg(color));
        labels[r][c].setStyleSheet(ss);
    }
}
// 辅助用的槽
void display_ui::rowClickedSlot(int _r){
    this->rowClicked(_r);
}
void display_ui::displaySlot(int page){
    display(page);
}
// 点击当前表格展示的第_r行的时候,会执行此函数,一般用于在子类中被重写
void display_ui::rowClicked(int _r){
    qDebug() << "dis " << _r;
}
