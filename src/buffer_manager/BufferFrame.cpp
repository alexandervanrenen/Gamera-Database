#include "BufferFrame.hpp"

namespace dbi {

using namespace std;

char* BufferFrame::data()
{
   return pageContent.data();
}

}
