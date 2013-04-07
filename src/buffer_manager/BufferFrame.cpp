#include "BufferFrame.hpp"

namespace dbi {

using namespace std;

void* BufferFrame::getData()
{
    return reinterpret_cast<void*>(data.data());
}

}
