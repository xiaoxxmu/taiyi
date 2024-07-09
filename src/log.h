
#pragma once

#include <cstdio>

#define LOG_DEBUG(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } while(0)

#define LOG_INFO(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } while(0)

#define LOG_ERROR(...) \
    do { \
        printf(__VA_ARGS__); \
        printf("\n"); \
    } while(0)
