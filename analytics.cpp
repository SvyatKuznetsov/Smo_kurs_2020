#include "analytics.h"
#include <map>

Smo_kurs::Analytics::Analytics()
{

}

void Smo_kurs::Analytics::newRequestGenerate(Smo_kurs::Request request)
{
    analytics.push_back(std::make_pair(std::make_pair(request, -1), NEW_REQ));
}

void Smo_kurs::Analytics::addRequestToBuff(Smo_kurs::Request request, int buff)
{
    analytics.push_back(std::make_pair(std::make_pair(request, buff), ADD_TO_BUFF));
}

void Smo_kurs::Analytics::removeRequestFromBuff(Smo_kurs::Request request, int buff)
{
    analytics.push_back(std::make_pair(std::make_pair(request, buff), REMOVE_FROM_BUFF));
}

void Smo_kurs::Analytics::getRequestFromBuff(Smo_kurs::Request request, int buff)
{
    analytics.push_back(std::make_pair(std::make_pair(request, buff), GET_FROM_BUFF));
}

void Smo_kurs::Analytics::addRequestToDevice(Smo_kurs::Request request, int device)
{
    analytics.push_back(std::make_pair(std::make_pair(request, device), ADD_TO_DEVICE));
}

void Smo_kurs::Analytics::removeRequestFromDevice(Smo_kurs::Request request, int device)
{
    analytics.push_back(std::make_pair(std::make_pair(request, device), REMOVE_FROM_DEVICE));
}

void Smo_kurs::Analytics::throwRequest(Smo_kurs::Request request)
{
    analytics.push_back(std::make_pair(std::make_pair(request, -1), THROW_REQUEST));
}

void Smo_kurs::Analytics::beginTransaction()
{
    sourcesAmount = 0;
    buffersAmount = 0;
    devicesAmount = 0;
    analytics.clear();
    req_proc.clear();
    req_fail.clear();
    time_in_system.clear();
    time_of_wait.clear();
    time_of_process.clear();
    disp_tow.clear();
    disp_top.clear();
    prob_of_fail.clear();
    deviceLoad.clear();
    steps.clear();
}

bool Smo_kurs::Analytics::config(int sourcesAmount, int buffersAmount, int devicesAmount)
{
    if (sourcesAmount > 0 && buffersAmount > 0 && devicesAmount > 0)
    {
        this->sourcesAmount = sourcesAmount;
        this->buffersAmount = buffersAmount;
        this->devicesAmount = devicesAmount;

        for (int i = 0; i < sourcesAmount; i++)
        {
            req_proc.push_back(0);
            req_fail.push_back(0);
            time_in_system.push_back(0);
            time_of_wait.push_back(0);
            time_of_process.push_back(0);
            disp_tow.push_back(0);
            disp_top.push_back(0);
            prob_of_fail.push_back(0);
        }

        for (int i = 0; i < devicesAmount; i++)
        {
            deviceLoad.push_back(0);
        }

        return true;
    }
    else
    {
        return false;
    }
}

void Smo_kurs::Analytics::commit(float allWorkTime)
{
    std::map<std::string, std::pair<float, float>> tmpTimeOfWait;
    std::map<std::string, std::pair<float, float>> tmpTimeOfProcess;

    std::map<std::string, std::pair<float, float>> tmpTimeOfDeviceWork;

    std::vector<std::string> buffersVec;
    std::vector<std::string> devicesVec;
    std::string tmpMessage = "";

    for (int i = 0; i < buffersAmount; i++)
    {
        buffersVec.push_back("null");
    }

    for (int i = 0; i < devicesAmount; i++)
    {
        devicesVec.push_back("null");
    }

    steps.push_back(StepModel(
                    buffersVec,
                    devicesVec,
                    tmpMessage,
                    0));

    for (auto it = analytics.begin(); it!= analytics.end(); ++it)
    {
        std::pair<Request, int> reqPair = (*it).first;

        switch ((*it).second)
        {
            case NEW_REQ:
            {
                tmpMessage += "Create request " +
                        std::to_string(reqPair.first.getSourceId()) +
                        "." +
                        std::to_string(reqPair.first.getRequestNumber());
            }
                break;
            case ADD_TO_BUFF:
            {
                std::string tmpKey = std::to_string(reqPair.first.getSourceId()) +
                                     "." +
                                     std::to_string(reqPair.first.getRequestNumber());
                tmpTimeOfWait.insert(std::make_pair(tmpKey, std::make_pair(reqPair.first.getTimeOfWait(), 0)));

                for (size_t i = 0; i < buffersVec.size(); i++)
                {
                    if (i == (reqPair.second + 1))
                    {
                        buffersVec.at(i) =
                                std::to_string(reqPair.first.getSourceId()) +
                                "." +
                                std::to_string(reqPair.first.getRequestNumber());
                    }
                }
                tmpMessage += " and load in buffer " + std::to_string(reqPair.second + 2);
                steps.push_back(StepModel(
                                buffersVec,
                                devicesVec,
                                tmpMessage,
                                reqPair.first.getTimeOfWait()));
                tmpMessage.clear();
            }
                break;
            case REMOVE_FROM_BUFF:
            {
                std::string tmpKey = std::to_string(reqPair.first.getSourceId()) +
                                     "." +
                                     std::to_string(reqPair.first.getRequestNumber());
                auto tmpIt = tmpTimeOfWait.find(tmpKey);
                if (tmpIt != tmpTimeOfWait.end())
                {
                    tmpIt->second.second = reqPair.first.getTimeOfWait();
                }

                *(req_fail.begin() + reqPair.first.getSourceId() - 1) = (*(req_fail.begin() + reqPair.first.getSourceId() - 1)) + 1;

                for (size_t i = 0; i < buffersVec.size(); i++)
                {
                    if (i == (reqPair.second))
                    {
                        buffersVec.at(i) = "null";
                    }
                }

                tmpMessage += ", request " +
                        std::to_string(reqPair.first.getSourceId()) +
                        "." +
                        std::to_string(reqPair.first.getRequestNumber()) +
                        " removed from buffer " +
                        std::to_string(reqPair.second+1);
                steps.push_back(StepModel(
                                buffersVec,
                                devicesVec,
                                tmpMessage,
                                reqPair.first.getTimeOfWait()));
                tmpMessage.clear();
            }
                break;
            case GET_FROM_BUFF:
            {
                std::string tmpKey = std::to_string(reqPair.first.getSourceId()) +
                                     "." +
                                     std::to_string(reqPair.first.getRequestNumber());
                auto tmpIt = tmpTimeOfWait.find(tmpKey);
                if (tmpIt != tmpTimeOfWait.end())
                {
                    tmpIt->second.second = reqPair.first.getTimeOfWait();
                }

                for (size_t i = 0; i < buffersVec.size(); i ++)
                {
                    if (i == (reqPair.second))
                    {
                        buffersVec.at(i) = "null";
                    }
                }
            }
                break;
            case ADD_TO_DEVICE:
            {
                std::string tmpKey = std::to_string(reqPair.first.getSourceId()) +
                                     "." +
                                     std::to_string(reqPair.first.getRequestNumber());

                tmpTimeOfDeviceWork.insert(std::make_pair(
                                               std::to_string(reqPair.second) + '.' + tmpKey,
                                               std::make_pair(reqPair.first.getTimeOfWait(), 0)
                                               ));

                tmpTimeOfProcess.insert(std::make_pair(
                                            tmpKey,
                                            std::make_pair(reqPair.first.getTimeOfWait(), 0)
                                            ));

                *(req_proc.begin() + reqPair.first.getSourceId() - 1) = (*(req_proc.begin() + reqPair.first.getSourceId() - 1)) + 1;

                for (size_t i = 0; i < devicesVec.size(); i ++)
                {
                    if (i == (reqPair.second - 1))
                    {
                        devicesVec.at(i) =
                                std::to_string(reqPair.first.getSourceId()) +
                                "." +
                                std::to_string(reqPair.first.getRequestNumber());
                    }
                }
                tmpMessage += "Device " + std::to_string(reqPair.second) + " load request " +
                        std::to_string(reqPair.first.getSourceId()) +
                        "." +
                        std::to_string(reqPair.first.getRequestNumber());

                steps.push_back(StepModel(
                                buffersVec,
                                devicesVec,
                                tmpMessage,
                                reqPair.first.getTimeOfWait()));
                tmpMessage.clear();
            }
                break;
            case REMOVE_FROM_DEVICE:
            {
                std::string tmpKey = std::to_string(reqPair.first.getSourceId()) +
                                     "." +
                                     std::to_string(reqPair.first.getRequestNumber());

                auto tmpIt = tmpTimeOfDeviceWork.find(std::to_string(reqPair.second) + '.' + tmpKey);
                if (tmpIt != tmpTimeOfDeviceWork.end())
                {
                    tmpIt->second.second = reqPair.first.getTimeOfWait();
                }

                auto tmpIt2 = tmpTimeOfProcess.find(tmpKey);
                if (tmpIt2 != tmpTimeOfProcess.end())
                {
                    tmpIt2->second.second = reqPair.first.getTimeOfWait();
                }

                for (size_t i = 0; i < devicesVec.size(); i ++)
                {
                    if (i == (reqPair.second - 1)) {
                        devicesVec.at(i) = "null";
                    }
                }

                tmpMessage += "Request " +
                        std::to_string(reqPair.first.getSourceId()) +
                        "." +
                        std::to_string(reqPair.first.getRequestNumber()) +
                        " removed from device " +
                        std::to_string(reqPair.second);
                steps.push_back(StepModel(
                                buffersVec,
                                devicesVec,
                                tmpMessage,
                                reqPair.first.getTimeOfWait()));
                tmpMessage.clear();
            }
                break;
        case THROW_REQUEST:
        {
            std::string tmpKey = std::to_string(reqPair.first.getSourceId()) +
                                 "." +
                                 std::to_string(reqPair.first.getRequestNumber());
            auto tmpIt = tmpTimeOfWait.find(tmpKey);
            if (tmpIt != tmpTimeOfWait.end())
            {
                tmpIt->second.second = reqPair.first.getTimeOfWait();
            }

            *(req_fail.begin() + reqPair.first.getSourceId() - 1) = (*(req_fail.begin() + reqPair.first.getSourceId() - 1)) + 1;

            for (size_t i = 0; i < buffersVec.size(); i ++)
            {
                if (i == (reqPair.second))
                {
                    buffersVec.at(i) = "null";
                }
            }

            tmpMessage += ", request " +
                    std::to_string(reqPair.first.getSourceId()) +
                    "." +
                    std::to_string(reqPair.first.getRequestNumber()) +
                    " throwed";
            steps.push_back(StepModel(
                            buffersVec,
                            devicesVec,
                            tmpMessage,
                            reqPair.first.getTimeOfWait()));
            tmpMessage.clear();
        }
            break;
        }
    }

    // count times
    std::vector<float> totalTOW;
    std::vector<float> totalTOP;
    std::vector<float> totalDevWork;

    for (int i = 0; i < sourcesAmount; i++)
    {
        totalTOW.push_back(0);
        totalTOP.push_back(0);
    }

    for (int i = 0; i < devicesAmount; i++)
    {
        totalDevWork.push_back(0);
    }

    for (auto it = tmpTimeOfDeviceWork.begin(); it != tmpTimeOfDeviceWork.end(); ++it)
    {
        std::string indexStr = "";
        for (char ch : it->first)
        {
            if (ch == '.')
            {
                break;
            }
            else
            {
                indexStr.push_back(ch);
            }
        }
        int index = std::stoi(indexStr);

        std::pair<float, float> pairDevWork = it->second;

        if (pairDevWork.second == 0)
            continue;

        totalDevWork.at(index - 1) += std::max(pairDevWork.first, pairDevWork.second) - std::min(pairDevWork.first, pairDevWork.second);
    }

    for (auto it = tmpTimeOfWait.begin(); it != tmpTimeOfWait.end(); ++it)
    {
        std::string indexStr = "";
        for (char ch : it->first)
        {
            if (ch == '.')
            {
                break;
            }
            else
            {
                indexStr.push_back(ch);
            }
        }
        int index = std::stoi(indexStr);

        std::pair<float, float> pairTOW = it->second;

        if (pairTOW.second == 0)
            continue;

        totalTOW.at(index - 1) += std::max(pairTOW.first, pairTOW.second) - std::min(pairTOW.first, pairTOW.second);
    }

    for (auto it = tmpTimeOfWait.begin(); it != tmpTimeOfWait.end(); ++it)
    {
        std::string indexStr = "";
        for (char ch : it->first)
        {
            if (ch == '.')
            {
                break;
            }
            else
            {
                indexStr.push_back(ch);
            }
        }
        int index = std::stoi(indexStr);

        std::pair<float, float> pairTOP = it->second;

        if (pairTOP.second == 0)
            continue;

        totalTOP.at(index - 1) += std::max(pairTOP.first, pairTOP.second) - std::min(pairTOP.first, pairTOP.second);
    }

    for (int i =0; i < deviceLoad.size(); i++)
    {
        deviceLoad.at(i) = (totalDevWork.at(i)/allWorkTime)*100;
    }

    for (int i =0; i < time_of_wait.size(); i++)
    {
        time_of_wait.at(i) = totalTOW.at(i)/tmpTimeOfWait.size();
    }

    for (int i =0; i < time_of_process.size(); i++)
    {
        time_of_process.at(i) = totalTOP.at(i)/tmpTimeOfProcess.size();
    }

    for (int i = 0; i < time_in_system.size(); i++)
    {
        time_in_system.at(i) = time_of_wait.at(i) + time_of_process.at(i);
    }

    for (int i = 0; i < prob_of_fail.size(); i++)
    {
        float a = req_fail.at(i);
        float b = req_proc.at(i);
        prob_of_fail.at(i) = (a/(a + b))*100;
    }

    for (int i = 0; i < disp_top.size(); i++)
    {
        disp_top.at(i) = (time_of_process.at(i)/time_in_system.at(i));
    }

    for (int i = 0; i < disp_tow.size(); i++)
    {
        disp_tow.at(i) = (time_of_wait.at(i)/time_in_system.at(i));
    }
}

std::vector<Smo_kurs::Analytics::StepModel> Smo_kurs::Analytics::getSteps() const
{
    return steps;
}

int Smo_kurs::Analytics::getLinesAmount() const
{
    return sourcesAmount + buffersAmount + devicesAmount;
}

int Smo_kurs::Analytics::getDevicesAmount() const
{
    return devicesAmount;
}

int Smo_kurs::Analytics::getBuffersAmount() const
{
    return buffersAmount;
}

int Smo_kurs::Analytics::getSourcesAmount() const
{
    return sourcesAmount;
}

std::vector<float> Smo_kurs::Analytics::getDeviceLoad() const
{
    return deviceLoad;
}

std::vector<float> Smo_kurs::Analytics::getProb_of_fail() const
{
    return prob_of_fail;
}

std::vector<float> Smo_kurs::Analytics::getDisp_top() const
{
    return disp_top;
}

std::vector<float> Smo_kurs::Analytics::getDisp_tow() const
{
    return disp_tow;
}

std::vector<float> Smo_kurs::Analytics::getTime_of_process() const
{
    return time_of_process;
}

std::vector<float> Smo_kurs::Analytics::getTime_of_wait() const
{
    return time_of_wait;
}

std::vector<float> Smo_kurs::Analytics::getTime_in_system() const
{
    return time_in_system;
}

std::vector<int> Smo_kurs::Analytics::getReq_fail() const
{
    return req_fail;
}

std::vector<int> Smo_kurs::Analytics::getReq_proc() const
{
    return req_proc;
}
