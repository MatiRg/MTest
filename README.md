# MTest 
MTest is small, quick and dirty, header-only C++17 Unit Test library inspired by Catch2.
## Installing
Copy MTest.hpp to your source dir.
## Example
```C++
#include "MTest.hpp"
/// Create Unit Test: Name and Section
UNIT_TEST2(Vector3, Adv)
{
    int a = 2, b = 3;
    /// CHECK_x - If it will Fail, test will continue execution
    /// CHECK_TRUE - must evaluate to `true` expression
    CHECK_TRUE( a == 1 );
    CHECK_TRUE( b == 3 );
    /// CHECK_FALSE - must evaluate to `false` expression
    CHECK_FALSE( b == 2 );
    /// Print Info to Console
    INFO( "Hello - 2" );
}
/// Create Unit Test: Name
UNIT_TEST(Abs)
{
    int a = 3;
    /// Stops Unit test Execution when fails. Must evaluate to true expression.
    ASSERT_TRUE( a == 0 );
}
/// Implement main()
MTEST_MAIN
```
## License
See [UNLICENSE.txt](UNLICENSE.txt)