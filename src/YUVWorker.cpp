#include "YUVWorker.h"

void YUVWorker::addImage(
    std::string&& vidPath,
    std::string&& picPath,
    std::string&& outPrefix
)
{

    std::ifstream is;
    is.open (vidPath, std::ios::binary | std::ios::in);
    if (!is.is_open())
    {
        std::cout << "can't open file: " << vidPath << "\n";
        return;
    }

    // find size of file
    // (that way is due to Linux)
    unsigned long long fileSize;
    is.seekg (0, std::ios::end);
    fileSize = (unsigned long long)is.tellg();
    is.seekg (0, std::ios::beg);

    // frame data (width, height, frame count)
    auto frameData = getStat(fileSize);
    if (std::get<0>(frameData) == 0)
    {
        std::cout << "Unknown video resolution. Exiting...";
        return;
    }

    // bytes count of one frame
    int frameLength = std::get<0>(frameData) * std::get<1>(frameData) * 1.5;
    Frame frame;
    frame.info = std::move(std::pair<int, int> (std::get<0>(frameData), std::get<1>(frameData)));
    frame.data.resize(frameLength);
    
    // skip new lines
    is.unsetf(std::ios::skipws);

    // read bmp picture
    BMPReader reader{};
    if (!reader.openBMP(picPath))
        return;
    
    reader.toYUV();

    // open stream for writing modified video
    std::ofstream os(std::string(outPrefix + "/output.yuv"), std::ios::binary|std::ios::out);
    if (!os.is_open())
    {
        std::cout << "can't open file output file\n";
        return;
    }

    for (int i = 0; i < std::get<2>(frameData); ++i)
    {
        is.read(reinterpret_cast<std::ifstream::char_type*>(&frame.data.front()), frameLength);
        merge(frame, reader.getYUV());        
        os.write(reinterpret_cast<std::ifstream::char_type*>(&frame.data.front()), frame.data.size());
        is.seekg((i + 1) * frameLength);
    }
    is.close();
    os.close();
    reader.closeBMP();

    std::cout << "Job done successfully\n";
};


void YUVWorker::merge(
        Frame& frame,
        Frame& image
)
{
    int imageLength = image.size() / 3 * 2;
    int frameLength = frame.size() / 3 * 2;

    // rewrite y values on frame
    for (int i = 0; i < image.info.second; ++i)
        for (int j = 0; j < image.info.first; ++j)
        {
            frame[frame.info.first * i + j] = image[image.info.first * i + j];
        }

    
    int imageVStart = imageLength * 5 / 4;
    int frameVStart = frameLength * 5 / 4;
    int imageVRow = image.info.first / 2;
    int frameVRow = frame.info.first / 2;

    // rewrite u and v values
    for (int i = 0; i < image.info.second / 2; ++i)
        for (int j = 0; j < image.info.first / 2; ++j)
        {
            frame[frameLength + frameVRow * i + j] = image[imageLength + imageVRow * i + j];
            frame[frameVStart + frameVRow * i + j] = image[imageVStart + imageVRow * i + j];
        }
}


// maybe there is better method, but idk
std::tuple<int, int, int> YUVWorker::getStat(unsigned long long size)
{
    double frames = size / 1920 / 1080 / 3. * 2;
    if (frames == (double)std::floor(frames))
    {
        return tuple_i{1920, 1080, (int)frames};
    }
    else
    {
        frames = size / 720 / 576 / 3. * 2.;
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
    }
    return tuple_i{0, 0, 0};
}