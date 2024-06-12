#include "HeapTimer.h"
#include <iostream>

void test_HeapTimer() {
    dwt::HeapTimer timer;

    
    for(int i = 0; i < 10; ++ i) {
        int sec = i + 3;
        timer.push(i, 1000 * sec, [=](int id) {
            std::cout << "timer " << id << " timeout, exp = " << sec << std::endl;
        });
    }


    while(timer.getNextTick() != -1);

}