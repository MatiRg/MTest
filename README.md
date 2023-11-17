# MTest 
MTest is small, quick and dirty, header-only C++20 Unit Test library inspired by Catch2 nad gtest.
## Example console output
![alt text](Output.png "Example console (PowerShell) output.") 
## Installing
Copy MTest.hpp to your source dir.
Include header and you are good to go.
## Example
```C++
#include "MTest.hpp"

//! Define test case, give section name and test case name. Tests are executed by section, test case name
//! must be unique in given section.
MTEST_UNIT_TEST(Basic, Hello)
{
    //! Print some information to stdout. 
    MTEST_INFO("Hello World");
    int a = 7;
    //! It must evaluate to true statement, if not test will fail but will continue execution.
    MTEST_CHECK(a == 7);
    int b = 3;
    //! Must evaluate to true statement, if not test will fail and will exit.
    MTEST_ASSERT( (a+b) == 10 );
}

MTEST_UNIT_TEST(Basic, HelloFail)
{
    int a = 7;
    int b = 3;
    //! Must evaluate to false statement, if not test will fail and will exit.
    MTEST_ASSERT_FALSE( (a+b) == 10 );
}

MTEST_UNIT_TEST(Other, MathFail)
{
    int a = 7;
    //! It must evaluate to true statement, if not test will fail but will continue execution.
    MTEST_CHECK_TRUE(a == 7);
    int b = 3;
    //! It must evaluate to false statement, if not test will fail but will continue execution.
    MTEST_CHECK_FALSE(b == 3);
    //! Must evaluate to true statement, if not test will fail and will exit.
    MTEST_ASSERT_TRUE( (a+b) == 10 );
}

//! Create fixture. Fixture is created everytime when test case is run.
struct MathOperationsFixture: public MTest::Fixture
{
    //! Can be used to init some data. Use Assertions to check state.
    void Setup() override
    {
        MTEST_INFO("Setup User Data");
        MyField = 6;
        MTEST_ASSERT_VALUE(MyField, 6);
    }

    //! Perform cleanup if needed.
    void Cleanup() override
    {
        MTEST_INFO("Cleanup User Data");
    }
protected:
    //! Fields, Methods can be accessed in test case unless they are marked private.
    int MyField;
};

//! Create test case with user fixture.
//! Fixture name is created as follows: MathOperations + 'Fixture'
MTEST_UNIT_TEST_F(MathOperations, CheckSomething)
{
    MTEST_ASSERT(MyField == 6);
    int* Tmp = nullptr;
    MTEST_INFO("Checking for NULL");
    MTEST_ASSERT_NULL(Tmp);
}

//! Similar as above
MTEST_UNIT_TEST_FX(MathOperations, CheckSomething2, MathOperationsFixture)
{
    int b = 3;
    MTEST_ASSERT_VALUE(MyField, 6);
    int* Tmp = &b;
    MTEST_ASSERT_NOT_NULL(Tmp);
    MTEST_ASSERT( MyField != *Tmp );
}

//! Test case skip example
MTEST_UNIT_TEST(SimpleSkip, ShowSkip1)
{
    MTEST_SKIP("Not Support on this PC");
    //
    int b = 3;
    MTEST_ASSERT(b == 3);
}

//! Use fixture for this skip
struct AdvancedSkipFixture: public MTest::Fixture
{
    bool Skip() override
    {
        //! Return true if must skip these tests
        return true; 
    }
};

//! Test case skip with Fixture example
MTEST_UNIT_TEST_F(AdvancedSkip, ShowSkip2)
{
    int b = 3;
    MTEST_ASSERT(b == 3);
}

//! Test case with uncaught exception
MTEST_UNIT_TEST(Exceptions, Uncaught)
{
    throw std::runtime_error{"Some exception"};
    //
    bool flag = false;
    MTEST_ASSERT_FALSE(flag);
}

//! Table test data definition
struct MathTestData
{
    std::string Name;
    int Arg1;
    int Arg2;
    int Result;
};
//! Table fixture: Pass test data type and base class(derivied from Fixture)
struct MathTestTableFixture: public MTest::TableFixture<MathTestData, MTest::Fixture>
{
    //! Overwrite to update test case name
    std::string GenerateName(const DataType& item, const std::size_t) override
    {
        return item.Name;
    }

    //! Some helper
    void CheckWork(const int x, const int y, const int r)
    {
        const int z = x+y;
        MTEST_ASSERT_VALUE(z, r);
    }
};
//! Define test array
std::vector<MathTestData> MathTestDataArray = 
{
    {"AddPositives", 3, 3, 6},
    {"AddZeros", 0, 0, 0},
    {"AddMixed", -2, 3, 1},
    {"AddPositives2", 3, 7, 10}
};
//! Define Test Case
MTEST_UNIT_TEST_F_T(MathTest, EnigmaticFunction, MathTestDataArray)
{
    //! Access test data with: testData
    CheckWork(testData.Arg1, testData.Arg2, testData.Result);
}

//! Implements main() function
MTEST_MAIN

//! User can also implement own main function.
/*
int main(int, char*[])
{
    // Some logic here
    MTEST_IMPLEMENT_STD_LOGGER;
    MTEST_IMPLEMENT_MAIN; //! Will run test application.
    // ... and some logic here
	return 0;
}
*/
```
## License
See [LICENSE.txt](LICENSE.txt)