#include "YUVWorker.h"
#include <stdarg.h>
#include <iostream>

int main(int argc, char* argv[])
{
    YUVWorker::addImage(argv[1], argv[2], argv[3]);
    std::cout << "Exiting...\n";

    return 0;
}