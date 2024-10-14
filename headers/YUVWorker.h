#ifndef YUVWORKER_H
#define YUVWORKER_H

#include <string>
#include "bmp_reader.h"
#include "Frame.h"
#include <cmath>
#include <iostream>

using BYTE = unsigned char;
using tuple_i = std::tuple<int, int, int>;

class YUVWorker
{
public:
    YUVWorker() = delete;

    static void addImage(
        std::string&& vidPath,
        std::string&& picPath,
        std::string&& outPrefix
    );

private:

    static void merge(
        Frame& video,
        Frame& image
    );

    static std::tuple<int, int, int> getStat(unsigned long long size);

};

#endif //YUVWORKER_H