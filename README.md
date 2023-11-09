# MTest 
MTest is small, quick and dirty, header-only C++20 Unit Test library inspired by Catch2 nad gtest.
## Example console output
![alt text](ConsoleOutput.JPG "Example console (PowerShell) output.") 
## Installing
Copy MTest.hpp to your source dir.
## Example
```C++
#include "MTest.hpp"

//! Define test case, give section name and test case name. Tests are executed by section, test case name
//! must be unique in given section.
MTEST_UNIT_TEST(Basic, Hello)
{
    //! Print some information to stdout. 
    MTEST_INFO("Hello World\n");
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
class CMathOperationsFixture: public MTest::IFixture
{
public:
    //! Can be used to init some data. Use Assertions to check state.
    void Setup() override
    {
        MTEST_INFO("Setup User Data\n");
        MyField = 6;
        MTEST_ASSERT_VALUE(MyField, 6);
    }

    //! Perform cleanup if needed.
    void Cleanup() override
    {
        MTEST_INFO("Cleanup User Data\n");
    }
protected:
    //! Fields, Methods can be accessed in test case unless they are marked private.
    int MyField;
};

//! Create test case with user fixture.
//! Fixture name is created as follows: 'C' + MathOperations + 'Fixture'
MTEST_UNIT_TEST_F(MathOperations, CheckSomething)
{
    MTEST_ASSERT(MyField == 6);
    int* Tmp = nullptr;
    MTEST_INFO("Checking for NULL\n");
    MTEST_ASSERT_NULL(Tmp);
}

//! Similar as above
MTEST_UNIT_TEST_FX(MathOperations, CheckSomething2, CMathOperationsFixture)
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
class CAdvancedSkipFixture: public MTest::IFixture
{
public:
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