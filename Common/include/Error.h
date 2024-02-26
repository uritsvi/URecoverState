#pragma once

#include <iostream>
#include <Windows.h>

#ifdef _DEBUG

/*
#define ERROR_LOG(Msg) std::cout << "\033[0;31m" << __FILE__ << ":" << __LINE__ << "\n" << Msg << "\n" << "\033[0m"
#define LOG(Msg) std::cout << Msg << "\n";
#define WARNING_LOG(Msg) std::cout << "\033[0;33m" << __FILE__ << ":" << __LINE__ << "\n" << Msg << "\n" << "\033[0m"
#define SUCCESS_LOG(Msg) std::cout << "\033[0;32m" << __FILE__ << ":" << __LINE__ << "\n" << Msg << "\n" << "\033[0m"
*/

#define ERROR_LOG(Msg) 
#define LOG(Msg) 
#define WARNING_LOG(Msg) 
#define SUCCESS_LOG(Msg) 
#else

#define ERROR_LOG(Msg)
#define WARNING_LOG(Msg)


#endif

#define EXIT_WITH_ERROR(Msg) ERROR_LOG(Msg); exit(-1);