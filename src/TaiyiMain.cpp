#include "common.h"
#include "logs.h"
#include "TaiyiModule.h"


int main(int argc, char *argv[]) {
    LogInfo("start taiyi system.")

    TaiyiConfig cfg; // TODO 从配置文件中解析

    TaiyiModule *tym = TaiyiModule::CreateTaiyiModule(&cfg);
    if tym == nullptr {
        LogError("create taiyi module failed.");
        return -1;
    }
    tym->Start();

    // TODO 捕捉异常信号

    return 0;
}
