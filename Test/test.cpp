#include <ctime>
#include <iostream>
#include <string>
#include <string_view>
#include "config.h"
#include "global_util.h"
#include "runtimer_util.h"

#include "log.h"

using namespace std;

// config()测试
void test_read_file(){
    LOG("CONFIG_PATH=>"<<CONFIG_PATH)
    LOG("RESOURSE_PATH=>"<<RESOURSE_PATH)
    LOG("test_str=>"<<config<string>(CONFIG_PATH, "test", "test_str"));
    LOG("test_int=>"<<config<int>(CONFIG_PATH, "test", "test_int"));
}


// log测试
void test_log(){
    LOG("begin---")
    snow::Logger::ptr logger(new snow::Logger);
    logger->addAppender(snow::LogAppender::ptr (new snow::StdoutLogAppender));
    snow::LogEvent::ptr event(new snow::LogEvent(__FILE__,__LINE__,0,1,2,time(0)));
    logger->log(snow::Loglevel::DEBUG, event);
    LOG("end-----")
}
int main(){
    // std::cout << "Hello, from sylorserver!\n";
    // test_read_file();
    test_log();
    return 0;
}
