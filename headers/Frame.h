#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <tuple>
#include <iterator>

using BYTE = unsigned char;
using tuple_i = std::tuple<int, int, int>;

struct Frame
{

public:

    Frame() = default;

    Frame(std::vector<BYTE>&& data_, std::pair<int, int>&& info_)
    {
        data = std::move(data_);
        info = std::move(info_);
    }

    Frame(Frame& rhs)
    : data(rhs.data),
    info(rhs.info)
    {}

    Frame(Frame&& rhs)
    : data(std::move(data)),
    info(std::move(info))
    {}

    std::vector<BYTE>::iterator begin()
    {
        return data.begin();
    }

    std::vector<BYTE>::iterator end()
    {
        return data.end();
    }

    BYTE& operator[](std::size_t i)
    {
        return data[i];
    }

    std::size_t size() const
    {
        return data.size();
    }

    std::vector<BYTE> data;
    std::pair<int, int> info;

};


#endif //FRAME_H