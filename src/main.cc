#include "ctpapi.h"
#include "instrument.h"
#include "log.h"

int main(int argc, char* argv[]) {
    LOG_INFO("start tiayi client");

    InitInstrumentHandle();

    Start();
    Wait();

    // TODO Free/DestroyHandle

    return 0;
}
