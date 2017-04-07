//#define __TESTS__

#ifdef __TESTS__
    #include "tests/testMain.hpp"
#else
    #include "controllerMain.hpp"
#endif

int main(){
    #ifdef __TESTS__
        #warning You are running the tests!
        testMain();
    #else
        controllerMain();
    #endif

    // We shouldn't be here...
    while(1);
}
