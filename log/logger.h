#ifndef LOGGER_H
#define LOGGER_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <mutex>
#include <queue>
#include "fileManagement.h"

#ifdef __linux__
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#elif _WIN32
#include <direct.h>
#include <process.h>
#include <Windows.h>
#endif

namespace cppServer
{

#define Error1 __LOGNAME__(Error)
#define Warn1 __LOGNAME__(Warn)
#define Info1 __LOGNAME__(Info)
#define Debug1 __LOGNAME__(Debug)
#define Trace1 __LOGNAME__(Trace)

#define __LOGTIME__ Logger::getInstance()->getLogCoutTime()      //时间宏
#define __LOGPID__ Logger::getInstance()->getLogCoutProcessId()  //进程宏
#define __LOGTID__ Logger::getInstance()->getLogCoutThreadId()   //线程宏
#define __USERNAME__ Logger::getInstance()->getLogCoutUserName() //获取调用用户名字
#define __LOGFILE__ __FILE__                                     //文件名宏
#define __LOGFUNC__ __func__                                     //函数名宏
#define __LOGLINE__ __LINE__                                     //行数宏
#define __LOGNAME__(name) #name                                  //名字宏

    const int MEGABYTES = 1048576;
    const std::string SQUARE_BRACKETS_LEFT = " [";
    const std::string SQUARE_BRACKETS_RIGHT = "] ";
    const std::string SPACE = " ";
    const std::string LINE_FEED = "\n";
    const std::string COLON = ":";
    const std::string SLASH = "/";
    const std::string DEFA = "\e[0m";
    const std::string SWITCH_OFF = "off";
    const std::string SWITCH_ON = "on";
    const std::string RED = "\e[1;31m";
    const std::string BLUE = "\e[1;34m";
    const std::string GREEN = "\e[1;32m";
    const std::string WHITE = "\e[1;37m";
    const std::string PURPLE = "\e[1;35m";

    enum class coutType : int
    {
        Error,
        Warn,
        Info,
        Debug,
        Trace
    };
    enum class fileType : int
    {
        Error,
        Warn,
        Info,
        Debug,
        Trace
    };
    enum class terminalType : int
    {
        Error,
        Warn,
        Info,
        Debug,
        Trace
    };

    struct Settings
    {
        std::string logSwitch;              //日志开关
        std::string logFileSwitch;          //是否写入文件
        std::string logTerminalSwitch;      //是否打印到终端
        std::string logFileQueueSwitch;     //是否开启队列策略
        std::string logName;                //日志文件名字
        std::string logFilePath;            //日志文件保存路径
        std::string logMixSize;             //日志文件最大大小
        std::string logBehavior;            //日志文件达到最大大小行为
        std::string logOutputLevelFile;     //日志输出等级(file)
        std::string logOutputLevelTerminal; //日志输出等级
    };

    class Logger
    {
    private:
        char sourceFilePath[128];
        Settings settings;
        FileManagement filemanagement;
        static Logger *singleObject;
        static std::mutex *mutex_log;
        static std::mutex *mutex_file;
        static std::mutex *mutex_queue;
        std::map<coutType, std::string> coutTypeMap;
        std::map<fileType, bool> fileCoutMap;
        std::map<terminalType, bool> terminalCoutMap;
        std::map<std::string, std::string> coutColor;
        std::queue<std::string> messageQueue;

    private:
        Logger();
        ~Logger();

    public:
        static std::mutex *mutex_terminal;

        void initLogConfig();

        void releaseConfig();

        void ConfInfoPrint();

        static Logger *getInstance();

        std::string getLogCoutTime();

        std::string getLogNameTime();

        std::string getSourceFilePash();

        std::string getFilePath();

        std::string getFilePathAndName();

        std::string getFilePathAndNameAndTime();

        std::string getLogCoutProcessId();

        std::string getLogCoutThreadId();

        std::string getLogCoutUserName();

        std::string getLogSwitch();

        std::string getLogFileSwitch();

        std::string getLogTerminalSwitch();

        std::string getCoutType(coutType coutType);

        std::string getCoutTypeColor(std::string colorType);

        bool getFileType(fileType fileCoutBool);

        bool getTerminalType(terminalType terminalCoutTyle);

        bool logFileWrite(std::string messages, std::string message, std::string LINE);

        bool insertQueue(std::string messages, std::string filePashAndName);

        bool bindFileCoutMap(std::string value1, fileType value2);

        bool bindTerminalCoutMap(std::string value1, terminalType value2);
    };

#define KV(value) " " << #value << "=" << value

#define COMBINATION_INFO_FILE(coutTypeInfo, message)                                                                               \
    do                                                                                                                             \
    {                                                                                                                              \
        std::ostringstream oss;                                                                                                    \
        std::streambuf *pOldBuf = std::cout.rdbuf(oss.rdbuf());                                                                    \
        std::cout << message;                                                                                                      \
        std::string ret = oss.str();                                                                                               \
        std::cout.rdbuf(pOldBuf);                                                                                                  \
        std::string messagesAll = __LOGTIME__ + coutTypeInfo + __USERNAME__ + __LOGTID__ + SQUARE_BRACKETS_LEFT +                  \
                                  __LOGFILE__ + SPACE + __LOGFUNC__ + COLON + std::to_string(__LOGLINE__) + SQUARE_BRACKETS_RIGHT; \
        Logger::getInstance()->logFileWrite(messagesAll, ret, LINE_FEED);                                                          \
    } while (0);

#ifdef __linux__
#define COMBINATION_INFO_TERMINAL(coutTypeInfo, message)                                                                           \
    do                                                                                                                             \
    {                                                                                                                              \
        std::string color = Logger::getInstance()->getCoutTypeColor(coutTypeInfo);                                                 \
        std::string logFormatCout = __LOGTIME__ + color + coutTypeInfo + DEFA + __USERNAME__ + __LOGTID__ + SQUARE_BRACKETS_LEFT + \
                                    __LOGFILE__ + COLON + std::to_string(__LOGLINE__) + SQUARE_BRACKETS_RIGHT;                     \
        Logger::mutex_terminal->lock();                                                                                            \
        std::cout << logFormatCout << message << LINE_FEED;                                                                        \
        fflush(stdout);                                                                                                            \
        Logger::mutex_terminal->unlock();                                                                                          \
    } while (0);
#elif _WIN32
#define COMBINATION_INFO_TERMINAL(coutTypeInfo, message)                                                                             \
    do                                                                                                                               \
    {                                                                                                                                \
        std::string logFormatCout = __LOGTIME__ + coutTypeInfo + __USERNAME__ + __LOGTID__ + SQUARE_BRACKETS_LEFT +                  \
                                    __LOGFILE__ + SPACE + __LOGFUNC__ + COLON + std::to_string(__LOGLINE__) + SQUARE_BRACKETS_RIGHT; \
        Logger::mutex_terminal->lock();                                                                                              \
        std::cout << logFormatCout << message << LINE_FEED;                                                                          \
        fflush(stdout);                                                                                                              \
        Logger::mutex_terminal->unlock();                                                                                            \
    } while (0);
#endif

#define LoggerCout(coutTyle, coutTypeInfo, fileCoutBool, terminalCoutBool, message) \
    do                                                                              \
    {                                                                               \
        std::string coutType = Logger::getInstance()->getCoutType(coutTyle);        \
        if (SWITCH_ON == Logger::getInstance()->getLogSwitch())                     \
        {                                                                           \
            if (SWITCH_OFF != Logger::getInstance()->getLogFileSwitch())            \
            {                                                                       \
                if (Logger::getInstance()->getFileType(fileCoutBool))               \
                {                                                                   \
                    COMBINATION_INFO_FILE(coutTypeInfo, message)                    \
                }                                                                   \
            }                                                                       \
            if (SWITCH_OFF != Logger::getInstance()->getLogTerminalSwitch())        \
            {                                                                       \
                if (Logger::getInstance()->getTerminalType(terminalCoutBool))       \
                {                                                                   \
                    COMBINATION_INFO_TERMINAL(coutTypeInfo, message)                \
                }                                                                   \
            }                                                                       \
        }                                                                           \
    } while (0);

#define LogError(...)                                                                                                           \
    do                                                                                                                          \
    {                                                                                                                           \
        LoggerCout(cppServer::coutType::Error, Error1, cppServer::fileType::Error, cppServer::terminalType::Error, __VA_ARGS__) \
    } while (0);

#define LogWarn(...)                                                                                                        \
    do                                                                                                                      \
    {                                                                                                                       \
        LoggerCout(cppServer::coutType::Warn, Warn1, cppServer::fileType::Warn, cppServer::terminalType::Warn, __VA_ARGS__) \
    } while (0);

#define LogInfo(...)                                                                                                        \
    do                                                                                                                      \
    {                                                                                                                       \
        LoggerCout(cppServer::coutType::Info, Info1, cppServer::fileType::Info, cppServer::terminalType::Info, __VA_ARGS__) \
    } while (0);

#define LogDebug(...)                                                                                                           \
    do                                                                                                                          \
    {                                                                                                                           \
        LoggerCout(cppServer::coutType::Debug, Debug1, cppServer::fileType::Debug, cppServer::terminalType::Debug, __VA_ARGS__) \
    } while (0);

#define LogTrace(...)                                                                                                           \
    do                                                                                                                          \
    {                                                                                                                           \
        LoggerCout(cppServer::coutType::Trace, Trace1, cppServer::fileType::Trace, cppServer::terminalType::Trace, __VA_ARGS__) \
    } while (0);
}

#endif
