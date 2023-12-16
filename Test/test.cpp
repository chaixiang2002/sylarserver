#include <iostream>
#include <string>
#include <string_view>
#include "config.h"
#include "global_util.h"
#include "runtimer_util.h"

using namespace std;

// config()测试
void test_read_file(){
    LOG("CONFIG_PATH=>"<<CONFIG_PATH)
    LOG("RESOURSE_PATH=>"<<RESOURSE_PATH)
    LOG("test_str=>"<<config<string>(CONFIG_PATH, "test", "test_str"));
    LOG("test_int=>"<<config<int>(CONFIG_PATH, "test", "test_int"));
}
int main(){
    std::cout << "Hello, from sylorserver!\n";
    test_read_file();
    return 0;
}
