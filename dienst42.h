#include <vcl.h>
#include "boost\regex.hpp"

#ifndef dienst42H
#define dienst42H

#define LOAD_LIBRARY_AS_IMAGE_RESOURCE 0x00000020

class Svc42
{
public:
   __stdcall Svc42();
   static wchar_t* atowc(AnsiString a);
   wchar_t* __stdcall charToW(const char * const arr);
   wchar_t* __stdcall Svc42::getEnvVar(const wchar_t* envVar);
   string __stdcall wcharToStr(const wchar_t *arr);
   wchar_t* __stdcall wsubstr(const wchar_t* arr, int begin, int len);
   bool __stdcall fileExists(const AnsiString &name);
   wchar_t * __stdcall Svc42::getFileMUIPath(wchar_t path[_MAX_PATH]);
   const wchar_t* __stdcall Svc42::re_auswahl(const wchar_t *Text, const char *pattern);
   wchar_t* __stdcall Svc42::convPathToAbs(wchar_t *Path);
   static wchar_t* __stdcall Svc42::loadString(wchar_t* resName);
   static AnsiString __stdcall Svc42::fileMUI_Descr(AnsiString cacheZeile);
};

#endif
