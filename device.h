#ifndef DEVICE_H
#define DEVICE_H

#include <list>
#include <vector>
#include <utility>
#include "request.h"

namespace Smo_kurs{
class Device
{
public:
    Device();
    Device(int size, float alpha, float beta);

    bool isFreeDevice();

    int addNewRequest(float currentTime, Request request);
    std::list<std::pair<Request, int>> freeDoneDevices(float currentTime);
private:
    int size;
    float alpha;
    float betta;

    std::vector<std::pair<float, Request> *> devices;
};
}

#endif // DEVICE_H
