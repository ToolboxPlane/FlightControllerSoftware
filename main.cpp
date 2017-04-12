//#define __TESTS__

#ifdef __TESTS__
    #warning You are running in test mode!
    #include "tests/testMain.hpp"
#else
    #include "controllerMain.hpp"
#endif

int main(){
    _main();

    // We shouldn't be here...
    while(1);
}
