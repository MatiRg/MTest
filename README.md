# MTest 
MTest is small, quick and dirty, header-only C++17 Unit Test library inspired by Catch2.
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
    MTEST_INFO( "Hello World" );
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
class CTestFixture: public MTest::IFixture
{
public:
    //! Can be used to init some data. Return false if something goes wrong.
    bool Setup() override
    {
        MTEST_LOG << "Setup User Data" << MTEST_NEW_LINE;
        MyField = 6;
        return true;
    }

    //! Perform cleanup if needed.
    void Cleanup() override
    {
        MTEST_LOG << "Cleanup User Data" << MTEST_NEW_LINE;
    }
protected:
    //! Fields, Methods can be accessed in test case unless they are marked private.
    int MyField;
};

//! Create test case with user fixture.
MTEST_UNIT_TEST_F(Advanced, CheckSomething, CTestFixture)
{
    MTEST_ASSERT(MyField == 6);
}

//! Implements main() function
MTEST_MAIN

//! User can also implement own main function.
int main(int, char*[])
{
    // Some logic here
    MTEST_IMPLEMENT_MAIN; //! Will run test application.
    // ... and some logic here
	return 0;
}
```
## License
See [UNLICENSE.txt](UNLICENSE.txt)