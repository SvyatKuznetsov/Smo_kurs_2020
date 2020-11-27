#include "source.h"
#include <QApplication>

Smo_kurs::Source::Source()
{
    this->lambda = 0;
    this->sourcesAmount = 0;

    this->timesToWait = new float[this->sourcesAmount];
    this->requestNumbers = new int[this->sourcesAmount];
}

Smo_kurs::Source::Source(float lambda, int sourcesAmount)
{
    this->lambda = lambda;
    this->sourcesAmount = sourcesAmount;

    this->timesToWait = new float[sourcesAmount];
    this->requestNumbers = new int[sourcesAmount];

    for (int i = 0; i < sourcesAmount; i++)
    {
        this->requestNumbers[i] = 0;
        this->timesToWait[i] = -1;
    }
}

Smo_kurs::Request Smo_kurs::Source::getNextRequest()
{
    fillTimesToWait();
    std::pair<int, float> min = getMinTimeAndIndex();
    removeMinTimeToWait();
    return Request(min.second, min.first + 1, requestNumbers[min.first]);
}

void Smo_kurs::Source::fillTimesToWait()
{
    for (int i = 0; i < sourcesAmount; i++)
    {
        if (timesToWait[i] <= 0)
        {
            requestNumbers[i]++;
            timesToWait[i] = (-1/lambda)*std::log(((float)qrand()/(float)RAND_MAX));
        }
    }
}

std::pair<int, float> Smo_kurs::Source::getMinTimeAndIndex()
{
    std::pair<int, float> min = std::make_pair(0, timesToWait[0]);
    for (int i = 0; i < sourcesAmount; i++)
    {
        if (timesToWait[i] < min.second)
        {
            min = std::make_pair(i, timesToWait[i]);
        }
    }

    return min;
}

void Smo_kurs::Source::removeMinTimeToWait()
{
    std::pair<int, float> min = getMinTimeAndIndex();
    for (int i = 0; i < sourcesAmount; i++)
    {
        timesToWait[i] -= min.second;
    }
}
