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

/// Create Unit Test: Name and Section
UNIT_TEST2(Vector3, Basic)
{
    INFO( "Hello" ); /// Print Info
}

UNIT_TEST2(Vector3, Adv)
{
    int a = 2, b = 3;
    CHECK_TRUE( a == 1 ); /// If Fails test will continue
    CHECK_TRUE( b == 3 );
    INFO( "Hello - 2" );
}

UNIT_TEST2(Abs, OnlyAssert)
{
    int a = 3;
    ASSERT_TRUE( a == 0 ); /// If Fails test will abort
}

/// Create Unit Test: Name
UNIT_TEST(Abs)
{
    int a = 3, b = 2;
    ASSERT_TRUE( a != 0 );
    int c = 1;
    CHECK_TRUE( b == c );
    CHECK_TRUE( a == 3 );
    CHECK_FALSE( b == 2 );
    int d = 7;
    ASSERT_FALSE( d != 0 );
    ASSERT_TRUE( a == 0 );
}

/// Implement main()
MTEST_MAIN
