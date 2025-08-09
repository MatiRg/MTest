# MTest 
MTest is small, quick and dirty, header-only C++23 Unit Test library inspired by Catch2 nad gtest.
## Installing
Copy MTest.hpp to your source dir.
After that include header and you are good to go.
## Documentation
### Test basic
First you need to start with including header.
```C++
#include "MTest.hpp"
```
Next step is to define test case. You need to specify Section which is used to group individual tests and unique name - per section.
```C++
MTEST_UNIT_TEST(SectionName, UniqueTestName)
```
After that you can define test body.
```C++
MTEST_UNIT_TEST(SectionName, UniqueTestName)
{
    int a = CalculateSomething();
    MTEST_CHECK_TRUE(a == 7);
    MTEST_CHECK_VALUE(a, 7);
    MTEST_ASSERT_TRUE(a > 0);
}
```
In final step in one source file you need to define `int main(int argc, char* argv[])`, use `MTEST_MAIN` macro.
```C++
MTEST_MAIN
```
Test can use various assertion divide into two groups:
* Check group - these will fail test and continue execution of given test
* Assert group - these will fail test and stop execution of given test

Aviable assertions are showcased here:

| Type | Break execution | Notes |
| - | - | - |
| MTEST_CHECK_TRUE | No | Must evalute to true |
| MTEST_CHECK_FALSE | No | Must evalute to false |
| MTEST_CHECK_VALUE | No | Check if both values are the same |
| MTEST_CHECK_NOT_VALUE | No | Check if both values are different |
| MTEST_CHECK_POINTER | No | Check if both pointers are the same |
| MTEST_CHECK_NOT_POINTER | No | Check if both pointers are different |
| MTEST_CHECK_NULL | No | Check if pointer is null |
| MTEST_CHECK_NOT_NULL | No | Check if pointer is not null |
| MTEST_CHECK_NEAR | No | Check if floating point is near given value |
| MTEST_CHECK_THROW | No | Check if exception of given type is throw |
| MTEST_CHECK_ANY_THROW | No | Check if any exception is throw |
| MTEST_CHECK_NO_THROW | No | Check if no exception is throw |
| MTEST_CHECK_CUSTOM | No | Check for user defined verification |
| MTEST_ASSERT_TRUE | Yes | Must evalute to true |
| MTEST_ASSERT_FALSE | Yes | Must evalute to false |
| MTEST_ASSERT_VALUE | Yes | Check if both values are the same |
| MTEST_ASSERT_NOT_VALUE | Yes | Check if both values are different |
| MTEST_ASSERT_POINTER | Yes | Check if both pointers are the same |
| MTEST_ASSERT_NOT_POINTER | Yes | Check if both pointers are different |
| MTEST_ASSERT_NULL | Yes | Check if pointer is null |
| MTEST_ASSERT_NOT_NULL | Yes | Check if pointer is not null |
| MTEST_ASSERT_NEAR | Yes | Check if floating point is near given value |
| MTEST_ASSERT_THROW | Yes | Check if exception of given type is throw |
| MTEST_ASSERT_ANY_THROW | Yes | Check if any exception is throw |
| MTEST_ASSERT_NO_THROW | Yes | Check if no exception is throw |
| MTEST_ASSERT_CUSTOM | Yes | Check for user defined verification |

### Fixtures
You can use fixture to create fixed environment in which tests are run and also share common helper methods or data. To create fixture you need inherit from `MTest::Fixture`. You can overwrite these methods:

* `bool Skip()` - Use that nethod to skip tests, return `true` if it must be skipped
* `void Setup()` - Use that method to setup test environment. Use Assertions to check state.
* `void Cleanup()` - Use to clear data that must be freed in manual way

To define test with Fixture you can use two macros:

* `MTEST_UNIT_TEST_F(SectionName, UniqueTestName)` - Create test case with fixture that its name is inferred from section name eg. `SectionName + Fixture`.
* `MTEST_UNIT_TEST_FX(SectionName, UniqueTestName, FixtureName)` - Create test case with fixture that is specified by FixtureName.

Example - both declaration define same test case, only their declaration method differs:
```C++
struct DynamicEngineFixture: public MTest::Fixture
{
    bool Skip() override { return true; }
};

MTEST_UNIT_TEST_F(DynamicEngine, CanCalculate) 
{}

MTEST_UNIT_TEST_FX(DynamicEngine, CanCalculate, DynamicEngineFixture)
{}
```
### Test skipping
You can skip test by overriding Fixture method `bool Skip()` and also by using 
`MTEST_SKIP(Why)` in test body.
```C++
MTEST_UNIT_TEST(Fundamentals, SetupEngine)
{
    MTEST_SKIP("Not Supported on this Platform");
}
```
### Trace messages
You can also output trace messages to output with `MTEST_INFO`.
```C++
MTEST_UNIT_TEST(Example, Basic)
{
    MTEST_INFO("Going with code path normal");
}
```
### Table tests
Table tests are test that reuse test body for several test items. First you need to define test data type - do not use heap types or types that require manual setup.
```C++
struct MathTestData
{
    std::string Name;
    int Arg1;
    int Arg2;
    int Result;
};
```
After that you must define special fixture derivied from `MTest::TableFixture`.
Base fixture `MTest::TableFixture` takes two template arguments, first is Data Type - in this case `MathTestData` and second param is base class - any other fixture, we will use `MTest::Fixture`. You can overwrite method `std::string GenerateName(const DataType& item, const std::size_t)` to precise test subname - if you don't do that test will use numbers starting from 0. `DataType` is alias from `MTest::TableFixture` and is same as `MathTestData`.
```C++
struct MathTestTableFixture: public MTest::TableFixture<MathTestData, MTest::Fixture>
{
    std::string GenerateName(const DataType& item, const std::size_t) override
    {
        return item.Name;
    }
    void CheckWork(const int x, const int y, const int r)
    {
        const int z = x+y;
        MTEST_ASSERT_VALUE(z, r);
    }
};
```
Next step is do define test data using `MathTestData` struct and `MTest::TableDataArray` as array.
```C++
MTest::TableDataArray<MathTestData> MathTestDataArray = 
{
    {"AddPositives", 3, 3, 6},
    {"AddZeros", 0, 0, 0},
    {"AddMixed", -2, 3, 1},
    {"AddPositives2", 3, 7, 10}
};
```
Final step is declare test case, you can use two macros:

* `MTEST_UNIT_TEST_F_T(SectionName, UniqueTestName, DataArray)` - Create test case with fixture that its name is inferred from section name eg. `SectionName + TableFixture`.
* `MTEST_UNIT_TEST_FX_T(SectionName, UniqueTestName, DataArray, FixtureName)` - Create test case with fixture that is specified by FixtureName.

```C++
MTEST_UNIT_TEST_F_T(MathTest, EnigmaticFunction, MathTestDataArray)
{
    CheckWork(testData.Arg1, testData.Arg2, testData.Result);
}
```
### User defined check
User can define their own check/assert macro to accommodate custom type. First step is to define check function:
```C++
template<class T>
MTest::CheckResult MyCheck(const T& value, const T& wanted)
{
    if( value == wanted )
    {
        return MTest::CheckSuccess(); // Verification success
    }
    // Verification failure
    return MTest::CheckFailure("My check should be {} but is {}", value, wanted);
}
```
After that best way to use them is to define helper macros:
```C++
#define MY_CHECK(Value, Wanted) MTEST_CHECK_CUSTOM(MyCheck(Value, Wanted))
#define MY_ASSERT(Value, Wanted) MTEST_ASSERT_CUSTOM(MyCheck(Value, Wanted))
```
And finally we can use these new assertions in simple way:
```C++
MTEST_UNIT_TEST(User, Sample)
{
    int a = 7;
    MY_CHECK(a, 7);
    MY_ASSERT(a, 7);
}
```
### Command line options
Test can be skipped (filtered out) by command line: `./Tests.exe -F=Selected` only test that full name contains `Selected` will be run.
## Example output
![alt text](Output.png "Example (Console) output.") 
## Example
See [Main.cpp](Example/Main.cpp)
## License
See [LICENSE.txt](LICENSE.txt)
