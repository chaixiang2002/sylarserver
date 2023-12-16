#pragma once

#include <fstream>
#include <memory>
#include <string>
#include <list>
namespace snow {

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();
private:
    const char* m_file=nullptr;     //文件名
    int32_t m_line=0;               //行号
    uint32_t m_elapse=0;            //程序启动到现在多少毫秒数
    uint32_t m_threadId=0;          //线程id
    uint32_t m_fiberId=0;           //协程id
    uint64_t m_time=0;              //时间戳
    std::string m_content;
};

//日志级别
class Loglevel{
public:
    enum Level{// 以后考虑 策略模式
        DEBUG=1,
        INFO=2,
        WARN=3,
        ERROR=4,
        FATAL=5
    };
};

// 日志格式器
class LogFormatter{
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    std::string format(LogEvent::ptr event);
private:
};

// 日志输出地
class LogAppender{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}

    virtual void log(Loglevel::Level level,LogEvent::ptr event)=0;
    void setFormatter(LogFormatter::ptr val){m_formatter=val;}
    LogFormatter::ptr getFormatter()const{ return m_formatter;}
protected:
    Loglevel::Level m_level;
    LogFormatter::ptr m_formatter;
};



// 日志器
class Logger{
public:
    typedef std::shared_ptr<Logger> ptr;
    
    Logger(const std::string& name="root");
    void log(Loglevel::Level level,LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    Loglevel::Level getLevel()const{return m_level;}
    void setLevel(Loglevel::Level val){m_level=val;}
private:
    std::string m_name;                     // 日志名称
    Loglevel::Level m_level;                // 日志级别
    std::list<LogAppender::ptr> m_appenders; // Appender集合
};

// 输出到控制台的LogAppender
class StdoutLogAppender:public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    virtual void log(Loglevel::Level level,LogEvent::ptr event)override;
};

// 输出到文件的LogAppender
class FileLogAppender:public LogAppender{
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    virtual void log(Loglevel::Level level,LogEvent::ptr event)override;

    // 重新打开文件，成功ture
    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};







}