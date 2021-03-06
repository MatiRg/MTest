/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <https://unlicense.org>
*/
#include "../Include/MTest.hpp"

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
/*int main(int, char*[])
{
    // Some logic here
    MTEST_IMPLEMENT_MAIN; //! Will run test application.
    // ... and some logic here
	return 0;
}
*/