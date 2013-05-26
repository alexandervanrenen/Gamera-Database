#include "Slot.hpp"
#include "common/Config.hpp"
#include <cassert>

using namespace std;

namespace dbi {

namespace {
const uint16_t kTopBitOne = 0x8000;
const uint16_t kTopBitZero = 0x0000;
}

Slot::Slot(uint16_t offsetInput, uint16_t lengthInput, Type type)
: qwe(offsetInput)
, length(lengthInput)
{
   // Top bit may not be used
   assert(qwe == (~kTopBitZero&qwe));
   assert(length == (~kTopBitZero&length));
   assert((qwe==0||length==0) ? (qwe==0&&length==0&&type==Type::kNormal) : true);

   // Set top bit according to type
   switch(type) {
      case Type::kRedirectedFromOtherPage: // 11
         qwe |= kTopBitOne;
         length |= kTopBitOne;
         break;
      case Type::kRedirectedToOtherPage: // 10
         qwe |= kTopBitOne;
         length |= kTopBitZero;
         break;
      case Type::kUnusedMemory: // 01
         qwe |= kTopBitZero;
         length |= kTopBitOne;
         break;
      case Type::kNormal: // 00
         qwe |= kTopBitZero;
         length |= kTopBitZero;
         break;
   }
}

uint16_t Slot::getOffset() const
{
   return qwe & ~kTopBitOne;
}

uint16_t Slot::getLength() const
{
   return length & ~kTopBitOne;
}

void Slot::setOffset(uint16_t offsetInput)
{
   assert(offsetInput != 0);
   assert(offsetInput == (~kTopBitOne&offsetInput));
   qwe = offsetInput;
}

void Slot::setLength(uint16_t lengthInput)
{
   assert(lengthInput <= length && lengthInput != 0);
   assert(lengthInput == (~kTopBitOne&lengthInput));
   length = lengthInput;
}

bool Slot::isRedirectedFromOtherPage() const
{
   return (qwe&kTopBitOne)==kTopBitOne && (length&kTopBitOne)==kTopBitOne;
}

bool Slot::isRedirectedToOtherPage() const
{
   return (qwe&kTopBitOne)==kTopBitOne && (length&kTopBitOne)==kTopBitZero;
}

bool Slot::isMemoryUnused() const
{
   return (qwe&kTopBitOne)==kTopBitZero && (length&kTopBitOne)==kTopBitOne;
}

bool Slot::isNormal() const
{
   return (qwe&kTopBitOne)==kTopBitZero && (length&kTopBitOne)==kTopBitZero;
}

}
