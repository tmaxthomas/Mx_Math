#include <iostream>
#include <string>
#include <cassert>
#include "CalcMachine.h"

int main(int argc, char* argv[]){
    assert(argc > 2);
    CalcMachine cm(argv[1]);
    if(argv[2] == std::string("evaluate")) {
        assert(argc == 4);
        std::cout << cm.evaluate(std::stod(argv[3]));
    } else if(argv[2] == std::string("integrate")) {
        assert(argc == 5);
        std::cout << cm.integrate(std::stod(argv[3]), std::stod(argv[4]));
    } else if(argv[2] == std::string("derivative")) {
        assert(argc == 3);

    }
}