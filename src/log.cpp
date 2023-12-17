#include "log.h"
#include "global_util.h"
#include <cctype>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace snow {

const char* Loglevel::ToString(Loglevel::Level level){
    switch (level) {
#define XX(name)    \
    case Loglevel::name:    \
        return #name;       \
        break;

    XX(DEBUG)
    XX(INFO)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)
#undef XX
    default:
        return "UNKNOW";
    }
    return "UNKNOW";
}

class MessageFormatItem:public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string &str=""){}
    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << event->getContent();
    }
};

class LevelFormatItem:public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << Loglevel::ToString(level);
    }
};

class ElapseFormatItem:public LogFormatter::FormatItem{
public:
    ElapseFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << event->getElapse();
    }
};

class NameFormatItem:public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << logger->getName();
    }
};

class ThreadIdFormatItem:public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << event->getThreadId();
    }
};

class FiberIdFormatItem:public LogFormatter::FormatItem{
public:
    FiberIdFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << event->getFiberID();
    }
};

class DateTimeFormatItem:public LogFormatter::FormatItem{
public:
    DateTimeFormatItem(const std::string& format= "%Y:%m:%d %H:%M:%S"):m_format(format){

    }
    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << event->getTime();
    }
private:
    std::string m_format;
};

class FilenameFormatItem:public LogFormatter::FormatItem{
public:
    FilenameFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << event->getFile();
    }
};

class LineFormatItem:public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << event->getLine();
    }
};

class NewLineFormatItem:public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string &str=""){}

    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << std::endl;
    }
};

class StringFormatItem:public LogFormatter::FormatItem{
public:

    StringFormatItem(const std::string& str):FormatItem(str),m_string(str){}//what???????
    void format(std::ostream& os,Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event)override{
        os << m_string;
    }
private:
    std::string m_string;
};

LogEvent::LogEvent(const char* file ,int32_t line,uint32_t elapse,uint32_t thread_id,uint32_t fiberId,uint64_t time)
                    :m_file(file),m_line(line),m_elapse(elapse),m_threadId(thread_id),m_fiberId(fiberId),m_time(time){ 

}


Logger::Logger(const std::string& name):m_name(name),m_level(Loglevel::DEBUG){
    m_formatter.reset(new LogFormatter("%d %p %f %l %m %n"));
    // m_formatter.reset(new LogFormatter("%d [%p] %f %l %m %n"));

}
void Logger::addAppender(LogAppender::ptr appender){
    if (!appender->getFormatter()) {// 如果没有formatter，把自己的formatter加进去
        appender->setFormatter(m_formatter);
    }
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
    auto self=shared_from_this();
    if (level>=m_level) {
        for (auto&it : m_appenders) {
            it->log(self,level, event);
        }
    }
}
void Logger::debug(LogEvent::ptr event){
    log(Loglevel::DEBUG,event);
}
void Logger::info(LogEvent::ptr event){
    log(Loglevel::INFO,event);

}
void Logger::warn(LogEvent::ptr event){
    log(Loglevel::WARN,event);

}
void Logger::error(LogEvent::ptr event){
    log(Loglevel::ERROR,event);

}
void Logger::fatal(LogEvent::ptr event){
    log(Loglevel::FATAL,event);

}

FileLogAppender::FileLogAppender(const std::string& filename):m_filename(filename){

}

void FileLogAppender::log(std::shared_ptr<Logger> logger,Loglevel::Level level,LogEvent::ptr event){
    if (level>=m_level) {
        m_filestream<<m_formatter->format(logger,level,event);
    }
}

bool FileLogAppender::reopen(){
    if (m_filestream) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
}
void StdoutLogAppender::log(Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event){
    if (level>=m_level) {
        println(m_formatter->format(logger,level,event));
    }
}

LogFormatter::LogFormatter(const std::string& pattern):m_pattern(pattern){
    init();
}

std::string LogFormatter::format(Logger::ptr logger,Loglevel::Level level,LogEvent::ptr event){
    std::stringstream ss;
    for (auto&it : m_items) {
        it->format(ss,logger, level,event);
    }
    return ss.str();
}

// %xxx %xxx(xxx) %%
void LogFormatter::init(){
    LOG("m_pattern=> "<< m_pattern)
    std::vector<std::tuple<std::string,std::string,int>> vec;
    std::string nstr;
    for (size_t i=0; i<m_pattern.size(); i++) {
        if (m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if ((i+1) < m_pattern.size()) {
            if (m_pattern[i+1] == '%') {
                nstr.append(1, '%');
                i++; // skip the next '%'
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while (n < m_pattern.size()) {
            if (!isalpha(m_pattern[n]) && m_pattern[n] != '{' && m_pattern[n] != '}') {
                break;
            }
            if (fmt_status == 0) {
                if (m_pattern[n] == '{') {
                    str = m_pattern.substr(i+1, n-i-1);
                    fmt_status = 1; // parsing format
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            }
            if (fmt_status == 1) {
                if (m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin+1, n-fmt_begin-1);
                    fmt_status = 2;
                    break;
                }
            }
            ++n;
        }

        if (fmt_status == 0) {
            if (!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
                nstr.clear();
            }
            str = m_pattern.substr(i+1, n-i-1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        } else if (fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        } else if (fmt_status == 2) {
            if (!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, "", 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }
    }
    if (!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)> > s_format_items={
#define XX(str,C) \
        {#str,[](const std::string& fmt){return FormatItem::ptr(new C(fmt)); }}

        XX(m, MessageFormatItem),
        XX(p, LevelFormatItem),
        XX(c, NameFormatItem),
        XX(t, ThreadIdFormatItem),
        XX(n, NameFormatItem),
        XX(d, DateTimeFormatItem),
        XX(f, FilenameFormatItem),
        XX(l, LineFormatItem),
#undef XX
    };

    for(auto &i : vec){
        if (std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if (it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }
        std::cout << "{" << std::get<0>(i) << "} - {" << std::get<1>(i) << "} - {" << std::get<2>(i) << "}" << std::endl;
    }

    //%m -- 消息体
    //%p -- level
    //%r -- 启动后的时间
    //%c -- 日志名称
    //%t -- 线程id
    //%n -- 回车换行
    //%d -- 时间
    //%f -- 文件名
    //%l -- 行号
}




}
