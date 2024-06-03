#ifndef __LOGS_H__
#define __LOGS_H__

#include <iostream>
using namespace std;

// TODO 对接spdlog库
#define LogDebug(...) cout << __VAR_ARGS__ << endl
#define LogInfo(...) cout << __VAR_ARGS__ << endl
#define LogError(...) cout << __VAR_ARGS__ << endl

#endif
