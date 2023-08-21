/***************************************************
#filename      : logger.cpp
#author        : litao
#e-mail        : Tao.Li@streamcomputing.com
#create time   : 2023-05-29 15:52:48
#last modified : 2023-06-19 18:00:47
#description   : NA
***************************************************/

#include <cstdlib>
#include <ctime>
#include "./include/logger.h"

namespace ds {

std::ofstream Logger::m_error_log_file;
std::ofstream Logger::m_info_log_file;
std::ofstream Logger::m_warn_log_file;

void InitLogger(const std::string&info_log_filename,
                const std::string&warn_log_filename,
                const std::string&error_log_filename){
   Logger::m_info_log_file.open(info_log_filename.c_str());
   Logger::m_warn_log_file.open(warn_log_filename.c_str());
   Logger::m_error_log_file.open(error_log_filename.c_str());
}

std::ostream& Logger::GetStream(LogRank log_rank){
   return (LogRank::I == log_rank) ?
                (m_info_log_file.is_open() ? m_info_log_file : std::cout) :
                (LogRank::W == log_rank ?
                    (m_warn_log_file.is_open()? m_warn_log_file : std::cerr) :
                    (m_error_log_file.is_open()? m_error_log_file : std::cerr));
}

std::ostream& Logger::Start(LogRank log_rank,
                            const int32_t line,
                            const std::string &&file,
                            const std::string &&function) {
  time_t tm;
  time(&tm);
  std::string time_string = ctime(&tm);
  auto const pos = file.find_last_of('/');
  const auto leaf = file.substr(pos+1);
  return GetStream(log_rank) << time_string.erase('\n')
                             << "file: "
                             << leaf << " :"
                             << line << " "
                             << function << "(): ";
}

Logger::~Logger(){
  GetStream(m_log_rank_) << std::endl << std::flush;

  if (LogRank::F == m_log_rank_) {
      m_info_log_file.close();
      m_warn_log_file.close();
      m_error_log_file.close();
      abort();
   }
}

}  // namespace nnte
