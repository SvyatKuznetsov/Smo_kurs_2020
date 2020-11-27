#include "buffer.h"

Smo_kurs::Buffer::Buffer()
{
    this->size = 0;
}

Smo_kurs::Buffer::Buffer(int size)
{
    this->size = size;
    for (int i = 0; i < size; i++)
    {
        buffers.push_back(nullptr);
    }
}

bool Smo_kurs::Buffer::isFreeBuff()
{
    bool res = false;
    for (int i = 0; i < buffers.size(); i++)
    {
        if (buffers.at(i) == nullptr)
            res = true;
    }
    return res;
}

bool Smo_kurs::Buffer::isEmptyBuff()
{
    bool res = true;
    for (int i = 0; i < buffers.size(); i++)
    {
        if (buffers.at(i) != nullptr)
            res = false;
    }
    return res;
}

int Smo_kurs::Buffer::addNewRequest(Smo_kurs::Request request)
{
    for (int i = 0; i < buffers.size(); i++)
    {
        if (buffers.at(i) != nullptr)
            continue;
        else
        {
            buffers.at(i) = new Request(request.getTimeOfWait(), request.getSourceId(), request.getRequestNumber());
            return i - 1;
        }
    }

    return -1;
}

std::pair<Smo_kurs::Request, int> Smo_kurs::Buffer::getRequest()
{
    std::size_t reqToGet = 0;
    bool isSelected = false;
    for (std::size_t i = 0; i < buffers.size(); i++)
    {
        if (buffers.at(i) != nullptr)
        {
            if (!isSelected)
            {
                reqToGet = i;
                isSelected = true;
            }

            if (buffers.at(i)->getSourceId() < buffers.at(reqToGet)->getSourceId())
            {
                reqToGet = i;
            }
            else if (buffers.at(i)->getSourceId() == buffers.at(reqToGet)->getSourceId())
            {
                if (buffers.at(i)->getRequestNumber() < buffers.at(reqToGet)->getRequestNumber())
                {
                    reqToGet = i;
                }
            }
        }
        if (!isSelected)
        {
            reqToGet = i;
        }
    }

    Request req = *buffers.at(reqToGet);
    buffers.at(reqToGet) = nullptr;

    /*for (int i = 0; i < buffers.size() - 1; ++i)
    {
        if (buffers.at(i) == nullptr)
        {
            buffers.at(i) = buffers.at(i + 1);
            buffers.at(i + 1) = nullptr;
        }
    }*/

    if (isSelected)
        return std::make_pair(req, reqToGet);
    else
        return std::make_pair(Smo_kurs::Request(), -1);
}
