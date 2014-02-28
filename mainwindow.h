#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myfilemgmodel.h"
#include <Objbase.h>
#include <ShellAPI.h>
#include <QTableWidget>
#include <QInputDialog>
#include <QMenu>
#include <QContextMenuEvent>
#include <QMessageBox>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();   

private slots:

    void toRootDir();
   // void setBtn();
    void on_action_3_triggered();
    void doLink();
    void folderOrFile(int i, int j);
    void on_action_2_triggered();

    void on_action_triggered();

    void on_action_4_triggered();

    void on_action_5_triggered();

    void on_action_6_triggered();
    void toFolder();
   //void toRoot();

    void on_action_9_triggered();

    void on_action_8_triggered();

    void on_comboBox_activated(const QString &arg1);

    void on_action_temp_triggered();

private:
    Ui::MainWindow *ui;
    MyFileMgModel mod;
    QVector<WIN32_FIND_DATA> *vecCurrentFolder;
    QVector<WIN32_FIND_DATA> *vecCurrentFile;
    QString path;
    QString pathCopy;
    QString tempPath;
    QMenu *menu;
    bool copy;
    int ind;
   // bool file;
    void findNextFolder(int i, int j);
    void myOpenFile(int i, int j);
    void displayData();
    void contextMenuEvent(QContextMenuEvent *event);
    void addToMenu(QString capt, int slot, QString key);
   // void CreateTree();
};

#endif // MAINWINDOW_H
