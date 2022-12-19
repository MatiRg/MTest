/*
MIT License

Copyright (c) 2022 Mateusz Rugor

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
class CMathOperationsFixture: public MTest::IFixture
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
//! Fixture name is created as follows: 'C' + MathOperations + 'Fixture'
MTEST_UNIT_TEST_F(MathOperations, CheckSomething)
{
    MTEST_ASSERT(MyField == 6);
    int* Tmp = nullptr;
    MTEST_LOG << "Checking for NULL" << MTEST_NEW_LINE;
    MTEST_ASSERT_NULL(Tmp);
}

//! Similar as above
MTEST_UNIT_TEST_FX(MathOperations, CheckSomething2, CMathOperationsFixture)
{
    int b = 3;
    MTEST_ASSERT(MyField == 6);
    int* Tmp = &b;
    MTEST_ASSERT_NOT_NULL(Tmp);
    MTEST_ASSERT( MyField != *Tmp );
}

//! Implements main() function
MTEST_MAIN

//! User can also implement own main function.
/*
int main(int, char*[])
{
    // Some logic here
    MTest::CLog::Instance().AddWriter<MTest::CStdWriter>();
    MTEST_IMPLEMENT_MAIN; //! Will run test application.
    // ... and some logic here
	return 0;
}
*/