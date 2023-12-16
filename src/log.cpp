#include "log.h"
#include <iostream>

namespace snow {
Logger::Logger(const std::string& name):m_name(name){

}
void Logger::addAppender(LogAppender::ptr appender){
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender){
    for (auto it=m_appenders.begin(); it!=m_appenders.end(); it++) {
        if (*it==appender) {
            m_appenders.erase(it);
            break;
        }
    }
    /**
        list是双向链表，所以可以在迭代器中删除，如果是数组的容器不建议这么使用，可能使迭代器失效
    */
}
void Logger::log(Loglevel::Level level,LogEvent::ptr event){
    if (level>=m_level) {
        for (auto&it : m_appenders) {
            it->log(level, event);
        }
    }
}
void Logger::debug(LogEvent::ptr event){
    debug(Loglevel::DEBUG,event);
}
void Logger::info(LogEvent::ptr event){
    debug(Loglevel::INFO,event);

}
void Logger::warn(LogEvent::ptr event){
    debug(Loglevel::WARN,event);

}
void Logger::error(LogEvent::ptr event){
    debug(Loglevel::ERROR,event);

}
void Logger::fatal(LogEvent::ptr event){
    debug(Loglevel::FATAL,event);

}

FileLogAppender::FileLogAppender(const std::string& filename):m_filename(filename){

}

void FileLogAppender::log(Loglevel::Level level,LogEvent::ptr event){
    if (level>=m_level) {
        m_filestream<<m_formatter->format(event);
    }
}

bool FileLogAppender::reopen(){
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}
void StdoutLogAppender::log(Loglevel::Level level,LogEvent::ptr event){
    if (level>=m_level) {
        std::cout <<m_formatter->format(event);
    }
}





}
