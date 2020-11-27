#ifndef BUFFER_H
#define BUFFER_H

#include "request.h"
#include <vector>

namespace Smo_kurs {
class Buffer
{
public:
    Buffer();
    Buffer(int size);

    bool isFreeBuff();
    bool isEmptyBuff();

    int addNewRequest(Request request);
    std::pair<Request, int> getRequest();
private:
    int size;

    std::vector<Request *> buffers;
};
}

#endif // BUFFER_H
