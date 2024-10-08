#include "YUVWorker.h"

void YUVWorker::addImage(
    std::string vidPath,
    std::string picPath,
    std::string outPrefix
)
{

    std::ifstream is;
    is.open (vidPath, std::ios::binary | std::ios::in);
    if (!is.is_open())
    {
        std::cout << "can't open file";
        return;
    }

    // find size of file
    // (that way is due to Linux)
    unsigned long long fileSize;
    is.seekg (0, std::ios::end);
    fileSize = (int)is.tellg() - 1;
    is.seekg (0, std::ios::beg);

    // frame data (width, height, frame count)
    auto frameData = getStat(fileSize);

    // bytes count of one frame
    int frameLength = std::get<0>(frameData) * std::get<1>(frameData) * 1.5;
    Frame frame;
    frame.info = std::move(std::pair<int, int> (std::get<0>(frameData), std::get<1>(frameData)));
    frame.data.resize(frameLength);
    
    // skip new lines
    is.unsetf(std::ios::skipws);

    // read bmp picture
    BMPReader reader{};
    reader.openBMP(picPath);
    reader.toYUV();

    // open stream for writing modified video
    std::ofstream os(std::string(outPrefix + "/output.yuv"), std::ios::binary|std::ios::out);

    for (int i = 0; i < std::get<2>(frameData); ++i)
    {
        is.read(reinterpret_cast<std::ifstream::char_type*>(&frame.data.front()), frameLength);
        merge(frame, reader.getYUV());        
        os.write(reinterpret_cast<std::ifstream::char_type*>(&frame.data.front()), frame.data.size());
        is.seekg((i + 1) * frameLength);
        std::cout << "Обработано фрэймов: " << i + 1 << "\n";
    }
    reader.closeBMP();
    is.close();
    os.close();
};


void YUVWorker::merge(
        Frame& frame,
        Frame& image
)
{
    int imageLength = image.size() / 3 * 2;
    int frameLength = frame.size() / 3 * 2;

    for (int i = 0; i < image.info.second; ++i)
        for (int j = 0; j < image.info.first; ++j)
        {
            frame[frame.info.first * i + j] = image[image.info.first * i + j];
        }

    for (int i = 0; i < image.info.second / 2; ++i)
        for (int j = 0; j < image.info.first / 2; ++j)
        {
            frame[frameLength + frame.info.first / 2 * i + j] = image[imageLength + image.info.first / 2 * i + j];
            frame[frameLength * 5 / 4 + frame.info.first / 2 * i + j] = image[imageLength * 5 / 4 + image.info.first / 2 * i + j];
        }


}


// maybe there is better method, but idk
std::tuple<int, int, int> YUVWorker::getStat(unsigned long long size)
{
    double frames = size / 720 / 576 / 3. * 2.;
    if (frames == (double)std::floor(frames))
    {
        return tuple_i{720, 576, (int)frames};
    }
    else
    {
        frames = size / 352 / 288 / 3. * 2;
        if (frames == std::floor(frames))
        {
            return tuple_i{352, 288, (int)frames};
        }
        else
        {
            frames = size / 176 / 144 / 3. * 2;   
            if (frames == std::floor(frames))
            {
                return tuple_i{176, 144, (int)frames};
            }
        }
    }
    return tuple_i{0, 0, 0};
}