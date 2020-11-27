#include "request.h"

Smo_kurs::Request::Request()
{
    this->timeOfWait = 0.0;
    this->sourceId = 0;
    this->requestNumber = 0;
}

Smo_kurs::Request::Request(float timeOfWait, int sourceId, int requestNumber)
{
    this->timeOfWait = timeOfWait;
    this->sourceId = sourceId;
    this->requestNumber = requestNumber;
}

float Smo_kurs::Request::getTimeOfWait() const
{
    return timeOfWait;
}

int Smo_kurs::Request::getSourceId() const
{
    return sourceId;
}

int Smo_kurs::Request::getRequestNumber() const
{
    return requestNumber;
}

void Smo_kurs::Request::setTimeOfWait(float value)
{
    timeOfWait = value;
}
