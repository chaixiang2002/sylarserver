#pragma once

#include <cstdio>
#include <iostream>
#include <ostream>
#include <string>
#include <functional>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "toml.hpp"

// 运行模式----
#define DEBUG

// 打印-------
#ifdef DEBUG
#define LOG(x) std::cout << "LOG: " << x << std::endl;
#else
#define LOG(x)
#endif

inline void println(const std::string& s){
    std::cout  << s << std::endl;
}

template<class T>
T config(const std::string& path,const std::string& table,const std::string& key){
    auto config = toml::parse_file( path );
    T res=config[table][key].value_or(T());
    // LOG(res)
    return res;
}

