#include "Timer.h"
#include <iostream>

void test_Timer() {
    dwt::Timer timer;

    
    for(int i = 0; i < 10; ++ i) {
        int sec = i + 3;
        timer.push(i, 1000 * sec, [=]() {
            std::cout << "timer " << i << " timeout, exp = " << sec << std::endl;
        });
    }


    while(timer.getNextTick() != -1);

}