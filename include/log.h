#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>
#include <array>

#if defined(__GNUC__) || defined(__clang__)
  #define FUNC_SIG __PRETTY_FUNCTION__
#else
  #define FUNC_SIG __FUNCTION__
#endif

#define MYLOG_INFO(MSG)   std::cout << class_name <<"::"<<__func__<<" [INFO]: "<<MSG<<std::endl;
#define MYLOG_DEBUG(MSG)  std::cout << class_name <<"::"<<__func__<<" [DEBUG]: "<<MSG<<std::endl;
#define MYLOG_ERROR(MSG)  std::cerr << class_name <<"::"<<__func__<<" [Error]: "<<MSG<<std::endl;

static constexpr std::array<std::array<int, 4>, 3> arg_3coin = {{
    {{1, 2, 4, 5}},
    {{0, 1, 3, 4}},
    {{0, 2, 3, 5}}
}};


#endif 
