/***************************************************
#filename      : log.h
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-29 15:49:08
#last modified : 2023-06-19 15:46:24
#description   : NA
***************************************************/
#ifndef DATA_STRUCT_INCLUDE_LOGGER_H_
#define DATA_STRUCT_INCLUDE_LOGGER_H_

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstdlib>
#include <stdint.h>

namespace ds {

///
/// \brief 日志文件的类型
///
typedef enum LogRank {
   I = 0,
   W,
   E,
   F
} LogRank;

///
/// \brief 初始化日志文件
/// \param info_log_filename 信息文件的名字
/// \param warn_log_filename 警告文件的名字
/// \param error_log_filename 错误文件的名字
void InitLogger(const std::string& info_log_filename,
                const std::string& warn_log_filename,
                const std::string& error_log_filename);

///
/// \brief 日志系统类
///
class Logger {
   friend void InitLogger(const std::string& info_log_filename,
                          const std::string& warn_log_filename,
                          const std::string& erro_log_filename);

public:
         //构造函数
   explicit Logger(LogRank log_rank) : m_log_rank_(log_rank) {};

   ~Logger();
   ///
   /// \brief 写入日志信息之前先写入的源代码文件名, 行号, 函数名
   /// \param log_rank 日志的等级
   /// \param line 日志发生的行号
   /// \param function 日志发生的函数
   static std::ostream& Start(LogRank log_rank,
                              const int32_t line,
                              const std::string &&file,
                              const std::string &&function);

private:
   ///
   /// \brief 根据等级获取相应的日志输出流
   ///
   static std::ostream& GetStream(LogRank log_rank);

   static std::ofstream m_info_log_file;                   ///< 信息日子的输出流
   static std::ofstream m_warn_log_file;                  ///< 警告信息的输出流
   static std::ofstream m_error_log_file;                  ///< 错误信息的输出流
   LogRank m_log_rank_;                             ///< 日志的信息的等级
};


///
/// \brief 根据不同等级进行用不同的输出流进行读写
///
#define LOG(log_rank)   \
Logger(log_rank).Start(log_rank, __LINE__, __FILE__,__FUNCTION__)


// print varible string and it's value
#define PVSTR(stream, val) stream << #val << ": " << val  << "\n "


// #define PrintValue()

///
/// \brief 利用日记进行检查的各种宏
///
#define CHECK(a)                                            \
   if(!(a)) {                                              \
       LOG(ERROR) << " CHECK failed " << endl              \
                   << #a << "= " << (a) << endl;          \
       abort();                                            \
   }                                                      \

#define CHECK_NOTNULL(a)                                    \
   if( NULL == (a)) {                                      \
       LOG(ERROR) << " CHECK_NOTNULL failed "              \
                   << #a << "== NULL " << endl;           \
       abort();                                            \
    }

#define CHECK_NULL(a)                                       \
   if( NULL != (a)) {                                      \
       LOG(ERROR) << " CHECK_NULL failed " << endl         \
                   << #a << "!= NULL " << endl;           \
       abort();                                            \
    }


#define CHECK_EQ(a, b)                                      \
   if(!((a) == (b))) {                                     \
       LOG(ERROR) << " CHECK_EQ failed "  << endl          \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_NE(a, b)                                      \
   if(!((a) != (b))) {                                     \
       LOG(ERROR) << " CHECK_NE failed " << endl           \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_LT(a, b)                                      \
   if(!((a) < (b))) {                                      \
       LOG(ERROR) << " CHECK_LT failed "                   \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_GT(a, b)                                      \
   if(!((a) > (b))) {                                      \
       LOG(ERROR) << " CHECK_GT failed "  << endl          \
                  << #a <<" = " << (a) << endl            \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_LE(a, b)                                      \
   if(!((a) <= (b))) {                                     \
       LOG(ERROR) << " CHECK_LE failed "  << endl          \
                   << #a << "= " << (a) << endl           \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_GE(a, b)                                      \
   if(!((a) >= (b))) {                                     \
       LOG(ERROR) << " CHECK_GE failed "  << endl          \
                   << #a << " = "<< (a) << endl            \
                   << #b << "= " << (b) << endl;          \
       abort();                                            \
    }

#define CHECK_DOUBLE_EQ(a, b)                               \
   do {                                                    \
       CHECK_LE((a), (b)+0.000000000000001L);              \
       CHECK_GE((a), (b)-0.000000000000001L);              \
    }while (0)

}  // namespace nnte

#endif // DATA_STRUCT_INCLUDE_LOGGER_H_

