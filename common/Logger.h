/*
    Copyright 2008 Jorge Gorbe Moya <slack@codemaniacs.com>

    This file is part of wenboi

    wenboi is free software: you can redistribute it and/or modify it under the
    terms of the GNU General Public License version 3 only, as published by the
    Free Software Foundation.

    wenboi is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with wenboi.  If not, see <http://www.gnu.org/licenses/>.
*/
#if !defined(LOGGER_H)
#define LOGGER_H

#include "Singleton.h"
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

class Logger : public Singleton<Logger> {
public:
  enum log_level {
    OFF = -1,
    CRITICAL = 0,
    ERROR = 1,
    WARNING = 2,
    INFO = 3,
    DEBUG = 4,
    TRACE = 5
  };

  Logger()
      : out(new std::ofstream("wenboi.log")), current_log_level(WARNING),
        log_start_time(time(NULL)) {
    // log_start_time = time(NULL);
    (*out) << "Starting log at " << static_cast<long>(log_start_time)
           << std::endl;
  }

  ~Logger() { delete out; }

  void log(log_level level, std::string str) {
    if (level <= current_log_level)
      (*out) << "[" << difftime(time(NULL), log_start_time) << "] " << str
             << std::endl;
  }

#define LOGFUNC1(func, tag)                                                    \
  template <typename T1> void func(T1 p1) {                                    \
    std::stringstream str;                                                     \
    str << p1;                                                                 \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC2(func, tag)                                                    \
  template <typename T1, typename T2> void func(T1 p1, T2 p2) {                \
    std::stringstream str;                                                     \
    str << p1 << p2;                                                           \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC3(func, tag)                                                    \
  template <typename T1, typename T2, typename T3>                             \
  void func(T1 p1, T2 p2, T3 p3) {                                             \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3;                                                     \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC4(func, tag)                                                    \
  template <typename T1, typename T2, typename T3, typename T4>                \
  void func(T1 p1, T2 p2, T3 p3, T4 p4) {                                      \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3 << p4;                                               \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC5(func, tag)                                                    \
  template <typename T1, typename T2, typename T3, typename T4, typename T5>   \
  void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5) {                               \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3 << p4 << p5;                                         \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC6(func, tag)                                                    \
  template <typename T1, typename T2, typename T3, typename T4, typename T5,   \
            typename T6>                                                       \
  void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6) {                        \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3 << p4 << p5 << p6;                                   \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC7(func, tag)                                                    \
  template <typename T1, typename T2, typename T3, typename T4, typename T5,   \
            typename T6, typename T7>                                          \
  void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7) {                 \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3 << p4 << p5 << p6 << p7;                             \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC8(func, tag)                                                    \
  template <typename T1, typename T2, typename T3, typename T4, typename T5,   \
            typename T6, typename T7, typename T8>                             \
  void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8) {          \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3 << p4 << p5 << p6 << p7 << p8;                       \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC9(func, tag)                                                    \
  template <typename T1, typename T2, typename T3, typename T4, typename T5,   \
            typename T6, typename T7, typename T8, typename T9>                \
  void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8, T9 p9) {   \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3 << p4 << p5 << p6 << p7 << p8 << p9;                 \
    log(tag, str.str());                                                       \
  }
#define LOGFUNC10(func, tag)                                                   \
  template <typename T1, typename T2, typename T3, typename T4, typename T5,   \
            typename T6, typename T7, typename T8, typename T9, typename T10>  \
  void func(T1 p1, T2 p2, T3 p3, T4 p4, T5 p5, T6 p6, T7 p7, T8 p8, T9 p9,     \
            T10 p10) {                                                         \
    std::stringstream str;                                                     \
    str << p1 << p2 << p3 << p4 << p5 << p6 << p7 << p8 << p9 << p10;          \
    log(tag, str.str());                                                       \
  }

#define LOGFUNC(func, tag)                                                     \
  LOGFUNC1(func, tag)                                                          \
  LOGFUNC2(func, tag)                                                          \
  LOGFUNC3(func, tag)                                                          \
  LOGFUNC4(func, tag)                                                          \
  LOGFUNC5(func, tag)                                                          \
  LOGFUNC6(func, tag)                                                          \
  LOGFUNC7(func, tag)                                                          \
  LOGFUNC8(func, tag)                                                          \
  LOGFUNC9(func, tag)                                                          \
  LOGFUNC10(func, tag)

  LOGFUNC(critical, CRITICAL)
  LOGFUNC(error, ERROR)
  LOGFUNC(warning, WARNING)
  LOGFUNC(info, INFO)
  LOGFUNC(debug, DEBUG)
  LOGFUNC(trace, TRACE)

  void set_log_level(log_level level) { current_log_level = level; }

private:
  std::ostream *out;
  log_level current_log_level;
  time_t log_start_time;
};

#define logger Logger::Instance()

#endif
