#include <iostream> 

#include "common/TaiyiMdSpi.h"

using namespace std;

#pragma comment (lib, "thostmduserapi.lib")
#pragma comment (lib, "thosttraderapi.lib")

CThostFtdcMdApi *g_pMdUserApi = nullptr;
char gMdFrontAddr[] = "tcp://180.168.146.187:10010";

int main(int argc, char *argv[]) {
    cout << "Taiyi setup" << endl;

    g_pMdUserApi = CThostFtdcMdApi::CreateFtdcMdApi();
    CThostFtdcMdSpi *pMdUserSpi = new TaiyiMdSpi;
    g_pMdUserApi->RegisterSpi(pMdUserSpi);
    g_pMdUserApi->RegisterFront(gMdFrontAddr);
    g_pMdUserApi->Init();

    return 0;
}