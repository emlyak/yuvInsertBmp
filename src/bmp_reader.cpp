#include "bmp_reader.h"

bool BMPReader::openBMP(const std::string& fileName)
{
    std::ifstream is{fileName, std::ios::binary|std::ios::in};
    if (!is.is_open())
    {
        std::cout << "Can't open this file: " << fileName << "\n";
        return false;
    }

    bmpFile = std::unique_ptr<BMPFile>(new BMPFile());

    is.read(reinterpret_cast<std::ifstream::char_type*>(&bmpFile->bmph), sizeof(BMPHeader));
    // check the magic number
    if (bmpFile->bmph.ID[0] != 'B' && bmpFile->bmph.ID[1] != 'M')
    {
        is.close();
        return false;
    }

    is.read(reinterpret_cast<std::ifstream::char_type*>(&bmpFile->dibh), sizeof(DIBHeader));
    // check format
    if (!isValid())
    {
        is.close();
        return false;
    }

    // calculate row padding in bytes
    bmpFile->rowPadding = (4 - (bmpFile->dibh.bitsPerPixel / 8 * bmpFile->dibh.width % 4)) % 4;

    int data_size = (bmpFile->dibh.width *
        bmpFile->dibh.bitsPerPixel / 8 + bmpFile->rowPadding) * bmpFile->dibh.height;
    
    is.seekg(bmpFile->bmph.offset);
    
    bmpFile->data.resize(data_size);
    is.read(reinterpret_cast<std::ifstream::char_type*>(&bmpFile->data.front()), data_size);
    is.close();

    return true;
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
    int rowSize = usefullRowSize + bmpFile->rowPadding;

    yuv.info = std::move(std::pair<int, int>(rowSize / 3, bmpFile->dibh.height));
    yuv.data.resize(bmpFile->dibh.height * bmpFile->dibh.width * 3 / 2);

    std::thread t1 (
        [&](){
            double Y;  
            int ptr = 0;
            for (int i (bmpFile->dibh.data_size - rowSize); i > -1; i -= rowSize)
                for (int j = i; j < i + usefullRowSize; j += bytesPerPixel)
                {
                    YfromRGB(Y, bmpFile->data[j+2], bmpFile->data[j+1], bmpFile->data[j]);
                    yuv[ptr++] = Y;
                }
        }
    );

    std::thread t2 (
        [&](){
            double U;
            int ptr = bmpFile->dibh.height * bmpFile->dibh.width;
            for (int i (bmpFile->dibh.data_size - rowSize); i > -1; i -= 2 * rowSize)
                for (int j = i; j < i + usefullRowSize; j += 2 * bytesPerPixel)
                {
                    UfromRGB(U, bmpFile->data[j+2], bmpFile->data[j+1], bmpFile->data[j]);
                    yuv[ptr++] = U;
                }
        }
    );

    std::thread t3 (
        [&](){
            double V;
            int ptr = bmpFile->dibh.height * bmpFile->dibh.width * 5 / 4;
            for (int i (bmpFile->dibh.data_size - rowSize); i > -1; i -= 2 * rowSize)
                for (int j = i; j < i + usefullRowSize; j += 2 * bytesPerPixel)
                {
                    VfromRGB(V, bmpFile->data[j+2], bmpFile->data[j+1], bmpFile->data[j]);
                    yuv[ptr++] = V;
                }
        }
    );
   
   t1.join();
   t2.join();
   t3.join(); 
}

Frame& BMPReader::getYUV()
{
    return yuv;
}

bool BMPReader::isValid() const
{
    if (bmpFile->dibh.bitsPerPixel != 0x18)
    {
        std::cout << "Please, use 24-bits bmp-file without alpha-channel\n";
        return false;
    }

    if(bmpFile->dibh.BI_RGB != 0x0)
    {
        std::cout << "Please, use bmp-file without compression\n";
        return false;
    }

    if(bmpFile->dibh.color_planes != 0x1)
    {
        std::cout << "Please, use bmp-file without color planes\n";
        return false;
    }

    if(bmpFile->dibh.colors_count != 0x0)
    {
        std::cout << "Please, use bmp-file without color palette\n";
        return false;
    }
        
    return true;
}