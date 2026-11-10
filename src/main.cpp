#include <iostream>
#include "solarSystem.h"

int main(int argc, char** argv)
{
    std::cout << "MAIN STARTED" << std::endl;

    std::cout << "Calling mySolarSystem..." << std::endl;

    mySolarSystem(argc, argv);

    std::cout << "Returned from mySolarSystem!" << std::endl;

    return 0;
}