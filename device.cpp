    #include "device.h"
#include <algorithm>
#include <QApplication>
#include <math.h>

Smo_kurs::Device::Device()
{
    this->size = 0;
    this->alpha = 0;
    this->betta = 0;
}

Smo_kurs::Device::Device(int size, float alpha, float betta)
{
    this->size = size;
    this->alpha = alpha;
    this->betta = betta;
    for (int i = 0; i < size; i++)
    {
        devices.push_back(nullptr);
    }
}

bool Smo_kurs::Device::isFreeDevice()
{
    bool res = false;
    for (int i = 0; i < devices.size(); i++)
    {
        if (devices.at(i) == nullptr)
            res = true;
    }
    return res;
}

int Smo_kurs::Device::addNewRequest(float currentTime, Smo_kurs::Request request)
{
    float timeToDo = currentTime + ((float)qrand()/(float)RAND_MAX)*(betta - alpha) + alpha;

    for (int i = 0; i < devices.size(); i++)
    {
        if (devices.at(i) == nullptr)
        {
            devices.at(i) = new std::pair<float, Request>(timeToDo, Request(request.getTimeOfWait(), request.getSourceId(), request.getRequestNumber()));
            return i+1;
        }
    }

    return -1;
}

std::list<std::pair<Smo_kurs::Request, int> > Smo_kurs::Device::freeDoneDevices(float currentTime)
{
    std::list<std::pair<Request, int>> tmpList;
    std::vector<std::pair<float, Request> *> devicesTmp;

    for (int i = 0; i < devices.size(); i++)
    {
        if (devices.at(i) != nullptr)
        {
            if ((*devices.at(i)).first < currentTime)
            {
                tmpList.push_back(std::make_pair(
                                      Request(
                                          (*devices.at(i)).first,
                                          (*devices.at(i)).second.getSourceId(),
                                          (*devices.at(i)).second.getRequestNumber()
                                          ),
                                      i + 1));
                devicesTmp.push_back(nullptr);
            }
            else
            {
                devicesTmp.push_back(devices.at(i));
            }
        }
        else
        {
            devicesTmp.push_back(nullptr);
        }
    }

    devices = devicesTmp;

    return tmpList;
}
