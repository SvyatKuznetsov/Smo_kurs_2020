#include "controller.h"
#include "source.h"
#include "buffer.h"
#include "device.h"

Smo_kurs::Controller::Controller()
{
    this->alpha = 0;
    this->betta = 0;
    this->lambda = 0;

    this->sourcesAmount = 0;
    this->buffersAmount = 0;
    this->devicesAmount = 0;

    this->requestsNumber = 0;
}

std::list<std::string> Smo_kurs::Controller::modulateWork(Analytics &analytics)
{
    analytics.beginTransaction();
    analytics.config(sourcesAmount, buffersAmount, devicesAmount);

    std::list<std::string> resultList;

    Smo_kurs::Source *sources = new Smo_kurs::Source(lambda, sourcesAmount);
    Smo_kurs::Buffer *buffers = new Smo_kurs::Buffer(buffersAmount);
    Smo_kurs::Device *devices = new Smo_kurs::Device(devicesAmount, alpha, betta);

    float currentTime = 0;

    for (int i = 0; i < requestsNumber; i++) {
        Request nextRequest = sources->getNextRequest();
        currentTime += nextRequest.getTimeOfWait();
        nextRequest.setTimeOfWait(currentTime);

        std::list<std::pair<Request, int> > doneRequests = devices->freeDoneDevices(currentTime);
        if (doneRequests.size() > 0)
        {
            for (auto it = doneRequests.begin(); it != doneRequests.end(); ++it)
            {
                resultList.push_back("Девайс №" + std::to_string((*it).second) + " освободился в " + std::to_string((*it).first.getTimeOfWait()));
                analytics.removeRequestFromDevice((*it).first, (*it).second);

                if (!buffers->isEmptyBuff())
                {
                    std::pair<Request, int> pair2 = buffers->getRequest();
                    Request request2 = pair2.first;
                    analytics.getRequestFromBuff(Request(
                                                     (*it).first.getTimeOfWait(),
                                                     request2.getSourceId(),
                                                     request2.getRequestNumber()),
                                                 pair2.second);

                    int deviceNum2 = devices->addNewRequest(std::max(request2.getTimeOfWait(), (*it).first.getTimeOfWait()), request2);
                    resultList.push_back("Заявка " + std::to_string(request2.getSourceId()) + "." + std::to_string(request2.getRequestNumber())
                                         + " загружена в девайс №" + std::to_string(deviceNum2));
                    analytics.addRequestToDevice(Request(
                                                     std::max(request2.getTimeOfWait(), (*it).first.getTimeOfWait()),
                                                     request2.getSourceId(),
                                                     request2.getRequestNumber()),
                                                 (*it).second);
                }
            }
        }

        std::string tmpResStr = "Generated заявка " + std::to_string(nextRequest.getSourceId()) + "." + std::to_string(nextRequest.getRequestNumber());
        analytics.newRequestGenerate(Request(
                                         currentTime,
                                         nextRequest.getSourceId(),
                                         nextRequest.getRequestNumber()));
        if (!devices->isFreeDevice())
        {
            if (buffers->isFreeBuff())
            {
                int bufferNum = buffers->addNewRequest(nextRequest);
                tmpResStr += " и загружена в буффер №" + std::to_string(bufferNum + 1);
                analytics.addRequestToBuff(Request(
                                                 currentTime,
                                                 nextRequest.getSourceId(),
                                                 nextRequest.getRequestNumber()),
                                             bufferNum);
            }
            else
            {
                resultList.push_back("Заявка " + std::to_string(nextRequest.getSourceId()) + "." + std::to_string(nextRequest.getRequestNumber())
                                     + " ушла в отказ.");
                analytics.throwRequest(Request(
                                           currentTime,
                                           nextRequest.getSourceId(),
                                           nextRequest.getRequestNumber()
                                           ));
            }
        }
        else
        {
            int deviceNum = devices->addNewRequest(currentTime, nextRequest);
            tmpResStr += " и загружена в девайс №" + std::to_string(deviceNum);

            if (buffers->isFreeBuff())
            {
                int bufferNum = buffers->addNewRequest(nextRequest);
                analytics.addRequestToBuff(Request(
                                                 currentTime,
                                                 nextRequest.getSourceId(),
                                                 nextRequest.getRequestNumber()),
                                             bufferNum);
                if (!buffers->isEmptyBuff())
                {
                    std::pair<Request, int> pair3 = buffers->getRequest();
                    Request request2 = pair3.first;
                    analytics.getRequestFromBuff(Request(
                                                     currentTime,
                                                     request2.getSourceId(),
                                                     request2.getRequestNumber()),
                                                 pair3.second);
                    analytics.addRequestToDevice(Request(
                                                     currentTime,
                                                     nextRequest.getSourceId(),
                                                     nextRequest.getRequestNumber()),
                                                 deviceNum);
                }
            }
        }

        resultList.push_back(tmpResStr);
    }

    analytics.commit(currentTime);
    return resultList;
}

void Smo_kurs::Controller::setDevicesAmount(int value)
{
    devicesAmount = value;
}

void Smo_kurs::Controller::setRequestsNumber(int value)
{
    requestsNumber = value;
}

void Smo_kurs::Controller::setBuffersAmount(int value)
{
    buffersAmount = value;
}

void Smo_kurs::Controller::setSourcesAmount(int value)
{
    sourcesAmount = value;
}

void Smo_kurs::Controller::setLambda(float value)
{
    lambda = value;
}

void Smo_kurs::Controller::setBetta(float value)
{
    betta = value;
}

void Smo_kurs::Controller::setAlpha(float value)
{
    alpha = value;
}
