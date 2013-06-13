#include "test/TestConfig.hpp"
#include "gtest/gtest.h"
#include "harriet/Value.hpp"
#include "harriet/ScriptLanguage.hpp"
#include <string>

//using namespace dbi;
using namespace std;
using namespace harriet;

namespace {
    // Raw input values
    // Requirements:
        // int1 must match float1
        // int1 must not match float2
        // bool1 must not match bool2
        // string1 must not match string2
    int int1 = 5;
    int int2 = 10;
    int zeroInt = 0;
    float float1 = 5.0f;
    float float2 = 7.1f;
    float zeroFloat = 0.0f;
    string string1 = string("Hello");
    string string2 = string("world");
    bool bool1 = true;
    bool bool2 = false;
    // Input value objects
    Value intValue1 = Value::createInteger(int1);
    Value intValue2 = Value::createInteger(int2);
    Value zeroIntValue = Value::createInteger(zeroInt);
    Value floatValue1 = Value::createFloat(float1);
    Value floatValue2 = Value::createFloat(float2);
    Value zeroFloatValue = Value::createFloat(zeroFloat);
    Value charValue1 = Value::createCharacter(string1, string1.size());
    Value charValue2 = Value::createCharacter(string2, string2.size());
    Value boolValue1 = Value::createBool(bool1);
    Value boolValue2 = Value::createBool(bool2);
    // Expected result types
    VariableType intResType = VariableType::createIntegerType();
    VariableType floatResType = VariableType::createFloatType();
    VariableType charResType = VariableType::createCharacterType(string1.size() + string2.size());
    VariableType boolResType = VariableType::createBoolType();
}

TEST(Value, CharValueTests) {
    // char + char
    Value addCharRes = charValue1.computeAdd(charValue2);
    ASSERT_EQ(addCharRes.type, charResType);
    ASSERT_EQ(addCharRes.str(), string1 + string2);
    // char + (int | float | bool) -> exception
    ASSERT_THROW(charValue1.computeAdd(intValue1), Exception);
    ASSERT_THROW(charValue1.computeAdd(floatValue1), Exception);
    ASSERT_THROW(charValue1.computeAdd(boolValue1), Exception);

    // char - (int | float | char | bool ) -> exception
    ASSERT_THROW(charValue1.computeSub(intValue1), Exception);
    ASSERT_THROW(charValue1.computeSub(floatValue1), Exception);
    ASSERT_THROW(charValue1.computeSub(charValue2), Exception);
    ASSERT_THROW(charValue1.computeSub(boolValue1), Exception);


    // char * (int | float | char | bool ) -> exception
    ASSERT_THROW(charValue1.computeMul(intValue1), Exception);
    ASSERT_THROW(charValue1.computeMul(floatValue1), Exception);
    ASSERT_THROW(charValue1.computeMul(charValue2), Exception);
    ASSERT_THROW(charValue1.computeMul(boolValue1), Exception);

    // char / (int | float | char | bool ) -> exception
    ASSERT_THROW(charValue1.computeDiv(intValue1), Exception);
    ASSERT_THROW(charValue1.computeDiv(floatValue1), Exception);
    ASSERT_THROW(charValue1.computeDiv(charValue2), Exception);
    ASSERT_THROW(charValue1.computeDiv(boolValue1), Exception);

    // char != char
    Value neqCharRes = charValue1.computeEq(charValue2);
    ASSERT_EQ(neqCharRes.type, boolResType);
    ASSERT_FALSE(neqCharRes.data.vbool);
    // char == char
    Value eqCharRes = charValue1.computeEq(charValue1);
    ASSERT_EQ(eqCharRes.type, boolResType);
    ASSERT_TRUE(eqCharRes.data.vbool);
    // char == (int | float | bool) -> exception
    ASSERT_THROW(charValue1.computeEq(intValue1), Exception);
    ASSERT_THROW(charValue1.computeEq(floatValue1), Exception);
    ASSERT_THROW(charValue1.computeEq(boolValue1), Exception);
    

}

TEST(Value, BoolValueTests) {
    // bool + (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue1.computeAdd(intValue1), Exception);
    ASSERT_THROW(boolValue1.computeAdd(floatValue1), Exception);
    ASSERT_THROW(boolValue1.computeAdd(charValue1), Exception);
    ASSERT_THROW(boolValue1.computeAdd(boolValue2), Exception);

    // bool - (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue1.computeSub(intValue1), Exception);
    ASSERT_THROW(boolValue1.computeSub(floatValue1), Exception);
    ASSERT_THROW(boolValue1.computeSub(charValue1), Exception);
    ASSERT_THROW(boolValue1.computeSub(boolValue2), Exception);

    // bool * (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue1.computeMul(intValue1), Exception);
    ASSERT_THROW(boolValue1.computeMul(floatValue1), Exception);
    ASSERT_THROW(boolValue1.computeMul(charValue1), Exception);
    ASSERT_THROW(boolValue1.computeMul(boolValue2), Exception);

    // bool / (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue1.computeDiv(intValue1), Exception);
    ASSERT_THROW(boolValue1.computeDiv(floatValue1), Exception);
    ASSERT_THROW(boolValue1.computeDiv(charValue1), Exception);
    ASSERT_THROW(boolValue1.computeDiv(boolValue2), Exception);

    // bool != bool
    Value neqBoolRes = boolValue1.computeEq(boolValue2);
    ASSERT_EQ(neqBoolRes.type, boolResType);
    ASSERT_FALSE(neqBoolRes.data.vbool);
    // bool == bool
    Value eqBoolRes = boolValue1.computeEq(boolValue1);
    ASSERT_EQ(eqBoolRes.type, boolResType);
    ASSERT_TRUE(eqBoolRes.data.vbool);
    // bool == (int | float | char) -> exception
    ASSERT_THROW(boolValue1.computeEq(intValue1), Exception);
    ASSERT_THROW(boolValue1.computeEq(floatValue1), Exception);
    ASSERT_THROW(boolValue1.computeEq(charValue1), Exception);
}

TEST(Value, IntegerValueTests) {
    // int + int
    Value addIntRes = intValue1.computeAdd(intValue2);
    ASSERT_EQ(addIntRes.type, intResType);
    ASSERT_EQ(addIntRes.data.vint, int1 + int2);
    // int + float
    Value addFloatRes = intValue1.computeAdd(floatValue1);
    ASSERT_EQ(addFloatRes.type, floatResType);
    ASSERT_EQ(addFloatRes.data.vfloat, int1 + float1);
    // int + (char | bool) -> exception
    ASSERT_THROW(intValue1.computeAdd(charValue1), Exception);
    ASSERT_THROW(intValue1.computeAdd(boolValue1), Exception);

    // int - int
    Value subIntRes = intValue1.computeSub(intValue2);
    ASSERT_EQ(subIntRes.type, intResType);
    ASSERT_EQ(subIntRes.data.vint, int1 - int2);
    // int - float
    Value subFloatRes = intValue1.computeSub(floatValue1);
    ASSERT_EQ(subFloatRes.type, floatResType);
    ASSERT_EQ(subFloatRes.data.vfloat, int1 - float1);
    // int - (char | bool) -> exception
    ASSERT_THROW(intValue1.computeSub(charValue1), Exception);
    ASSERT_THROW(intValue1.computeSub(boolValue1), Exception);

    // int * int
    Value mulIntRes = intValue1.computeMul(intValue2);
    ASSERT_EQ(mulIntRes.type, intResType);
    ASSERT_EQ(mulIntRes.data.vint, int1 * int2);
    // int * float
    Value mulFloatRes = intValue1.computeMul(floatValue1);
    ASSERT_EQ(mulFloatRes.type, floatResType);
    ASSERT_EQ(mulFloatRes.data.vfloat, int1 * float1);
    // int * (char | bool) -> exception
    ASSERT_THROW(intValue1.computeMul(charValue1), Exception);
    ASSERT_THROW(intValue1.computeMul(boolValue1), Exception);

    // int / int (non-zero)
    Value divIntRes = intValue1.computeDiv(intValue2);
    ASSERT_EQ(divIntRes.type, intResType);
    ASSERT_EQ(divIntRes.data.vint, int1 / int2);
    // int / float (non-zero)
    Value divFloatRes = intValue1.computeDiv(floatValue1);
    ASSERT_EQ(divFloatRes.type, floatResType);
    ASSERT_EQ(divFloatRes.data.vfloat, int1 / float1);
    // int / ((int | float)) 0 -> exception
    ASSERT_THROW(intValue1.computeDiv(zeroIntValue), Exception);
    ASSERT_THROW(intValue1.computeDiv(zeroFloatValue), Exception);
    // int / (char | bool) -> exception
    ASSERT_THROW(intValue1.computeDiv(charValue1), Exception);
    ASSERT_THROW(intValue1.computeDiv(boolValue1), Exception);

    // int != int
    Value neqIntRes = intValue1.computeEq(intValue2);
    ASSERT_EQ(neqIntRes.type, boolResType);
    ASSERT_FALSE(neqIntRes.data.vbool);
    // int == int
    Value eqIntRes = intValue1.computeEq(intValue1);
    ASSERT_EQ(eqIntRes.type, boolResType);
    ASSERT_TRUE(eqIntRes.data.vbool);
    // int != float
    Value neqFloatRes = intValue1.computeEq(floatValue2);
    ASSERT_EQ(neqFloatRes.type, boolResType);
    ASSERT_FALSE(neqFloatRes.data.vbool);
    // int == float
    Value eqFloatRes = intValue1.computeEq(floatValue1);
    ASSERT_EQ(eqFloatRes.type, boolResType);
    ASSERT_TRUE(eqFloatRes.data.vbool);
    // int == (char | bool) -> exception
    ASSERT_THROW(intValue1.computeEq(charValue1), Exception);
    ASSERT_THROW(intValue1.computeEq(boolValue1), Exception);

}

TEST(Value, FloatValueTest) {
    // float + int
    Value addIntRes = floatValue1.computeAdd(intValue2);
    ASSERT_EQ(addIntRes.type, floatResType);
    ASSERT_EQ(addIntRes.data.vfloat, float1 + int2);
    // float + float
    Value addFloatRes = floatValue1.computeAdd(floatValue2);
    ASSERT_EQ(addFloatRes.type, floatResType);
    ASSERT_EQ(addFloatRes.data.vfloat, float1 + float2);
    // float + (char | bool) -> exception
    ASSERT_THROW(floatValue1.computeAdd(charValue1), Exception);
    ASSERT_THROW(floatValue1.computeAdd(boolValue1), Exception);

    // float - int
    Value subIntRes = floatValue1.computeSub(intValue2);
    ASSERT_EQ(subIntRes.type, floatResType);
    ASSERT_EQ(subIntRes.data.vfloat, float1 - int2);
    // float - float
    Value subFloatRes = floatValue1.computeSub(floatValue2);
    ASSERT_EQ(subFloatRes.type, floatResType);
    ASSERT_EQ(subFloatRes.data.vfloat, float1 - float2);
    // float - (char | bool) -> exception
    ASSERT_THROW(floatValue1.computeSub(charValue1), Exception);
    ASSERT_THROW(floatValue1.computeSub(boolValue1), Exception);

    // float * int
    Value mulIntRes = floatValue1.computeMul(intValue2);
    ASSERT_EQ(mulIntRes.type, floatResType);
    ASSERT_EQ(mulIntRes.data.vfloat, float1 * int2);
    // float * float
    Value mulFloatRes = floatValue1.computeMul(floatValue2);
    ASSERT_EQ(mulFloatRes.type, floatResType);
    ASSERT_EQ(mulFloatRes.data.vfloat, float1 * float2);
    // float * (char | bool) -> exception
    ASSERT_THROW(floatValue1.computeMul(charValue1), Exception);
    ASSERT_THROW(floatValue1.computeMul(boolValue1), Exception);

    // float / int (non-zero)
    Value divIntRes = floatValue1.computeDiv(intValue2);
    ASSERT_EQ(divIntRes.type, floatResType);
    ASSERT_EQ(divIntRes.data.vfloat, float1 / int2);
    // float / float (non-zero)
    Value divFloatRes = floatValue1.computeDiv(floatValue2);
    ASSERT_EQ(divFloatRes.type, floatResType);
    ASSERT_EQ(divFloatRes.data.vfloat, float1 / float2);
    // float / ((float | float)) 0 -> exception
    ASSERT_THROW(floatValue1.computeDiv(zeroIntValue), Exception);
    ASSERT_THROW(floatValue1.computeDiv(zeroFloatValue), Exception);
    // float / (char | bool) -> exception
    ASSERT_THROW(floatValue1.computeDiv(charValue1), Exception);
    ASSERT_THROW(floatValue1.computeDiv(boolValue1), Exception);

    // float != int
    Value neqIntRes = floatValue1.computeEq(intValue2);
    ASSERT_EQ(neqIntRes.type, boolResType);
    ASSERT_FALSE(neqIntRes.data.vbool);
    // float == int
    Value eqIntRes = floatValue1.computeEq(intValue1);
    ASSERT_EQ(eqIntRes.type, boolResType);
    ASSERT_TRUE(eqIntRes.data.vbool);
    // float != float
    Value neqFloatRes = floatValue1.computeEq(floatValue2);
    ASSERT_EQ(neqFloatRes.type, boolResType);
    ASSERT_FALSE(neqFloatRes.data.vbool);
    // float == float
    Value eqFloatRes = floatValue1.computeEq(floatValue1);
    ASSERT_EQ(eqFloatRes.type, boolResType);
    ASSERT_TRUE(eqFloatRes.data.vbool);
    // float == (char | bool) -> exception
    ASSERT_THROW(floatValue1.computeEq(charValue1), Exception);
    ASSERT_THROW(floatValue1.computeEq(boolValue1), Exception);
}