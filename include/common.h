/***************************************************
#filename      : common.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-06-19 15:50:45
#last modified : 2023-06-19 15:50:52
#description   : NA
***************************************************/
#ifndef DT_COMMON_HPP_
#define DT_COMMON_HPP_

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <iterator>

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

#define LOG_INFO                  "[info:]"
#define LOG_DEBUG                 "[debug:]"
#define LOG_WARNNING              "[warning:]"
#define LOG_ERROR                 "[error:]"
#define LOG_FATAL                 "[fatal:]"

// #ifdef LOG
  // fprintf(stdout, GREENCOLOR, LOG_INFO, COLOREND, ##__VA_ARGS__);
#define LOGI(...) do { \
  flockfile (stdout); \
  fprintf(stdout, ##__VA_ARGS__); \
  funlockfile(stdout); \
} while(0)

  // fprintf(stdout, REDCOLOR, LOG_ERROR, COLOREND, ##__VA_ARGS__);
#define LOGE(...) do { \
  flockfile(stdout); \
  fprintf(stdout, ##__VA_ARGS__); \
  funlockfile(stdout); \
} while(0)



// get it from internet
/*
#ifdef NOLOG
#define LOG(...)
#define LogInfo(...)
#define LogError(...)
#define LogWarning(...)
#define LogDebug(...)
#else

#define NONECOLOR                 "\e[0m"
#define BLACKCOLOR                "\e[0;30m"
#define L_BLACKCOLOR              "\e[1;30m"
#define REDCOLOR                  "\e[0;31m"
#define L_REDCOLOR                "\e[1;31m"
#define GREENCOLOR                "\e[0;32m"
#define L_GREENCOLOR              "\e[1;32m"
#define BROWNCOLOR                "\e[0;33m"
#define YELLOWCOLOR               "\e[1;33m"
#define BLUECOLOR                 "\e[0;34m"
#define L_BLUECOLOR               "\e[1;34m"
#define PURPLECOLOR               "\e[0;35m"
#define L_PURPLECOLOR             "\e[1;35m"
#define CYANCOLOR                 "\e[0;36m"
#define L_CYANCOLOR               "\e[1;36m"
#define GRAYCOLOR                 "\e[0;37m"
#define WHITECOLOR                "\e[1;37m"

#define BOLDCOLOR                 "\e[1m"
#define UNDERLINECOLOR            "\e[4m"
#define BLINKCOLOR                "\e[5m"
#define REVERSECOLOR              "\e[7m"
#define HIDECOLOR                 "\e[8m"
#define CLEARCOLOR                "\e[2J"
#define CLRLINECOLOR              "\r\e[K" //or "\e[1K\r"

#ifdef NOCOLOR
#define ERRORSTART "Error:"
#define ERROREND 
#define WARNINGSTART "Warning:"
#define WARNINGEND
#else
#define ERRORSTART REDCOLOR "Error:" BROWNCOLOR
#define ERROREND NONECOLOR
#define WARNINGSTART PURPLECOLOR "Warning:" BLUECOLOR
#define WARNINGEND NONECOLOR
#endif
#define INFOSTART "Info:"
#define INFOEND 
#define DEBUGSTART "Debug:"
#define DEBUGEND 

void consolelogger_log(const char* format, ...);

#ifdef NOTIME
#define TIMEARG(_ptm)
#define TIMEFORMAT
#else
#define TIMEARG(_ptm) ((_ptm)->tm_year + 1900), ((_ptm)->tm_mon + 1), (_ptm)->tm_mday, (_ptm)->tm_hour, (_ptm)->tm_min, (_ptm)->tm_sec
#define TIMEFORMAT "%d-%d-%d %d:%d:%d "
#endif

#ifdef NOLOCALTION
#define LOCALTIONARG
#define LOCALTION
#else
#define LOCALTIONARG(_modulename) _modulename,__FILE__, __FUNCTION__, __LINE__
#define LOCALTION " %s %s %s():%d "
#endif

#define LOG(format,...) { consolelogger_log(format,##__VA_ARGS__); }

#ifdef NOLOGINFO
#define LogInfo(...)
#else
#define LogInfo(modulename,format,...) do{   \
    time_t now = time(NULL);         \
    struct tm * localnow = localtime(&now);                      \
    LOG(TIMEFORMAT INFOSTART LOCALTION format INFOEND,TIMEARG(localnow),LOCALTIONARG(modulename),##__VA_ARGS__);\
    }while(false)
#endif

#ifdef NOLOGERROR
#define LogError(...)
#else
#define LogError(modulename,format,...) do{   \
    time_t now = time(NULL);         \
    struct tm * localnow = localtime(&now);                      \
 LOG(TIMEFORMAT ERRORSTART LOCALTION format ERROREND,TIMEARG(localnow),LOCALTIONARG(modulename),##__VA_ARGS__);\
    }while(false)
#endif

#ifdef NOLOGWARNING
#define LogWarning(...)
#else
#define LogWarning(modulename,format,...) do{   \
    time_t now = time(NULL);            \
    struct tm * localnow = localtime(&now);                      \
    LOG(TIMEFORMAT WARNINGSTART LOCALTION format WARNINGEND,TIMEARG(localnow),LOCALTIONARG(modulename),##__VA_ARGS__);\
    }while(false)
#endif

#ifdef NOLOGDEBUG
#define LogDebug(...)
#else
#define LogDebug(modulename,format,...) do{   \
    time_t now = time(NULL);            \
    struct tm * localnow = localtime(&now);                      \
    LOG(TIMEFORMAT DEBUGSTART LOCALTION format DEBUGEND,TIMEARG(localnow),LOCALTIONARG(modulename),##__VA_ARGS__);\
    }while(false)
#endif

void consolelogger_log(const char* format, ...) {
	va_list args;
	va_start(args, format);
	(void)vprintf(format, args);
	va_end(args);
	(void)printf("\n");
}
#endif
*/

#endif // DT_COMMON_HPP_
