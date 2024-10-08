#include "bmp_reader.h"

void BMPReader::openBMP(const std::string& fileName)
{
    std::ifstream is{fileName, std::ios::binary|std::ios::in};
    if (!is.is_open())
    {
        std::cout << "Can't open this file: " << fileName << "\n";
        return;
    }

    bmpFile = std::unique_ptr<BMPFile>(new BMPFile());

    is.read(reinterpret_cast<std::ifstream::char_type*>(&bmpFile->bmph), sizeof(BMPHeader));
    is.read(reinterpret_cast<std::ifstream::char_type*>(&bmpFile->dibh), sizeof(DIBHeader));

    int data_size = bmpFile->dibh.width * bmpFile->dibh.height *
        bmpFile->dibh.bitsPerPixel / 8;
    
    is.seekg(bmpFile->bmph.offset);
    
    bmpFile->data.resize(data_size);
    is.read(reinterpret_cast<std::ifstream::char_type*>(&bmpFile->data.front()), data_size);

    is.close();
}

void BMPReader::closeBMP()
{
    // let's let smart_ptr do all the work for us
    bmpFile.reset();

    // delete all data in vector
    yuv.data.erase(yuv.begin(), yuv.end());
    yuv.data.shrink_to_fit();
}

void BMPReader::toYUV()
{ 
    int bytesPerPixel = bmpFile->dibh.bitsPerPixel / 8;
    int usefullRowSize = bytesPerPixel * bmpFile->dibh.width;
    int rowPadding = (4 - (usefullRowSize % 4)) % 4;
    int rowSize = usefullRowSize + rowPadding;
    
    yuv.data.resize(usefullRowSize * bmpFile->dibh.height);

    
    int ptr = 0;

    for (int i (bmpFile->dibh.data_size - rowSize); i > -1; i -= rowSize)
        for (int j = i; j < i + usefullRowSize; j += bytesPerPixel)
        {
            yuv[ptr++] = bmpFile->data[j];
            yuv[ptr++] = bmpFile->data[j + 1];
            yuv[ptr++] = bmpFile->data[j + 2];
        }

    bmpFile->data = yuv.data;  

    rowSize = bmpFile->dibh.width * 3;
    yuv.info = std::pair<int, int>(rowSize / 3, bmpFile->dibh.height);
    yuv.data.resize(bmpFile->dibh.width * bmpFile->dibh.width * 3 / 2);
    
    double Y, U, V;
    ptr = 0;
    for(int i = 0; i < bmpFile->data.size(); i+=3)
    {
        YfromRGB(Y, bmpFile->data[i+2], bmpFile->data[i+1], bmpFile->data[i]);
        yuv[ptr++] = Y;
    }
    
    for(int i = 0; i < bmpFile->dibh.height; i += 2)
        for (int j = rowSize * i; j < rowSize * (i + 1); j += 6)
        {
            UfromRGB(U, bmpFile->data[j+2], bmpFile->data[j+1], bmpFile->data[j]);
            yuv[ptr++] = U;
        }

    for(int i = 0; i < bmpFile->dibh.height; i += 2)
        for (int j = rowSize * i; j < rowSize * (i + 1); j += 6)
        {
            VfromRGB(V, bmpFile->data[j+2], bmpFile->data[j+1], bmpFile->data[j]);
            yuv[ptr++] = V;
        }
}

Frame& BMPReader::getYUV()
{
    return yuv;
}