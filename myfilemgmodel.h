#ifndef MYFILEMGMODEL_H
#define MYFILEMGMODEL_H
#define BOOST_NO_CXX11_SCOPED_ENUMS


#include <Windows.h>
#include <QVector>
#include <iostream>
#include <QString>
#include <QHeaderView>
#include <ShlObj.h>
#include <Objbase.h>
#include <QTextCodec>

class MyFileMgModel
{
public:
    MyFileMgModel();
    void CreateTree(QVector<WIN32_FIND_DATA> *vecFolder, QVector<WIN32_FIND_DATA> *vecFile, LPCTSTR path);
    void doAllLink(QString path, QString tempPath);
    bool doSomeFileAction(UINT act,QString from, QString to);
    bool folderExists(QString str,QString tempPath);


private:
    void MyFileMgModel::toFolder(QString name, QString path, QString tempPath);
    int numFiles;

};

#endif // MYFILEMGMODEL_H
