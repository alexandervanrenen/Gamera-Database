#include "ScriptLanguage.hpp"
#include "Environment.hpp"
#include "Expression.hpp"
#include "Function.hpp"
#include "Utility.hpp"
#include "Value.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <ctype.h>
#include <istream>
#include <sstream>
//---------------------------------------------------------------------------
// Harriet Script Language
// Copyright (c) 2013 Alexander van Renen (alexandervanrenen@gmail.com)
// See the file LICENSE.txt for copying permission.
//---------------------------------------------------------------------------
using namespace std;
//---------------------------------------------------------------------------
namespace harriet {
// //---------------------------------------------------------------------------
// unique_ptr<Value> readValueContent(VariableType type, uint16_t length, const char* data) throw()
// {
//    switch(type) {
//       case VariableType::TInteger:
//          return make_unique<IntegerValue>(*reinterpret_cast<const int32_t*>(data));
//       case VariableType::TFloat:
//          return make_unique<FloatValue>(*reinterpret_cast<const float*>(data));
//       case VariableType::TBool:
//          return make_unique<BoolValue>(*reinterpret_cast<const bool*>(data));
//       case VariableType::TVector:
//          return make_unique<VectorValue>(*reinterpret_cast<const Vector3<float>*>(data));
//       case VariableType::TCharacter: {
//          vector<char> result(length);
//          memcpy(result.data(), data, length);
//          return make_unique<CharacterValue>(result, length);
//       }
//    }
//    throw Exception{"unreachable"};
// }
// //---------------------------------------------------------------------------
// void writeValueContent(const Value& value, char* data) throw()
// {
//    switch(value.getResultType()) {
//       case VariableType::TInteger:
//          memcpy(data, &reinterpret_cast<const IntegerValue&>(value).result, sizeof(int32_t));
//          return;
//       case VariableType::TFloat:
//          memcpy(data, &reinterpret_cast<const FloatValue&>(value).result, sizeof(float));
//          return;
//       case VariableType::TBool:
//          memcpy(data, &reinterpret_cast<const BoolValue&>(value).result, sizeof(bool));
//          return;
//       case VariableType::TVector:
//          memcpy(data, &reinterpret_cast<const VectorValue&>(value).result, sizeof(Vector3<float>));
//          return;
//       case VariableType::TCharacter:
//          memcpy(data, reinterpret_cast<const CharacterValue&>(value).result.data(), reinterpret_cast<const CharacterValue&>(value).result.size());
//          return;
//    }
//    throw Exception{"unreachable"};
// }
//---------------------------------------------------------------------------
bool isImplicitCastPossible(VariableType from, VariableType to) throw()
{
   bool implicitCast[5][5] = {
      /* to\from         bool    float   int     vector  character */
      /* bool      */ {  true ,  false,  false,  false,  false},
      /* int       */ {  false,  true ,  true ,  false,  false},
      /* float     */ {  false,  true ,  true ,  false,  false},
      /* vector    */ {  false,  false,  false,  true ,  false},
      /* charactor */ {  false,  false,  false,  false,  true }
   };

   return implicitCast[static_cast<uint32_t>(to.type)][static_cast<uint32_t>(from.type)];
}
//---------------------------------------------------------------------------
} // end of namespace harriet
//---------------------------------------------------------------------------
