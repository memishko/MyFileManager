#include "mainwindow.h"
#include "ui_mainwindow.h"

//void CopyFolder(CSTRING strFrom, CString strTo);
QString getPath(QString str);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->setColumnWidth(0,25);
    ui->tableWidget->setColumnWidth(1,170);
    ui->tableWidget->setColumnWidth(2,60);
    ui->tableWidget->setColumnWidth(3,120);
    ui->tableWidget->verticalHeader()->hide();
    ui->pushButton_2->setVisible(false);
    ui->action_2->setEnabled(false);
    ui->action_7->setCheckable(true);
    ui->action_7->setChecked(false);
    ui->tableWidget->setEditTriggers(0);
    connect(ui->tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(folderOrFile(int,int)));
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(toRootDir()));
    connect(ui->pushButton_2,SIGNAL(clicked()),this,SLOT(doLink()));
    connect(ui->pushButton_3,SIGNAL(clicked()),this,SLOT(toFolder()));

    QPushButton *popupButton = new QPushButton/*(tr("Pop&up Button"))*/;
    menu = new QMenu(this);

    addToMenu(QString::fromLocal8Bit("копировать"),1,"ctrl+c");
    addToMenu(QString::fromLocal8Bit("вставить"),2,"ctrl+v");
    addToMenu(QString::fromLocal8Bit("удалить"),3,"del");
    addToMenu(QString::fromLocal8Bit("вырезать"),4,"ctrl+x");
    addToMenu(QString::fromLocal8Bit("переименовать"),5,"");
    addToMenu(QString::fromLocal8Bit("создать папку"),6,"");
    addToMenu(QString::fromLocal8Bit("создать файл"),7,"");

    ind = 0;
    DWORD dr = GetLogicalDrives();

        for( int i = 0; i < 26; i++ )
        {
            if((dr>>i)&0x00000001){
               // dd[0] =  char(65+i); dd[1] = ':'; dd[2] = '\\'; dd[3] = 0;
                ui->comboBox->addItem(QString(char(65+i)) + ":\\");
                if(i<2) ind++;
            }
        }
    ui->comboBox->setCurrentIndex(ind);
   // SetCurrentDirectory(L"c:\\");  
    path = "C:\\*";
    tempPath = "C:\\myFMgTemp\\";
    ui->lineEdit->setText(path);
    displayData();
}

void MainWindow::addToMenu(QString capt,int slot,QString key){
    QAction *act = new QAction(this);
    act->setText(capt);
    switch(slot){
        case 1 : connect(act,SIGNAL(triggered()),this,SLOT(on_action_triggered())); break;
        case 2 : connect(act,SIGNAL(triggered()),this,SLOT(on_action_2_triggered())); break;
        case 3 : connect(act,SIGNAL(triggered()),this,SLOT(on_action_4_triggered())); break;
        case 4 : connect(act,SIGNAL(triggered()),this,SLOT(on_action_5_triggered())); break;
        case 5 : connect(act,SIGNAL(triggered()),this,SLOT(on_action_6_triggered())); break;
        case 6 : connect(act,SIGNAL(triggered()),this,SLOT(on_action_8_triggered())); break;
        case 7 : connect(act,SIGNAL(triggered()),this,SLOT(on_action_9_triggered())); break;
    }
    if(key!="") act->setShortcut(key);
    menu->addAction(act);
}

QString toNormTime(FILETIME dtime){
    FILETIME ftime;
    SYSTEMTIME stime;
    //FILETIME dtime = vecCurrentFolder->at(i).ftLastAccessTime;
    if(FileTimeToLocalFileTime(&dtime,&ftime))
       if(FileTimeToSystemTime(&ftime,&stime))
           return QString::number(stime.wYear)+"-"+QString::number(stime.wMonth)+"-"+QString::number(stime.wDay)+" "
                   +QString::number(stime.wHour)+":"+QString::number(stime.wMinute)+":"+QString::number(stime.wSecond);
}

void MainWindow::displayData(){//построение дерева папок и файлов по входному пути
    if(path.trimmed()=="*") path = "C:\\*";
    if(!mod.folderExists(path.left(path.length()-2),"")){
        QMessageBox::warning(this,"error",QString::fromLocal8Bit("неверно указано имя папки"));
        path = path.left(path.length() - path.lastIndexOf("\\")+1);
        return ;
    }
    vecCurrentFolder = new QVector<WIN32_FIND_DATA>;
    vecCurrentFile = new QVector<WIN32_FIND_DATA>;
    mod.CreateTree(vecCurrentFolder,vecCurrentFile,(LPCWSTR)path.utf16());
    ui->tableWidget->setRowCount(vecCurrentFolder->size()+vecCurrentFile->size());
    for(int i = 0;i<vecCurrentFolder->size();i++){
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::fromWCharArray(vecCurrentFolder->at(i).cFileName)));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::fromLocal8Bit("папка")));    
           // ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(stime.wYear)+"-"+QString::number(stime.wMonth)+"-"+QString::number(stime.wDay)+" "+QString::number(stime.wHour)+":"+QString::number(stime.wMinute)+":"+QString::number(stime.wSecond)));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(toNormTime(vecCurrentFolder->at(i).ftLastAccessTime)));
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QPixmap(":/folder"),0));
    }
    for(int i = 0;i<vecCurrentFile->size();i++){
        QString name = QString::fromWCharArray(vecCurrentFile->at(i).cFileName);        
       // }else{
            if(name.indexOf(".")!=-1){
                ui->tableWidget->setItem(i+vecCurrentFolder->size(),2,new QTableWidgetItem(name.right(name.length()-name.lastIndexOf(".")-1)));
                ui->tableWidget->setItem(i+vecCurrentFolder->size(),0,new QTableWidgetItem(QPixmap(":/"+name.right(name.length()-name.lastIndexOf(".")-1)),0));
            }else{
                ui->tableWidget->setItem(i+vecCurrentFolder->size(),2,new QTableWidgetItem(QString::fromLocal8Bit("файл")));
                ui->tableWidget->setItem(i+vecCurrentFolder->size(),0,new QTableWidgetItem(QPixmap(":/no"),0));
            }
            ui->tableWidget->setItem(i+vecCurrentFolder->size(),1,new QTableWidgetItem(QString::fromWCharArray(vecCurrentFile->at(i).cFileName)));
            if(path.indexOf(tempPath)!=-1){
                name = name.left(name.length()-4);
                ui->tableWidget->setItem(i+vecCurrentFolder->size(),1,new QTableWidgetItem(name.right(name.length()-name.lastIndexOf(".,")-2)));
                name.replace(".,","\\");
                ui->tableWidget->setItem(i+vecCurrentFolder->size(),2,new QTableWidgetItem(name));
            }    
          //  ui->tableWidget->setItem(i+vecCurrentFolder->size(),2,new QTableWidgetItem(QString::number(stime.wYear)+"-"+QString::number(stime.wMonth)+"-"+QString::number(stime.wDay)+" "+QString::number(stime.wHour)+":"+QString::number(stime.wMinute)+":"+QString::number(stime.wSecond)));
        ui->tableWidget->setItem(i+vecCurrentFolder->size(),3,new QTableWidgetItem(toNormTime(vecCurrentFile->at(i).ftLastAccessTime)));
    }
    path = path.left(path.length()-1);
    ui->lineEdit->setText(path);
    for(int i = 0;i<ui->tableWidget->rowCount();i++)
        ui->tableWidget->setRowHeight(i,16);    
    //ui->tableWidget->setIconSize(QSize(15,15));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::folderOrFile(int i, int j){
    if(path.indexOf("*")!=-1) path.remove(path.indexOf("*"),1);
    if(i<vecCurrentFolder->size())
        findNextFolder(i,j);
    else
        myOpenFile(i,j);
}

void MainWindow::findNextFolder(int i,int j){       
        path+= QString::fromWCharArray(vecCurrentFolder->at(i).cFileName)+"\\*";
        if(path.right(4)=="..\\*"){
            path.remove(path.length()-5,4);
            path.remove(path.lastIndexOf("\\")+1,path.length()-path.lastIndexOf("\\")-2);
        }       
    displayData();
}

void MainWindow::myOpenFile(int i, int j){
    if(ShellExecuteW(0,L"open",(path+QString::fromWCharArray(vecCurrentFile->at(i-vecCurrentFolder->size()).cFileName)).utf16(),0,0,SW_SHOWNORMAL)!=INVALID_HANDLE_VALUE) std::cout<<"туц";
}

void MainWindow::toRootDir(){
    path = ui->comboBox->currentText()+"*";
    displayData();
}

void MainWindow::doLink(){
    ui->comboBox->setCurrentIndex(ind);
    CreateDirectory(tempPath.utf16(),0);
    if(ui->action_7->isChecked()){
        mod.doSomeFileAction(FO_DELETE,tempPath.left(tempPath.length()-1),"");
        CreateDirectory(tempPath.utf16(),0);
    }
    ui->pushButton_2->setVisible(false);
    mod.doAllLink(ui->lineEdit->text(),tempPath);
    path = tempPath+"*";   
    displayData();

}

void MainWindow::on_action_3_triggered(){
    ui->pushButton_2->setVisible(true);
}

void MainWindow::on_action_2_triggered(){
  //  ui->action_2->setEnabled(true);
   // WIN32_FIND_DATA structFD;
    if(path.indexOf("*")!=-1) path.remove(path.indexOf("*"),1);    
   // std::cout<<"сейчас... "<<path.toStdString()<<std::endl;*/
            if(copy)
                mod.doSomeFileAction(FO_COPY,pathCopy,path);
            else
                mod.doSomeFileAction(FO_MOVE,pathCopy,path);
    path += "*";
    displayData();
}

void MainWindow::on_action_triggered(){
    copy = true;
    ui->action_2->setEnabled(true);
    if(path.indexOf("*")!=-1) path.remove(path.indexOf("*"),1);
    if(ui->tableWidget->selectionModel()->currentIndex().row()<vecCurrentFolder->size())
      //  file = false;
        pathCopy = path + QString::fromWCharArray(vecCurrentFolder->at(ui->tableWidget->selectionModel()->currentIndex().row()).cFileName);
    else
        pathCopy = path+"\\" + QString::fromWCharArray(vecCurrentFile->at(ui->tableWidget->selectionModel()->currentIndex().row()-vecCurrentFolder->size()).cFileName);

}

void MainWindow::on_action_4_triggered()//удалить
{
    QString tmp = path;
    if(ui->tableWidget->selectionModel()->currentIndex().row()<vecCurrentFolder->size())
        tmp += QString::fromWCharArray(vecCurrentFolder->at(ui->tableWidget->selectionModel()->currentIndex().row()).cFileName);        
    else
        tmp += QString::fromWCharArray(vecCurrentFile->at(ui->tableWidget->selectionModel()->currentIndex().row()-vecCurrentFolder->size()).cFileName);
    mod.doSomeFileAction(FO_DELETE,tmp,"");
    path += "*";
    displayData();
}

void MainWindow::on_action_5_triggered(){
    ui->action_2->setEnabled(true);
    copy = false;
    if(ui->tableWidget->selectionModel()->currentIndex().row()<vecCurrentFolder->size())
        pathCopy = path + QString::fromWCharArray(vecCurrentFolder->at(ui->tableWidget->selectionModel()->currentIndex().row()).cFileName);
    else
        pathCopy = path+"\\" + QString::fromWCharArray(vecCurrentFile->at(ui->tableWidget->selectionModel()->currentIndex().row()-vecCurrentFolder->size()).cFileName);
}

void MainWindow::on_action_6_triggered(){
    bool bOk;
    QString tmp = "";
    if(ui->tableWidget->selectionModel()->currentIndex().row()<vecCurrentFolder->size())
        tmp += QString::fromWCharArray(vecCurrentFolder->at(ui->tableWidget->selectionModel()->currentIndex().row()).cFileName);
    else
        tmp += QString::fromWCharArray(vecCurrentFile->at(ui->tableWidget->selectionModel()->currentIndex().row()-vecCurrentFolder->size()).cFileName);
    QString str = QInputDialog::getText( 0,
                                         QString::fromLocal8Bit("переименовать"),
                                         QString::fromLocal8Bit("Новое имя:"),
                                         QLineEdit::Normal,
                                         tmp,
                                         &bOk
                                        );
    str = path + str;
    str += '\0';
    tmp = path + tmp;
    if (bOk)
        mod.doSomeFileAction(FO_RENAME,tmp,str);
    path += "*";
    displayData();
}

void MainWindow::toFolder(){
    path = ui->lineEdit->text();
    if(path.lastIndexOf("\\")!=path.length()-1) path += "\\";
    if(path.indexOf("*")==-1) path +="*";
    displayData();
}

void MainWindow::on_action_9_triggered(){//создать файл
    bool bOk;
    QString str = QInputDialog::getText( 0,
                                         QString::fromLocal8Bit("Создать файл"),
                                         QString::fromLocal8Bit("имя:"),
                                         QLineEdit::Normal,
                                         "",
                                         &bOk
                                        );
    HANDLE hndl;
    if(bOk)
        if(str.trimmed()!=""){
            hndl = CreateFile((path+str).utf16(),GENERIC_READ,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
            if(hndl){
                ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
                ui->tableWidget->setRowHeight(ui->tableWidget->rowCount()-1,16);
              //  ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,new QTableWidgetItem(str));
            }
    }
    CloseHandle(hndl);
    path += "*";
    displayData();
}

void MainWindow::on_action_8_triggered(){//создать папку
    bool bOk;
    QString str = QInputDialog::getText( 0,
                                         QString::fromLocal8Bit("Создать папку"),
                                         QString::fromLocal8Bit("имя:"),
                                         QLineEdit::Normal,
                                         QString::fromLocal8Bit("новая папка"),
                                         &bOk
                                        );
    if(bOk)
        if(str.trimmed()!=""){
            if(str.right(1)!="\\") str += "\\";
                if(!CreateDirectory((path + str).utf16(),0))
                    if(GetLastError()!=ERROR_ALREADY_EXISTS)
                        int err=  QMessageBox::warning(this,"error",QString::fromLocal8Bit("неверно указано имя папки"));
    }
    path += "*";
    displayData();
}

void MainWindow::on_comboBox_activated(const QString &arg1){
    ui->lineEdit->setText(arg1);
    toFolder();
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event){
    menu->exec(event->globalPos());
}

void MainWindow::on_action_temp_triggered(){
    bool bOk;
    QString str = QInputDialog::getText( 0,
                                         QString::fromLocal8Bit("изменить temp"),
                                         QString::fromLocal8Bit("текущий: ")+tempPath+QString::fromLocal8Bit("\nновый:"),
                                         QLineEdit::Normal,
                                         path,
                                         &bOk
                                        );
    if((bOk)&&(str.trimmed()!="")){
        tempPath = str;
        if(tempPath.right(1)!="\\") tempPath += "\\";
        if(!CreateDirectory(tempPath.utf16(),0))
            if(GetLastError()!=ERROR_ALREADY_EXISTS)
                int err=  QMessageBox::warning(this,"error",QString::fromLocal8Bit("неверно указано имя папки"));
        path += "*";
        displayData();
    }
}
