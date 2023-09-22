#include <io>
#include <string>
#include <wchar>
#include <stdio>
#include <iostream>
#include <comdef>
#include "dienst42.h"
#include "boost\regex.hpp" // namespace boost
#include <boost\regex.h>   // old-style regex
#include <wchar.h>
using namespace std;
#pragma hdrstop
#pragma package(smart_init)


__stdcall Svc42::Svc42(){
  //-----------------------------------------------------
}

static wchar_t* Svc42::atowc(AnsiString a)
{
  int sz = a.WideCharBufSize();
  wchar_t *res = new wchar_t[sz];
  a.WideChar(res, sz);
  return res;
}

wchar_t* __stdcall Svc42::charToW(const char * const arr)
{
  const size_t cSize = strlen(arr)+1;
  wchar_t* mSec = new wchar_t[cSize];
  mbstowcs(mSec, arr, cSize);
  return mSec;
}

string __stdcall Svc42::wcharToStr(const wchar_t *arr)
{
  wstring ws(arr);
  string res(ws.begin(), ws.end());
  return res;
}

wchar_t* __stdcall Svc42::wsubstr(const wchar_t* arr, int begin, int len)
{
    wchar_t* res = new wchar_t[len + 1];
    for (int i = 0; i < len; i++)
        res[i] = *(arr + begin + i);
    res[len] = 0;
    return res;
}

bool __stdcall Svc42::fileExists(const AnsiString &name){
      wchar_t* serf;
  return (access(name.c_str(), 0 ) != -1);
}

wchar_t* __stdcall Svc42::getFileMUIPath(wchar_t path[_MAX_PATH]){
  wchar_t drive[_MAX_DRIVE], fname[_MAX_FNAME], res[_MAX_PATH],
    wpath[_MAX_PATH], dir[_MAX_DIR], ext[_MAX_EXT], langmui[7] = L"de-DE\\";

  _wsplitpath(path, drive, dir, fname, ext);
  swprintf(res, L"%s%s%s%s%s.mui", drive, dir, langmui, fname, ext);

  return res;
}

const wchar_t* __stdcall Svc42::re_auswahl(const wchar_t *Text, const char *pattern){
    regex_t kanpiljat;
    int rc;
    size_t nmatch = 2;
    regmatch_t ptr_match[3];

    if(0 != (rc = regcomp(&kanpiljat, pattern, REG_EXTENDED))) {
        // "regcomp() failed, returning nonzero (%d)\n", rc
        exit(EXIT_FAILURE);
    }

    if(0 != (rc = regexec(&kanpiljat, wcharToStr(Text).c_str(), nmatch, ptr_match, 0))){
        // "Failed to match '%s' with '%s',returning %d.\n", text, pattern, rc
    }
    else {
      wchar_t* RASA = wsubstr(Text, ptr_match[1].rm_so, ptr_match[1].rm_eo - ptr_match[1].rm_so);
//      regfree(&kanpiljat);
//    return wstring(Text).substr(ptr_match[1].rm_so, ptr_match[1].rm_eo - ptr_match[1].rm_so).c_str();
      return wsubstr(Text, ptr_match[1].rm_so, ptr_match[1].rm_eo - ptr_match[1].rm_so);
    }
    regfree(&kanpiljat);
    return NULL;
}

wchar_t* __stdcall Svc42::getEnvVar(const wchar_t* envVar){
    long buffSize;
    wchar_t buffer[65535];
    buffSize = sizeof(buffer);
    GetEnvironmentVariableW(envVar, &buffer[0], buffSize);
    return buffer; // the same as function _wgetenv("SystemRoot");
}

wchar_t *__stdcall Svc42::convPathToAbs(wchar_t *Path){
  boost::smatch m;
  boost::regex re("%(.*)%");
  wchar_t *envVarW, *hardPath;


  if(0 != wcscmp(L"%", wsubstr(Path, 1, 1))){
    // in case of @appmgmts.dll,-3250
    Path++; // trims left - removes the first character of an array = Path.erase(0, 1);
    hardPath = L"%SystemRoot%\\System32\\";
    wcscat(hardPath, Path);
    Path = hardPath;
  }

  boost::regex_search(wcharToStr(Path), m, re);
  wchar_t *mat = charToW(m[0].second); // boost 1.39 memory leak

  const wchar_t *re_res = re_auswahl(Path, "%(.*)%");
  envVarW = getEnvVar(re_res);

  const wchar_t *relPath = re_auswahl(mat, "(.*),-[0-9]+$"); // mat[0].second  \system32\aelupsvc.dll,-1
  wcscat(envVarW, relPath);

  delete mat; // delete mSec in charToW
  return envVarW;
}

static wchar_t* __stdcall Svc42::loadString(wchar_t* resName){
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

static AnsiString __stdcall Svc42::fileMUI_Descr(AnsiString cacheZeile){
  TRegistry* regSchluessel = new TRegistry;
  AnsiString result;

  regSchluessel->RootKey = HKEY_CLASSES_ROOT;
  regSchluessel->OpenKeyReadOnly("Local Settings\\MuiCache\\BA5\\46693477");
  if(regSchluessel->ValueExists(cacheZeile))
   result = regSchluessel->ReadString(cacheZeile);
  else
   result = loadString(atowc(cacheZeile));

  regSchluessel->CloseKey();
  return result;
}

