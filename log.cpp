#include "log.h"

namespace sylar
{
    Logger::Logger(const std::string &name) : m_name(name)
    {
    }
    void Logger::addAppender(LogAppender::ptr appender)
    {
        m_appenders.push_back(appender);
    }
    void Logger::delAppender(LogAppender::ptr appender)
    {
        for (auto it = m_appenders.begin(); it != m_appenders.end(); ++it)
        {
            if (*it == appender)
            {
                m_appenders.erase(it);
                break;
            }
        }
    }
    void Logger::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            for (auto &i : m_appenders)
            {
                i->log(level, event);
            }
        }
    }
    void Logger::debug(LogEvent::ptr event)
    {
        log(LogLevel::DEBUG, event);
    }
    void Logger::info(LogEvent::ptr event)
    {
        log(LogLevel::INFO, event);
    }
    void Logger::warn(LogEvent::ptr event)
    {
        log(LogLevel::WARN, event);
    }
    void Logger::error(LogEvent::ptr event)
    {
        log(LogLevel::ERROR, event);
    }
    void Logger::fatal(LogEvent::ptr event)
    {
        log(LogLevel::FATAL, event);
    }

    FileLogAppender::FileLogAppender(const std::string &filename) : m_filename(filename)
    {
    }
    void FileLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            m_filestream << m_formatter->format(event);
        }
    }
    bool FileLogAppender::reopen()
    {
        if (m_filestream)
        {
            m_filestream.close();
        }
        m_filestream.open(m_filename);
        return !!m_filestream;
    }

    void StdoutLogAppender::log(LogLevel::Level level, LogEvent::ptr event)
    {
        if (level >= m_level)
        {
            std::cout << m_formatter->format(event);
        }
    }

    LogFormatter::LogFormatter(const std::string &pattern) : m_pattern(pattern)
    {
    }
    std::string LogFormatter::format(LogEvent::ptr event)
    {
        std::stringstream ss;
        for (auto &i : m_items)
        {
            i->format(ss, event);
        }
        return ss.str();
    }
    void LogFormatter::init()
    {
        // str, format , type
        std::vector<std::tuple<std::string, std::string, int>> vec;
        size_t last_pos = 0;
        std::string nstr;
        for (size_t i = 0; i < m_pattern.size(); ++i)
        {
            if (m_pattern[i] == '%')
            {
                nstr.append(1, m_pattern);
                continue;
            }

            if ((i + 1) < m_pattern.size())
            {
                if (m_pattern[i + 1] == '%')
                {
                    nstr.append(1, '%');
                    continue;
                }
            }

            std::string str;
            std::string fmt;

            size_t n = i + 1;
            int fmt_status = 0;
            size_t fmt_begin = 0;

            while (n < m_pattern.size())
            {
                if (isspace(m_pattern[n]))
                {
                    break;
                }
                if (fmt_status == 0)
                {
                    if (m_pattern[n] == '{')
                    {
                        str = m_pattern.substr(i + 1, n - i - 1);
                        fmt_status = 1; //解析格式
                        ++n;
                        fmt_begin = n;
                        continue;
                    }
                }
                if (fmt_status == 1)
                {
                    if (m_pattern[n] == '}')
                    {
                        fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                        fmt_status = 2;
                        ++n;
                        continue;
                    }
                }
            }

            if (fmt_status == 0)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_pair(nstr, "", 0));
                }
                str = m_pattern.substr(i + 1, n - i - 1);
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
            else if (fmt_status == 1)
            {
                std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
                vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
            }
            else if (fmt_status == 2)
            {
                if (!nstr.empty())
                {
                    vec.push_back(std::make_pair(nstr, "", 0));
                }
                vec.push_back(std::make_tuple(str, fmt, 1));
                i = n;
            }
        }

        if(!nstr.empty()){
            vec.push_back(std::make_pair(nstr,"",0));
        }

        //%m输出代码中的指定信息
        //%p输出优先级（level），即DEBUG,INFO,WARN,ERROR,FATAL
        //%r输出自应用启动到输出该log信息耗费的毫秒数
        //%c输出所属的类目，通常就是所在类的全名
        //%t输出产生该日志实践的线程名
        //%n输出一个回车换行符，Windows平台为"\\r\\n",unix平台为"\\n"
        //%d输出日志时间点的日期或者时间，默认格式为ISO8601
        //%l输出日志时间发生的位置，包括类目名，发生的线程，以及在代码中的行数
        //%f输出文件名


    }

} // namespace sylar
