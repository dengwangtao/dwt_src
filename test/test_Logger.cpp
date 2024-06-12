#include <iostream>


#define DWT_DEBUG

#include "tcp/Logger.h"

void test_Logger() {
    LOG_DEBUG("LOG_DEBUG %s:%d %s", __FILE__, __LINE__, __FUNCTION__);
    LOG_INFO("LOG_INFO %s:%d %s", __FILE__, __LINE__, __FUNCTION__);
    LOG_WARN("LOG_WARN %s:%d %s", __FILE__, __LINE__, __FUNCTION__);
    LOG_ERROR("LOG_ERROR %s:%d %s", __FILE__, __LINE__, __FUNCTION__);
    LOG_FATAL("LOG_FATAL %s:%d %s", __FILE__, __LINE__, __FUNCTION__);
}