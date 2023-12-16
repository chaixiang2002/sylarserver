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

#include "config.h"




template<class T>
T read_config(const std::string& table,const std::string& key){
    auto config = toml::parse_file( "/home/snow/code/netserver/config/config.toml" );
    T res=config[table][key].value_or(T());
    LOG(res)
    return res;
}

