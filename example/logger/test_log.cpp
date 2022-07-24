#include <iostream>
#include "../../log/logger.h" //添加日志库头文件

using namespace log; //日志库的命名空间

int main()
{

    time_t begin, end;
    double ret;
    begin = clock();
    string name = "日志";
    for (int i = 0; i < 10000; i++)
    {
        LogError("测试" << name << i);
        // LogWarn("测试" << name);
        // LogInfo("测试" << name);
        LogDebug("测试" << name);
        LogTrace("测试" << name);
    }
    end = clock();
    ret = double(end - begin) / CLOCKS_PER_SEC;
    cout << "runtime:   " << ret << endl;

    return 0;
}