#include <iostream>

#include "test_Logger.cpp"
#include "test_HeapTimer.cpp"
#include "test_HeapHeartbeatCounter.cpp"

// extern void test_HeapTimer();
// extern void test_Logger();
// extern void test_HeapHeartbeatCounter();

int main() {

    // test_Logger();

    // test_HeapTimer();

    // test_HeapHeartbeatCounter();

    test_HeapHeartbeatCounter_HeapTimer();

    return 0;
}