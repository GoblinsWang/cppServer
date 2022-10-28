#ifdef _WIN32
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "../logger.h"

using namespace cpplog;

Logger *Logger::singleObject = nullptr;
mutex *Logger::mutex_log = new (mutex);
mutex *Logger::mutex_file = new (mutex);
mutex *Logger::mutex_queue = new (mutex);
mutex *Logger::mutex_terminal = new (mutex);

Logger::Logger()
{
    initLogConfig();
}

Logger::~Logger()
{
}

Logger *Logger::getInstance()
{
    mutex_log->lock();
    if (singleObject == nullptr)
    {
        singleObject = new Logger();
    }
    mutex_log->unlock();
    return singleObject;
}

void Logger::initLogConfig()
{
#ifdef __linux__
    coutColor["Error"] = "\e[1;31m";
    coutColor["Warn"] = "\e[1;35m";
    coutColor["Info"] = "\e[1;34m";
    coutColor["Debug"] = "\e[1;32m";
    coutColor["Trace"] = "\e[1;37m";
#elif _WIN32
    coutColor["Error"] = "";
    coutColor["Warn"] = "";
    coutColor["Info"] = "";
    coutColor["Debug"] = "";
    coutColor["Trace"] = "";
#endif

    map<string, string *> flogConfInfo;
    flogConfInfo["logSwitch"] = &this->settings.logSwitch;
    flogConfInfo["logFileSwitch"] = &this->settings.logFileSwitch;
    flogConfInfo["logTerminalSwitch"] = &this->settings.logTerminalSwitch;
    flogConfInfo["logFileQueueSwitch"] = &this->settings.logFileQueueSwitch;
    flogConfInfo["logName"] = &this->settings.logName;
    flogConfInfo["logFilePath"] = &this->settings.logFilePath;
    flogConfInfo["logMixSize"] = &this->settings.logMixSize;
    flogConfInfo["logBehavior"] = &this->settings.logBehavior;
    flogConfInfo["logOutputLevelFile"] = &this->settings.logOutputLevelFile;
    flogConfInfo["logOutputLevelTerminal"] = &this->settings.logOutputLevelTerminal;

    bool isOpen = true;
    string str;
    ifstream file;
    char str_c[100] = {0};
#ifdef __linux__
    file.open("./log.conf");
#elif _WIN32
    file.open("../log.conf");
#endif
    if (!file.is_open())
    {
        isOpen = false;
        cout << "File open failed" << endl;
    }
    while (getline(file, str))
    {
        if (!str.length())
        {
            continue;
        }
        string str_copy = str;
        int j = 0;
        for (unsigned int i = 0; i < str.length(); i++)
        {
            if (str[i] == ' ')
                continue;
            str_copy[j] = str[i];
            j++;
        }
        str_copy.erase(j);
        if (str_copy[0] != '#')
        {
            sscanf(str_copy.data(), "%[^=]", str_c);
            auto iter = flogConfInfo.find(str_c);
            if (iter != flogConfInfo.end())
            {
                sscanf(str_copy.data(), "%*[^=]=%s", str_c);
                *iter->second = str_c;
            }
            else
            {
            }
        }
    }
    file.close();

    bindFileCoutMap("5", fileType::Error);
    bindFileCoutMap("4", fileType::Warn);
    bindFileCoutMap("3", fileType::Info);
    bindFileCoutMap("2", fileType::Debug);
    bindFileCoutMap("1", fileType::Trace);

    bindTerminalCoutMap("5", terminalType::Error);
    bindTerminalCoutMap("4", terminalType::Warn);
    bindTerminalCoutMap("3", terminalType::Info);
    bindTerminalCoutMap("2", terminalType::Debug);
    bindTerminalCoutMap("1", terminalType::Trace);

    string filePathAndName = getFilePathAndName();
    string filePath = getFilePath();
    cout << filePathAndName << " : " << filePath << endl;
    if (settings.logFileSwitch == SWITCH_ON)
    {
        //检查路径
        filemanagement.createFilePath(filePath);
        //检测文件有效性
        if (!filemanagement.verifyFileExistence(filePathAndName))
        {
            filemanagement.createFile(filePathAndName);
        }
        else
        {
            long fileSize = filemanagement.verifyFileSize(filePathAndName);
            if (fileSize > (long)atoi(settings.logMixSize.data()) * MEGABYTES && settings.logBehavior == "1")
            {
                string newFileName = getFilePathAndNameAndTime();
                filemanagement.fileRename(filePathAndName, newFileName);
                filemanagement.createFile(filePathAndName);
            }
        }
    }
    if (isOpen)
    {
        ConfInfoPrint();
    }
    return;
}

void Logger::releaseConfig()
{
}

void Logger::ConfInfoPrint()
{
#ifdef __linux__
    for (unsigned int i = 0; i < settings.logFilePath.size() + 15; i++)
    {
        cout << GREEN << "-";
        if (i == (settings.logFilePath.size() + 15) / 2)
        {
            cout << "Logger";
        }
    }
    cout << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志开关　　" << settings.logSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  文件输出　　" << settings.logFileSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  终端输出开关" << settings.logTerminalSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  文件输出等级" << settings.logOutputLevelFile << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  终端输出等级" << settings.logOutputLevelTerminal << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志队列策略" << settings.logFileQueueSwitch << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志文件名称" << settings.logName << ".log" << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志保存路径" << settings.logFilePath << DEFA << endl;
    cout << GREEN << ::left << setw(25) << "  日志文件大小" << settings.logMixSize << "M" << DEFA << endl;
    for (unsigned int i = 0; i < settings.logFilePath.size() + 25; i++)
    {
        cout << GREEN << "-" << DEFA;
    }
    cout << endl;
#elif _WIN32
    for (unsigned int i = 0; i < settings.logFilePath.size() + 15; i++)
    {
        cout << "-";
        if (i == (settings.logFilePath.size() + 15) / 2)
        {
            cout << "Logger";
        }
    }
    cout << endl;
    cout << ::left << setw(25) << "  日志开关　　" << logger.logSwitch << endl;
    cout << ::left << setw(25) << "  文件输出　　" << logger.logFileSwitch << endl;
    cout << ::left << setw(25) << "  终端输出开关" << logger.logTerminalSwitch << endl;
    cout << ::left << setw(25) << "  文件输出等级" << logger.logOutputLevelFile << endl;
    cout << ::left << setw(25) << "  终端输出等级" << logger.logOutputLevelTerminal << endl;
    cout << ::left << setw(25) << "  日志队列策略" << logger.logFileQueueSwitch << endl;
    cout << ::left << setw(25) << "  日志文件名称" << logger.logName << ".log" << endl;
    cout << ::left << setw(25) << "  日志保存路径" << logger.logFilePath << endl;
    cout << ::left << setw(25) << "  日志文件大小" << logger.logMixSize << "M" << endl;
    for (int i = 0; i < logger.logFilePath.size() + 25; i++)
    {
        cout << "-";
    }
    cout << endl;
#endif
}

std::string Logger::getCoutType(coutType coutType)
{
    return singleObject->coutTypeMap[coutType];
}

bool Logger::getFileType(fileType fileCoutBool)
{
    return singleObject->fileCoutMap[fileCoutBool];
}

bool Logger::getTerminalType(terminalType terminalCoutTyle)
{
    return singleObject->terminalCoutMap[terminalCoutTyle];
}

string Logger::getLogCoutTime()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    string tmp_str = tmp;
    return SQUARE_BRACKETS_LEFT + tmp_str + SQUARE_BRACKETS_RIGHT;
}

string Logger::getLogNameTime()
{
    time_t timep;
    time(&timep);
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d-%H:%M:%S", localtime(&timep));
    return tmp;
}

string Logger::getSourceFilePash()
{
#ifdef __linux__
    getcwd(sourceFilePath, sizeof(sourceFilePath) - 1);
#elif _WIN32
    getcwd(sourceFilePash, sizeof(sourceFilePash) - 1);
#endif
    string sourceFilePash_str = sourceFilePath;
    return sourceFilePash_str + SLASH;
}
string Logger::getFilePath()
{
    return settings.logFilePath + SLASH;
}

string Logger::getFilePathAndName()
{
#ifdef __linux__
    return settings.logFilePath + SLASH + settings.logName + ".log";
#elif _WIN32
    return settings.logFilePath + settings.logName + ".log";
#endif
}

string Logger::getFilePathAndNameAndTime()
{
    return settings.logFilePath + settings.logName + getLogNameTime() + ".log";
}

string Logger::getLogCoutProcessId()
{
#ifdef __linux__
    return to_string(getpid());
#elif _WIN32
    return to_string(getpid());
//  return GetCurrentProcessId();
#endif
}

string Logger::getLogCoutThreadId()
{
#ifdef __linux__
    return to_string(syscall(__NR_gettid));
#elif _WIN32
    return to_string(GetCurrentThreadId());
#endif
}

string Logger::getLogCoutUserName()
{
#ifdef __linux__
    struct passwd *my_info;
    my_info = getpwuid(getuid());
    string name = my_info->pw_name;
    return SPACE + name + SPACE;
#elif _WIN32
    const int MAX_LEN = 100;
    TCHAR szBuffer[MAX_LEN];
    DWORD len = MAX_LEN;
    GetUserName(szBuffer, &len);

    int iLen = WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, NULL, 0, NULL, NULL);
    char *chRtn = new char[iLen * sizeof(char)];
    WideCharToMultiByte(CP_ACP, 0, szBuffer, -1, chRtn, iLen, NULL, NULL);
    string str(chRtn);
    return " " + str + " ";
#endif
}

bool Logger::logFileWrite(string messages, string message, string line_effd)
{
    string filePathAndName = getFilePathAndName();

    long fileSize = filemanagement.verifyFileSize(filePathAndName);
    if (fileSize > (long)atoi(settings.logMixSize.data()) * MEGABYTES && settings.logBehavior == "1")
    {
        string newFileName = getFilePathAndNameAndTime();
        filemanagement.fileRename(filePathAndName, newFileName);
        filemanagement.createFile(filePathAndName);
    }
    if (settings.logFileQueueSwitch == SWITCH_OFF)
    {
        mutex_file->lock();
        ofstream file;
        file.open(filePathAndName, ::ios::app | ios::out);
        file << messages << message << line_effd;
        file.close();
        mutex_file->unlock();
    }
    else
    {
        insertQueue(messages + message + line_effd, filePathAndName);
    }
    return 1;
}

bool Logger::insertQueue(string messages, string filePathAndName)
{
    mutex_queue->lock();
    messageQueue.push(messages);
    if (messageQueue.size() >= 5000)
    {
        mutex_file->lock();
        ofstream file;
        file.open(filePathAndName, ::ios::app | ios::out);
        while (!messageQueue.empty())
        {
            file << messageQueue.front();
            messageQueue.pop();
        }
        file.close();
        mutex_file->unlock();
    }
    mutex_queue->unlock();
    return true;
}

string Logger::getLogSwitch()
{
    return settings.logSwitch;
}

string Logger::getLogFileSwitch()
{
    return settings.logFileSwitch;
}

string Logger::getLogTerminalSwitch()
{
    return settings.logTerminalSwitch;
}
string Logger::getCoutTypeColor(string colorType)
{
#ifdef __linux__
    return coutColor[colorType];
#elif _WIN32
    return "";
#endif
}

bool Logger::bindFileCoutMap(string value1, fileType value2)
{
    if (settings.logOutputLevelFile.find(value1) != std::string::npos)
    {
        fileCoutMap[value2] = true;
    }
    else
    {
        fileCoutMap[value2] = false;
    }
    return true;
}

bool Logger::bindTerminalCoutMap(string value1, terminalType value2)
{
    if (settings.logOutputLevelTerminal.find(value1) != std::string::npos)
    {
        terminalCoutMap[value2] = true;
    }
    else
    {
        terminalCoutMap[value2] = false;
    }
    return true;
}
