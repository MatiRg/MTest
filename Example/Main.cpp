/*
MIT License

Copyright (c) 2025 Mateusz Rugor

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "../Include/MTest.hpp"

// Define test case, give section name and test case name. Tests are executed by section, test case name
// must be unique in given section.
MTEST_UNIT_TEST(Basic, Hello)
{
    // Print some information to stdout. 
    MTEST_INFO("Hello World");
    int a = 7;
    // Value must be equal, if not test will fail but will continue execution.
    MTEST_CHECK_VALUE(a, 7);
    MTEST_CHECK_TRUE(a == 7);
    int b = 3;
    // Value must be equal, if not test will fail and will exit.
    MTEST_ASSERT_VALUE(a+b, 10);
    // Check floating point is equal (near given value).
    float f = 3.0f;
    MTEST_CHECK_NEAR(f, 3.0f, MTest::EPSILON_SMALL<float>);
    // Compare pointer values.
    int* aa = &a;
    int* bb = &b;
    // Pointers must be equal (it does not compare values).
    MTEST_CHECK_POINTER(aa, &a);
    // Pointers must not be equal (it does not compare values).
    MTEST_CHECK_NOT_POINTER(aa, nullptr);
    MTEST_CHECK_NOT_POINTER(aa, bb);
}

MTEST_UNIT_TEST(Basic, HelloFail)
{
    int a = 7;
    int b = 3;
    // Must not be equal, if not test will fail and will exit.
    MTEST_ASSERT_NOT_VALUE(a+b, 10);
}

MTEST_UNIT_TEST(Other, MathFail)
{
    int a = 7;
    // It must evaluate to true statement, if not test will fail but will continue execution.
    MTEST_CHECK_TRUE(a == 7);
    int b = 3;
    // It must evaluate to false statement, if not test will fail but will continue execution.
    MTEST_CHECK_FALSE(b == 3);
    // Must evaluate to true statement, if not test will fail and will exit.
    MTEST_ASSERT_TRUE(a+b == 10);
}

// Create fixture. Fixture is created everytime when test case is run.
struct MathOperationsFixture: public MTest::Fixture
{
    // Can be used to init some data. Use Assertions to check state.
    void Setup() override
    {
        MTEST_INFO("Setup User Data");
        MyField = 6;
        MTEST_ASSERT_VALUE(MyField, 6);
    }

    // Perform cleanup if needed.
    void Cleanup() override
    {
        MTEST_INFO("Cleanup User Data");
    }
protected:
    // Fields, Methods can be accessed in test case unless they are marked private.
    int MyField;
};

// Create test case with user fixture.
// Fixture name is created as follows: MathOperations + 'Fixture'
MTEST_UNIT_TEST_F(MathOperations, CheckSomething)
{
    MTEST_ASSERT_TRUE(MyField == 6);
    int* Tmp = nullptr;
    MTEST_INFO("Checking for NULL");
    MTEST_ASSERT_NULL(Tmp);
}

// Similar as above
MTEST_UNIT_TEST_FX(MathOperations, CheckSomething2, MathOperationsFixture)
{
    int b = 3;
    MTEST_ASSERT_VALUE(MyField, 6);
    int* Tmp = &b;
    MTEST_ASSERT_NOT_NULL(Tmp);
    MTEST_ASSERT_TRUE(MyField != *Tmp);
}

// Test case skip example
MTEST_UNIT_TEST(SimpleSkip, ShowSkip1)
{
    MTEST_SKIP("Not Support on this PC");
    int b = 3;
    MTEST_ASSERT_FALSE(b == 3);
}

// Use fixture for this skip
struct AdvancedSkipFixture: public MTest::Fixture
{
    bool Skip() override
    {
        // Return true if must skip these tests
        return true; 
    }
};

// Test case skip with Fixture example
MTEST_UNIT_TEST_F(AdvancedSkip, ShowSkip2)
{
    int b = 3;
    MTEST_ASSERT_FALSE(b == 3);
}

// Test case with uncaught exception
MTEST_UNIT_TEST(Exceptions, Uncaught)
{
    throw std::runtime_error{"Some exception"};
    bool flag = false;
    MTEST_ASSERT_FALSE(flag);
}

void ThrowFunction()
{
    throw std::runtime_error{"My exception"};
}

void NoThrowFunction()
{
}

// Test case with exceptions
MTEST_UNIT_TEST(Exceptions, CheckExceptions)
{
    MTEST_CHECK_THROW(ThrowFunction(), std::runtime_error);
    MTEST_CHECK_NO_THROW(NoThrowFunction());
    // These fails
    MTEST_CHECK_THROW(NoThrowFunction(), std::runtime_error);
    MTEST_CHECK_NO_THROW(ThrowFunction()); 
}

// Table test data definition
struct MathTestData
{
    std::string Name;
    int Arg1;
    int Arg2;
    int Result;
};
// Table fixture: Pass test data type and base class(derivied from Fixture)
struct MathTestTableFixture: public MTest::TableFixture<MathTestData, MTest::Fixture>
{
    // Overwrite to update test case name
    std::string GenerateName(const DataType& item, const std::size_t) override
    {
        return item.Name;
    }

    // Some helper
    void CheckWork(const int x, const int y, const int r)
    {
        const int z = x+y;
        MTEST_ASSERT_VALUE(z, r);
    }
};
// Define test array
MTest::TableDataArray<MathTestData> MathTestDataArray = 
{
    {"AddPositives", 3, 3, 6},
    {"AddZeros", 0, 0, 0},
    {"AddMixed", -2, 3, 1},
    {"AddPositives2", 3, 7, 10}
};
// Define Test Case
MTEST_UNIT_TEST_F_T(MathTest, EnigmaticFunction, MathTestDataArray)
{
    // Access test data with: testData
    CheckWork(testData.Arg1, testData.Arg2, testData.Result);
}

// User defined check

// Define check function
template<class T>
MTest::CheckResult MyCheck(const T& value, const T& wanted)
{
    if( value == wanted )
    {
        return MTest::CheckSuccess();
    }
    return MTest::CheckFailure("My check should be {} but is {}", value, wanted);
}
// Define helper
#define MY_CHECK(Value, Wanted) MTEST_CHECK_CUSTOM(MyCheck(Value, Wanted))
#define MY_ASSERT(Value, Wanted) MTEST_ASSERT_CUSTOM(MyCheck(Value, Wanted))

// Define test for user check
MTEST_UNIT_TEST(User, Sample)
{
    int a = 7;
    MY_CHECK(a, 7);
    MY_ASSERT(a, 7);
}

// Implements main() function
MTEST_MAIN

// User can also implement own main function.
/*
int main(int argc, char* argv[])
{
    // Add output - console
    MTEST_CREATE_STD_SINK;
    // ... and some logic here
	return MTEST_RUN_TESTS(argc, argv) ? 0 : 1; // Will run test application.
}
*/
