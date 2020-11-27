#ifndef SOURCE_H
#define SOURCE_H

#include <vector>
#include <cmath>
#include "request.h"

namespace Smo_kurs {
class Source{
public:
    Source();
    Source(float lambda, int sourcesAmount);

    Smo_kurs::Request getNextRequest();

private:
    float lambda;
    int sourcesAmount;

    float *timesToWait;
    int *requestNumbers;

    void fillTimesToWait();
    std::pair<int, float> getMinTimeAndIndex();
    void removeMinTimeToWait();
};
}

#endif // SOURCE_H
