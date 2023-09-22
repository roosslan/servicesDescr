#include <windows.h>
#include <conio>
#include <iostream>
#include <string>
#include <io.h>
#include <sys/stat.h>
#include <Registry.hpp>
#include "boost\regex.hpp" // namespace boost
#include <boost\regex.h>   // old-style regex
#include <stdio>
#include <stdlib>
#include "IniFiles.hpp"
#include "dienst42.h"
#pragma hdrstop

Svc42 *svc42;

wchar_t* loadString(wchar_t* resName){
  boost::smatch m;
  boost::regex re("%(.*)%");
  wchar_t *Buf, *pathToDLL;
  HINSTANCE hInstance;
  int resNum;

  pathToDLL = svc42->convPathToAbs(resName);
  re = "[0-9]+$";
  regex_search((char*)resName, m, re);

  if(svc42->fileExists(svc42->getFileMUIPath(Svc42::atowc(pathToDLL))))
    pathToDLL = svc42->getFileMUIPath(Svc42::atowc(pathToDLL));

  hInstance = LoadLibraryEx(svc42->wcharToStr(pathToDLL).c_str(), 0, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
  if(NULL != hInstance){
    resNum = atoi(m[0].first);
    LoadString(hInstance, resNum, (char*)Buf, MAX_PATH);
    FreeLibrary(hInstance);
    return Buf;
  };
  return NULL;
}

#pragma argsused
int main(int argc, char* argv[]){
    svc42 = new Svc42();
    AnsiString laufenderSchluessel, svcDescr, derName, descript, svcName;
    TRegistry* regSchluessel = new TRegistry;
    TStringList* subSchluessel = new TStringList;
    TIniFile *ini = new TIniFile("C:\\dienste42.inf");

    int i = 0;

    regSchluessel->RootKey = HKEY_LOCAL_MACHINE;
    regSchluessel->OpenKeyReadOnly("SYSTEM\\CurrentControlSet\\services");
    regSchluessel->GetKeyNames(subSchluessel);
    regSchluessel->CloseKey();
    wcout << "subSchluessel->Strings[i]" << ": " << "loadString(derName)" << " | " << "fileMUI_Descr(descript)" << endl;
    try {
        do {
            laufenderSchluessel = "SYSTEM\\CurrentControlSet\\services\\" + subSchluessel->Strings[i];
            regSchluessel->OpenKeyReadOnly(laufenderSchluessel);
            if(regSchluessel->GetDataType("DisplayName") == rdString && (derName = regSchluessel->ReadString("DisplayName")) != "")
              if(regSchluessel->GetDataType("Description") == rdString && (descript = regSchluessel->ReadString("Description")) != "")
              {
                  if(derName.SubString(1, 1) == "@")
                    svcName = fileMUI_Descr(derName); // loadString(derName);
                  else
                    svcName = derName;

                  if(descript.SubString(1, 1) == "@")
                    svcDescr = fileMUI_Descr(descript);
                  else
                    svcDescr = descript;

                  ini->WriteString(subSchluessel->Strings[i], svcName, svcDescr);
                  ini->WriteString(subSchluessel->Strings[i], "OrigName", derName);
                  ini->WriteString(subSchluessel->Strings[i], "OrigDescript", descript);
              }
            regSchluessel->CloseKey();
        } while (++i < subSchluessel->Count);
    }
    catch (Exception& e) {
        //ShowMessage(i);
    }
    cout << "subSchluessel->SaveToFile(dienste42.inf)";
    subSchluessel->Free();
    regSchluessel->Free();
    delete ini;
    getch();
    return 0;
}
