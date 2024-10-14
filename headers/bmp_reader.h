#ifndef BMP_READER_H
#define BMP_READER_H

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <Frame.h>
#include <thread>

using BYTE = unsigned char;

#pragma pack(1)
// Определим структуру bmp-файла

struct BMPHeader {
    char ID[2];
    uint32_t size;
    uint16_t unused1;
    uint16_t unused2;
    uint32_t offset;

    void toString();
};


struct DIBHeader {
    uint32_t header_size;
    uint32_t width;
    uint32_t height;
    uint16_t color_planes;
    uint16_t bitsPerPixel;
    uint32_t BI_RGB;
    uint32_t data_size;
    uint32_t pWidth;
    uint32_t pHeight;
    uint32_t colors_count;
    uint32_t important_colors_count;

    void toString();
};

struct BMPFile
{
    BMPHeader bmph;
    DIBHeader dibh;

    std::vector<BYTE> data;
    short rowPadding;

    void toString();
};

#pragma pop

class BMPReader
{
public:
    bool openBMP(const std::string& fileName);
    void closeBMP();
    Frame& getYUV();
    void toYUV();
    

private:
    Frame yuv;
    std::unique_ptr<BMPFile> bmpFile;

    std::unique_ptr<BMPFile> getFile();
    void toString();
    void YfromRGB(double& Y, const double R, const double G, const double B)
    {
        Y = (( 66 * R + 129 * G +  25 * B + 128) / 256) +  16;
    }
    void UfromRGB(double& U, const double R, const double G, const double B)
    {
        U = ((-38 * R -  74 * G + 112 * B + 128) / 256) + 128;
    }
    void VfromRGB(double& V, const double R, const double G, const double B)
    {
        V = ((112 * R -  94 * G -  18 * B + 128) / 256) + 128;
    }

    bool isValid() const;
};

#endif //BMP_READER_H