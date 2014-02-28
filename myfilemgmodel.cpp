#include "myfilemgmodel.h"

MyFileMgModel::MyFileMgModel()
{

}

void MyFileMgModel::CreateTree(QVector<WIN32_FIND_DATA> *vecFolder, QVector<WIN32_FIND_DATA> *vecFile,LPCTSTR path){
    WIN32_FIND_DATA structFD;    
   // QVector<WIN32_FIND_DATA> vec;// = new QVector<QString>;
    HANDLE hFind = FindFirstFile(path,&structFD);
    if(hFind!=0)
        do{
        if(QString::fromWCharArray(structFD.cFileName)!=QString("."))
            if(structFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
                vecFolder->push_back(structFD);
            else{
                structFD.dwReserved1 = 1;
                vecFile->push_back(structFD);
            }
        }while(FindNextFile(hFind,&structFD)!=0);
    FindClose(hFind);
 //   CloseHandle(hFind);
    //for(int i=0;i<vec.size();i++)
      //  std::cout<<vec.at(i).toStdString()<<std::endl;
   // return vec;
}

void CreateLink(LPCWSTR lpszPathObj, LPSTR  lpszPathLink, LPCWSTR lpszDesc)
{
    //std::cout<<lpszPathObj<<" файл"<<std::endl;
   // std::cout<<lpszPathLink<<" €рлык"<<std::endl;
    CoInitialize(0);
    HRESULT hres;
    IShellLink  *psl;

    hres =  CoCreateInstance(CLSID_ShellLink, NULL,
    CLSCTX_INPROC_SERVER,  IID_IShellLink, (void  **)&psl);
    if(SUCCEEDED(hres))
    {
        IPersistFile  *ppf;
        psl->SetPath(lpszPathObj);
        psl->SetDescription(lpszDesc);
        hres  = psl->QueryInterface(IID_IPersistFile,  (void**)&ppf);
        if(SUCCEEDED(hres))
        {
            wchar_t  wsz[MAX_PATH];
            MultiByteToWideChar(1251, 0, lpszPathLink, -1, wsz,  MAX_PATH);
            hres = ppf->Save(wsz,  true);
            ppf->Release();
        }
        psl->Release();
    }
    CoUninitialize();
    //return  hres;
}

QString getType(QString str){
    if(str.indexOf(".")==-1) return QString::fromLocal8Bit("нету");
    return str.right(str.length()-1 - str.lastIndexOf(".",-1));
}

bool MyFileMgModel::folderExists(QString str,QString tempPath){
    WIN32_FIND_DATA structFD;
    QString path = tempPath;
    HANDLE hnd = FindFirstFile((path+str+"\\*").utf16(),&structFD);
   // std::cout<<(path+str+"\\*").toStdString()<<std::endl;
    if(hnd ==INVALID_HANDLE_VALUE){
        FindClose(hnd);
        CloseHandle(hnd);
        return false;        
    }else{
        FindClose(hnd);
       // CloseHandle(hnd);
        return true;
    }
}

void MyFileMgModel::toFolder(QString name, QString path,QString tempPath){
    numFiles += 1;
    QString type = getType(name);
    if(!folderExists(type,tempPath))
        //CreateDirectory((QString("c:\\myProg\\")+type).utf16(),0);
        CreateDirectory((QString(tempPath)+type).utf16(),0);
    //QString pathLink = QString("link"+QString::number(numFiles));
    QString pathLink = (path+name);
    pathLink.replace("\\",".,");
    pathLink.remove(1,1);
   // CreateFile(QString(QString("c:\\myProg\\")+type+"\\"+pathLink+".lnk").utf16(),GENERIC_READ,0,NULL,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,NULL);
   // CoInitialize(0);
  //  std::cout<<(path+QString::fromWCharArray(name)).toStdString()<< " путь"<<std::endl;
    CreateLink((path+name).utf16(),(LPSTR)QString(tempPath+type+"\\"+pathLink+QString(".lnk")).toLocal8Bit().data(),L"");
        //std::cout<<(path.left(path.length()-1)+QString::fromWCharArray(name)).toStdString()<<"файл"<<std::endl;
    //std::cout<<(path.left(path.length()-1)+QString::fromWCharArray(name)).toStdString()<<std::endl;
  //  std::cout<<("c:\\myProg\\"+type+"\\"+pathLink+QString(".lnk")).toStdString()<<std::endl;

}

void MyFileMgModel::doAllLink(QString path,QString tempPath){
    WIN32_FIND_DATA structFD;
    numFiles = 0;
    HANDLE hFind;
   // QString thisPath = path;
   // CreateLink(L"c:\\Finish.log","c:\\myProg\\rnd\\link1000.lnk",L"");
   // QVector<WIN32_FIND_DATA> vec;// = new QVector<QString>;
   // QString path = "c:\\*";


    hFind = FindFirstFile((path+"*").utf16(),&structFD);
    if(hFind!=0)
        do{
        if((QString::fromWCharArray(structFD.cFileName)!=QString("."))&&(QString::fromWCharArray(structFD.cFileName)!=QString("..")))
        if(structFD.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ){
               // Path.remove(thisPath.indexOf("*",1));
                //thisPath += QString::fromWCharArray(structFD.cFileName)+"\\*";
            doAllLink(path+QString::fromWCharArray(structFD.cFileName)+"\\",tempPath);
            }else{
                toFolder(QString::fromWCharArray(structFD.cFileName),path,tempPath);
            }
        }while(FindNextFile(hFind,&structFD)!=0);
    FindClose(hFind);
    CloseHandle(hFind);
}

bool MyFileMgModel::doSomeFileAction(UINT act,QString from,QString to){
    from += '\0';
    SHFILEOPSTRUCT fos;
    fos.hwnd = 0;
    fos.wFunc = act;
  //  fos.pTo = (path.left(path.length()-1)).utf16();
    fos.pFrom = from.utf16();
    fos.pTo = to.utf16();
   // fos.pFrom += '\0\0';
  //  fos.pFrom = L"c:\\ICQ";
    //fos.pTo = L"c:\\Intel";
  //  std::cout<<"c:\\intel";
    fos.fFlags = FOF_ALLOWUNDO|FOF_NOCONFIRMATION;
    return SHFileOperation(&fos);
}
