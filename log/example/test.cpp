#include <iostream>
#include "../logger.h" //添加日志库头文件

using namespace cpplog; //日志库的命名空间

int main()
{

    time_t begin, end;
    double ret;
    begin = clock();
    string name = "log";
    for (int i = 0; i < 10000; i++)
    {
        LogError("test" << name << i);
        LogWarn("test" << name);
        LogInfo("test" << name);
        LogDebug("test" << name);
        LogTrace("test" << name);
    }
    end = clock();
    ret = double(end - begin) / CLOCKS_PER_SEC;
    cout << "runtime:   " << ret << endl;

    return 0;
}