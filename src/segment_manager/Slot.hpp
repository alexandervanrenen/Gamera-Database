#pragma once

#include <cstdint>

namespace dbi {

class Slot {
public:
   enum struct Type : uint8_t {kRedirectedFromOtherPage, kRedirectedToOtherPage, kUnusedMemory, kNormal};

   Slot(uint16_t offset, uint16_t bytes, Type type);

   uint16_t getOffset() const;
   uint16_t getLength() const;
   void setOffset(uint16_t offset);
   void setLength(uint16_t length);

   bool isRedirectedFromOtherPage() const;
   bool isRedirectedToOtherPage() const;

   bool isMemoryUnused() const;
   bool isNormal() const;
private:
   uint16_t qwe;
   uint16_t length;
};

}
