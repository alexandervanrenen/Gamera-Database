#include <sstream>
#include <string>

namespace dbi {

namespace util {

template<class T>
void writeBinary(std::ostream& out, const T& t)
{
   out.write((char*)&t, sizeof(T));
}

template<class T>
void readBinary(T& t, std::istream& in)
{
   in.read((char*)&t, sizeof(T));
}  

template<>
void writeBinary<std::string>(std::ostream& out, const std::string& str)
{
   size_t len = str.size();
   out.write((char*)&len, sizeof(size_t));
   out.write(str.data(), str.size());
}

template<>
void readBinary<std::string>(std::string& str, std::istream& in)
{
   size_t length;
   in.read((char*)&length, sizeof(size_t));
   str.resize(length);
   in.read(&str[0], length);
}

}

}
