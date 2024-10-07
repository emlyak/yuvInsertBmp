#include "YUVWorker.h"
#include <stdarg.h>

int main(int argc, char* argv[])
{
    YUVWorker::addImage(argv[1], argv[2], argv[3]);

    return 0;
}