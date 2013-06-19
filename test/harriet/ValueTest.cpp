#include "test/TestConfig.hpp"
#include "gtest/gtest.h"
#include "harriet/Value.hpp"
#include "harriet/ScriptLanguage.hpp"
#include <string>

//using namespace dbi;
using namespace std;
using namespace harriet;

namespace {
    // be careful when changing these values: many tests depend on them
    int int_5 = 5;
    int int_10 = 10;
    int int_0 = 0;
    float float_5_0 = 5.0f;
    float float_7_1 = 7.1f;
    float float_0_0 = 0.0f;
    string string_Hello = string("Hello");
    string string_world = string("world");
    string string_worldx = string("worldx");
    bool bool_true = true;
    bool bool_false = false;
    // Input value objects
    Value intValue_5 = Value::createInteger(int_5);
    Value intValue_10 = Value::createInteger(int_10);
    Value intValue_0 = Value::createInteger(int_0);
    Value floatValue_5_0 = Value::createFloat(float_5_0);
    Value floatValue_7_1 = Value::createFloat(float_7_1);
    Value floatValue_0_0 = Value::createFloat(float_0_0);
    Value charValue_Hello = Value::createCharacter(string_Hello, string_Hello.size());
    Value charValue_world = Value::createCharacter(string_world, string_world.size());
    Value charValue_worldx = Value::createCharacter(string_worldx, string_worldx.size());
    Value boolValue_true = Value::createBool(bool_true);
    Value boolValue_false = Value::createBool(bool_false);
    // Expected result types
    VariableType intResType = VariableType::createIntegerType();
    VariableType floatResType = VariableType::createFloatType();
    VariableType charResType = VariableType::createCharacterType(string_Hello.size() + string_world.size());
    VariableType boolResType = VariableType::createBoolType();
}

TEST(Value, CharValueTests) {
    // char + char
    Value addCharRes = charValue_Hello.computeAdd(charValue_world);
    ASSERT_EQ(addCharRes.type, charResType);
    ASSERT_EQ(addCharRes.str(), string_Hello + string_world);
    // char + (int | float | bool) -> exception
    ASSERT_THROW(charValue_Hello.computeAdd(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeAdd(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeAdd(boolValue_true), Exception);

    // char - (int | float | char | bool ) -> exception
    ASSERT_THROW(charValue_Hello.computeSub(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeSub(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeSub(charValue_world), Exception);
    ASSERT_THROW(charValue_Hello.computeSub(boolValue_true), Exception);

    // char * (int | float | char | bool ) -> exception
    ASSERT_THROW(charValue_Hello.computeMul(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeMul(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeMul(charValue_world), Exception);
    ASSERT_THROW(charValue_Hello.computeMul(boolValue_true), Exception);

    // char / (int | float | char | bool ) -> exception
    ASSERT_THROW(charValue_Hello.computeDiv(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeDiv(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeDiv(charValue_world), Exception);
    ASSERT_THROW(charValue_Hello.computeDiv(boolValue_true), Exception);

    // char != char
    Value neqCharRes = charValue_Hello.computeEq(charValue_world);
    ASSERT_EQ(neqCharRes.type, boolResType);
    ASSERT_FALSE(neqCharRes.data.vbool);
    // char == char
    Value eqCharRes = charValue_Hello.computeEq(charValue_Hello);
    ASSERT_EQ(eqCharRes.type, boolResType);
    ASSERT_TRUE(eqCharRes.data.vbool);
    // char == (int | float | bool) -> exception
    ASSERT_THROW(charValue_Hello.computeEq(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeEq(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeEq(boolValue_true), Exception);
    
    // char && (int | float | char | bool) -> exception
    ASSERT_THROW(charValue_Hello.computeAnd(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeAnd(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeAnd(charValue_Hello), Exception);
    ASSERT_THROW(charValue_Hello.computeAnd(boolValue_true), Exception);
    
    // char <= char
    Value leqCharRes1 = charValue_Hello.computeLeq(charValue_world);
    ASSERT_EQ(leqCharRes1.type, boolResType);
    ASSERT_TRUE(leqCharRes1.data.vbool);
    Value leqCharRes2 = charValue_world.computeLeq(charValue_worldx);
    ASSERT_EQ(leqCharRes2.type, boolResType);
    ASSERT_TRUE(leqCharRes2.data.vbool);
    Value leqCharRes3 = charValue_Hello.computeLeq(charValue_Hello);
    ASSERT_EQ(leqCharRes3.type, boolResType);
    ASSERT_TRUE(leqCharRes3.data.vbool);
    // char !<= char
    Value nleqCharRes1 = charValue_world.computeLeq(charValue_Hello);
    ASSERT_EQ(nleqCharRes1.type, boolResType);
    ASSERT_FALSE(nleqCharRes1.data.vbool);
    Value nleqCharRes2 = charValue_worldx.computeLeq(charValue_world);
    ASSERT_EQ(nleqCharRes2.type, boolResType);
    ASSERT_FALSE(nleqCharRes2.data.vbool);
    // char <= (int | float | bool) -> exception
    ASSERT_THROW(charValue_Hello.computeLeq(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeLeq(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeLeq(boolValue_false), Exception);
    
    // char >= char
    Value geqCharRes1 = charValue_world.computeGeq(charValue_Hello);
    ASSERT_EQ(geqCharRes1.type, boolResType);
    ASSERT_TRUE(geqCharRes1.data.vbool);
    Value geqCharRes2 = charValue_worldx.computeGeq(charValue_world);
    ASSERT_EQ(geqCharRes2.type, boolResType);
    ASSERT_TRUE(geqCharRes2.data.vbool);
    Value geqCharRes3 = charValue_Hello.computeGeq(charValue_Hello);
    ASSERT_EQ(geqCharRes3.type, boolResType);
    ASSERT_TRUE(geqCharRes3.data.vbool);
    // char !>= char
    Value ngeqCharRes1 = charValue_Hello.computeGeq(charValue_world);
    ASSERT_EQ(ngeqCharRes1.type, boolResType);
    ASSERT_FALSE(ngeqCharRes1.data.vbool);
    Value ngeqCharRes2 = charValue_world.computeGeq(charValue_worldx);
    ASSERT_EQ(ngeqCharRes2.type, boolResType);
    ASSERT_FALSE(ngeqCharRes2.data.vbool);
    // char >= (int | float | bool) -> exception
    ASSERT_THROW(charValue_Hello.computeGeq(intValue_5), Exception);
    ASSERT_THROW(charValue_Hello.computeGeq(floatValue_5_0), Exception);
    ASSERT_THROW(charValue_Hello.computeGeq(boolValue_false), Exception);
}

TEST(Value, BoolValueTests) {
    // bool + (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue_true.computeAdd(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeAdd(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeAdd(charValue_Hello), Exception);
    ASSERT_THROW(boolValue_true.computeAdd(boolValue_false), Exception);

    // bool - (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue_true.computeSub(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeSub(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeSub(charValue_Hello), Exception);
    ASSERT_THROW(boolValue_true.computeSub(boolValue_false), Exception);

    // bool * (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue_true.computeMul(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeMul(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeMul(charValue_Hello), Exception);
    ASSERT_THROW(boolValue_true.computeMul(boolValue_false), Exception);

    // bool / (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue_true.computeDiv(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeDiv(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeDiv(charValue_Hello), Exception);
    ASSERT_THROW(boolValue_true.computeDiv(boolValue_false), Exception);

    // bool != bool
    Value neqBoolRes = boolValue_true.computeEq(boolValue_false);
    ASSERT_EQ(neqBoolRes.type, boolResType);
    ASSERT_FALSE(neqBoolRes.data.vbool);
    // bool == bool
    Value eqBoolRes = boolValue_true.computeEq(boolValue_true);
    ASSERT_EQ(eqBoolRes.type, boolResType);
    ASSERT_TRUE(eqBoolRes.data.vbool);
    // bool == (int | float | char) -> exception
    ASSERT_THROW(boolValue_true.computeEq(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeEq(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeEq(charValue_Hello), Exception);
    
    // bool && bool
    Value andBoolRes = boolValue_true.computeAnd(boolValue_false);
    ASSERT_EQ(andBoolRes.type, boolResType);
    ASSERT_FALSE(andBoolRes.data.vbool);
    // bool && (int | float | char) -> exception
    ASSERT_THROW(boolValue_true.computeAnd(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeAnd(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeAnd(charValue_Hello), Exception);
    
    // bool <= (int | float | char | bool -> exception
    ASSERT_THROW(boolValue_true.computeLeq(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeLeq(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeLeq(charValue_Hello), Exception);
    ASSERT_THROW(boolValue_false.computeLeq(boolValue_true), Exception);
    
    //bool >= (int | float | char | bool) -> exception
    ASSERT_THROW(boolValue_true.computeGeq(intValue_5), Exception);
    ASSERT_THROW(boolValue_true.computeGeq(floatValue_5_0), Exception);
    ASSERT_THROW(boolValue_true.computeGeq(charValue_Hello), Exception);
    ASSERT_THROW(boolValue_false.computeGeq(boolValue_true), Exception);
}

TEST(Value, IntegerValueTests) {
    // int + int
    Value addIntRes = intValue_5.computeAdd(intValue_10);
    ASSERT_EQ(addIntRes.type, intResType);
    ASSERT_EQ(addIntRes.data.vint, int_5 + int_10);
    // int + float
    Value addFloatRes = intValue_5.computeAdd(floatValue_5_0);
    ASSERT_EQ(addFloatRes.type, floatResType);
    ASSERT_EQ(addFloatRes.data.vfloat, int_5 + float_5_0);
    // int + (char | bool) -> exception
    ASSERT_THROW(intValue_5.computeAdd(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeAdd(boolValue_true), Exception);

    // int - int
    Value subIntRes = intValue_5.computeSub(intValue_10);
    ASSERT_EQ(subIntRes.type, intResType);
    ASSERT_EQ(subIntRes.data.vint, int_5 - int_10);
    // int - float
    Value subFloatRes = intValue_5.computeSub(floatValue_5_0);
    ASSERT_EQ(subFloatRes.type, floatResType);
    ASSERT_EQ(subFloatRes.data.vfloat, int_5 - float_5_0);
    // int - (char | bool) -> exception
    ASSERT_THROW(intValue_5.computeSub(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeSub(boolValue_true), Exception);

    // int * int
    Value mulIntRes = intValue_5.computeMul(intValue_10);
    ASSERT_EQ(mulIntRes.type, intResType);
    ASSERT_EQ(mulIntRes.data.vint, int_5 * int_10);
    // int * float
    Value mulFloatRes = intValue_5.computeMul(floatValue_5_0);
    ASSERT_EQ(mulFloatRes.type, floatResType);
    ASSERT_EQ(mulFloatRes.data.vfloat, int_5 * float_5_0);
    // int * (char | bool) -> exception
    ASSERT_THROW(intValue_5.computeMul(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeMul(boolValue_true), Exception);

    // int / int (non-zero)
    Value divIntRes = intValue_5.computeDiv(intValue_10);
    ASSERT_EQ(divIntRes.type, intResType);
    ASSERT_EQ(divIntRes.data.vint, int_5 / int_10);
    // int / float (non-zero)
    Value divFloatRes = intValue_5.computeDiv(floatValue_5_0);
    ASSERT_EQ(divFloatRes.type, floatResType);
    ASSERT_EQ(divFloatRes.data.vfloat, int_5 / float_5_0);
    // int / ((int | float)) 0 -> exception
    ASSERT_THROW(intValue_5.computeDiv(intValue_0), Exception);
    ASSERT_THROW(intValue_5.computeDiv(floatValue_0_0), Exception);
    // int / (char | bool) -> exception
    ASSERT_THROW(intValue_5.computeDiv(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeDiv(boolValue_true), Exception);

    // int != int
    Value neqIntRes = intValue_5.computeEq(intValue_10);
    ASSERT_EQ(neqIntRes.type, boolResType);
    ASSERT_FALSE(neqIntRes.data.vbool);
    // int == int
    Value eqIntRes = intValue_5.computeEq(intValue_5);
    ASSERT_EQ(eqIntRes.type, boolResType);
    ASSERT_TRUE(eqIntRes.data.vbool);
    // int != float
    Value neqFloatRes = intValue_5.computeEq(floatValue_7_1);
    ASSERT_EQ(neqFloatRes.type, boolResType);
    ASSERT_FALSE(neqFloatRes.data.vbool);
    // int == float
    Value eqFloatRes = intValue_5.computeEq(floatValue_5_0);
    ASSERT_EQ(eqFloatRes.type, boolResType);
    ASSERT_TRUE(eqFloatRes.data.vbool);
    // int == (char | bool) -> exception
    ASSERT_THROW(intValue_5.computeEq(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeEq(boolValue_true), Exception);
    
    // int && (int | float | char | bool) -> exception
    ASSERT_THROW(intValue_5.computeAnd(intValue_5), Exception);
    ASSERT_THROW(intValue_5.computeAnd(floatValue_5_0), Exception);
    ASSERT_THROW(intValue_5.computeAnd(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeAnd(boolValue_true), Exception);
    
    // int <= (int | float)
    Value leqIntRes1 = intValue_5.computeLeq(intValue_10);
    ASSERT_EQ(leqIntRes1.type, boolResType);
    ASSERT_TRUE(leqIntRes1.data.vbool);
    Value leqIntRes2 = intValue_5.computeLeq(intValue_5);
    ASSERT_EQ(leqIntRes2.type, boolResType);
    ASSERT_TRUE(leqIntRes2.data.vbool);
    Value leqFloatRes1 = intValue_5.computeLeq(floatValue_7_1);
    ASSERT_EQ(leqFloatRes1.type, boolResType);
    ASSERT_TRUE(leqFloatRes1.data.vbool);
    Value leqFloatRes2 = intValue_5.computeLeq(floatValue_5_0);
    ASSERT_EQ(leqFloatRes2.type, boolResType);
    ASSERT_TRUE(leqFloatRes2.data.vbool);
    // int !<= (int | float)
    Value nleqIntRes1 = intValue_10.computeLeq(intValue_5);
    ASSERT_EQ(nleqIntRes1.type, boolResType);
    ASSERT_FALSE(nleqIntRes1.data.vbool);
    Value nleqFloatRes1 = intValue_10.computeLeq(floatValue_7_1);
    ASSERT_EQ(nleqFloatRes1.type, boolResType);
    ASSERT_FALSE(nleqFloatRes1.data.vbool);
    // int <= (char | bool) -> exception
    ASSERT_THROW(intValue_5.computeLeq(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeLeq(boolValue_false), Exception);
    
    // int >= (int | float)
    Value geqIntRes1 = intValue_10.computeGeq(intValue_5);
    ASSERT_EQ(geqIntRes1.type, boolResType);
    ASSERT_TRUE(geqIntRes1.data.vbool);
    Value geqIntRes2 = intValue_5.computeGeq(intValue_5);
    ASSERT_EQ(geqIntRes2.type, boolResType);
    ASSERT_TRUE(geqIntRes2.data.vbool);
    Value geqFloatRes1 = intValue_10.computeGeq(floatValue_7_1);
    ASSERT_EQ(geqFloatRes1.type, boolResType);
    ASSERT_TRUE(geqFloatRes1.data.vbool);
    Value geqFloatRes2 = intValue_5.computeGeq(floatValue_5_0);
    ASSERT_EQ(geqFloatRes2.type, boolResType);
    ASSERT_TRUE(geqFloatRes2.data.vbool);
    // int !>= (int | float)
    Value ngeqIntRes1 = intValue_5.computeGeq(intValue_10);
    ASSERT_EQ(ngeqIntRes1.type, boolResType);
    ASSERT_FALSE(ngeqIntRes1.data.vbool);
    Value ngeqFloatRes1 = intValue_5.computeGeq(floatValue_7_1);
    ASSERT_EQ(ngeqFloatRes1.type, boolResType);
    ASSERT_FALSE(ngeqFloatRes1.data.vbool);
    // int >= (char | bool) -> exception
    ASSERT_THROW(intValue_5.computeGeq(charValue_Hello), Exception);
    ASSERT_THROW(intValue_5.computeGeq(boolValue_false), Exception);
}

TEST(Value, FloatValueTests) {
    // float + int
    Value addIntRes = floatValue_5_0.computeAdd(intValue_10);
    ASSERT_EQ(addIntRes.type, floatResType);
    ASSERT_EQ(addIntRes.data.vfloat, float_5_0 + int_10);
    // float + float
    Value addFloatRes = floatValue_5_0.computeAdd(floatValue_7_1);
    ASSERT_EQ(addFloatRes.type, floatResType);
    ASSERT_EQ(addFloatRes.data.vfloat, float_5_0 + float_7_1);
    // float + (char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeAdd(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeAdd(boolValue_true), Exception);

    // float - int
    Value subIntRes = floatValue_5_0.computeSub(intValue_10);
    ASSERT_EQ(subIntRes.type, floatResType);
    ASSERT_EQ(subIntRes.data.vfloat, float_5_0 - int_10);
    // float - float
    Value subFloatRes = floatValue_5_0.computeSub(floatValue_7_1);
    ASSERT_EQ(subFloatRes.type, floatResType);
    ASSERT_EQ(subFloatRes.data.vfloat, float_5_0 - float_7_1);
    // float - (char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeSub(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeSub(boolValue_true), Exception);

    // float * int
    Value mulIntRes = floatValue_5_0.computeMul(intValue_10);
    ASSERT_EQ(mulIntRes.type, floatResType);
    ASSERT_EQ(mulIntRes.data.vfloat, float_5_0 * int_10);
    // float * float
    Value mulFloatRes = floatValue_5_0.computeMul(floatValue_7_1);
    ASSERT_EQ(mulFloatRes.type, floatResType);
    ASSERT_EQ(mulFloatRes.data.vfloat, float_5_0 * float_7_1);
    // float * (char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeMul(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeMul(boolValue_true), Exception);

    // float / int (non-zero)
    Value divIntRes = floatValue_5_0.computeDiv(intValue_10);
    ASSERT_EQ(divIntRes.type, floatResType);
    ASSERT_EQ(divIntRes.data.vfloat, float_5_0 / int_10);
    // float / float (non-zero)
    Value divFloatRes = floatValue_5_0.computeDiv(floatValue_7_1);
    ASSERT_EQ(divFloatRes.type, floatResType);
    ASSERT_EQ(divFloatRes.data.vfloat, float_5_0 / float_7_1);
    // float / ((float | float)) 0 -> exception
    ASSERT_THROW(floatValue_5_0.computeDiv(intValue_0), Exception);
    ASSERT_THROW(floatValue_5_0.computeDiv(floatValue_0_0), Exception);
    // float / (char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeDiv(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeDiv(boolValue_true), Exception);

    // float != int
    Value neqIntRes = floatValue_5_0.computeEq(intValue_10);
    ASSERT_EQ(neqIntRes.type, boolResType);
    ASSERT_FALSE(neqIntRes.data.vbool);
    // float == int
    Value eqIntRes = floatValue_5_0.computeEq(intValue_5);
    ASSERT_EQ(eqIntRes.type, boolResType);
    ASSERT_TRUE(eqIntRes.data.vbool);
    // float != float
    Value neqFloatRes = floatValue_5_0.computeEq(floatValue_7_1);
    ASSERT_EQ(neqFloatRes.type, boolResType);
    ASSERT_FALSE(neqFloatRes.data.vbool);
    // float == float
    Value eqFloatRes = floatValue_5_0.computeEq(floatValue_5_0);
    ASSERT_EQ(eqFloatRes.type, boolResType);
    ASSERT_TRUE(eqFloatRes.data.vbool);
    // float == (char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeEq(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeEq(boolValue_true), Exception);
    
    // float && (int | float | char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeAnd(intValue_5), Exception);
    ASSERT_THROW(floatValue_5_0.computeAnd(floatValue_5_0), Exception);
    ASSERT_THROW(floatValue_5_0.computeAnd(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeAnd(boolValue_true), Exception);
    
    // int <= (int | float)
    Value leqIntRes1 = floatValue_5_0.computeLeq(intValue_10);
    ASSERT_EQ(leqIntRes1.type, boolResType);
    ASSERT_TRUE(leqIntRes1.data.vbool);
    Value leqIntRes2 = floatValue_5_0.computeLeq(intValue_5);
    ASSERT_EQ(leqIntRes2.type, boolResType);
    ASSERT_TRUE(leqIntRes2.data.vbool);
    Value leqFloatRes1 = floatValue_5_0.computeLeq(floatValue_7_1);
    ASSERT_EQ(leqFloatRes1.type, boolResType);
    ASSERT_TRUE(leqFloatRes1.data.vbool);
    Value leqFloatRes2 = floatValue_5_0.computeLeq(floatValue_5_0);
    ASSERT_EQ(leqFloatRes2.type, boolResType);
    ASSERT_TRUE(leqFloatRes2.data.vbool);
    // float !<= (int | float)
    Value nleqIntRes1 = floatValue_7_1.computeLeq(intValue_5);
    ASSERT_EQ(nleqIntRes1.type, boolResType);
    ASSERT_FALSE(nleqIntRes1.data.vbool);
    Value nleqFloatRes1 = floatValue_7_1.computeLeq(floatValue_5_0);
    ASSERT_EQ(nleqFloatRes1.type, boolResType);
    ASSERT_FALSE(nleqFloatRes1.data.vbool);
    // float <= (char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeLeq(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeLeq(boolValue_false), Exception);
    
    // float >= (int | float)
    Value geqIntRes1 = floatValue_7_1.computeGeq(intValue_5);
    ASSERT_EQ(geqIntRes1.type, boolResType);
    ASSERT_TRUE(geqIntRes1.data.vbool);
    Value geqIntRes2 = floatValue_5_0.computeGeq(intValue_5);
    ASSERT_EQ(geqIntRes2.type, boolResType);
    ASSERT_TRUE(geqIntRes2.data.vbool);
    Value geqFloatRes1 = floatValue_7_1.computeGeq(floatValue_5_0);
    ASSERT_EQ(geqFloatRes1.type, boolResType);
    ASSERT_TRUE(geqFloatRes1.data.vbool);
    Value geqFloatRes2 = floatValue_5_0.computeGeq(floatValue_5_0);
    ASSERT_EQ(geqFloatRes2.type, boolResType);
    ASSERT_TRUE(geqFloatRes2.data.vbool);
    // float !>= (int | float)
    Value ngeqIntRes1 = floatValue_5_0.computeGeq(intValue_10);
    ASSERT_EQ(ngeqIntRes1.type, boolResType);
    ASSERT_FALSE(ngeqIntRes1.data.vbool);
    Value ngeqFloatRes1 = floatValue_5_0.computeGeq(floatValue_7_1);
    ASSERT_EQ(ngeqFloatRes1.type, boolResType);
    ASSERT_FALSE(ngeqFloatRes1.data.vbool);
    // float >= (char | bool) -> exception
    ASSERT_THROW(floatValue_5_0.computeGeq(charValue_Hello), Exception);
    ASSERT_THROW(floatValue_5_0.computeGeq(boolValue_false), Exception);
}