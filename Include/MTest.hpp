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
#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <source_location>
#include <cstdint>
#include <optional>
#include <concepts>
#include <chrono>
#include <numeric>
#include <cstdlib>
#include <cstdio>
#include <format>
#include <cmath>
#include <limits>

#if defined(_WIN64) || defined(_WIN32) || defined(WIN32)
    #define MTEST_WINDOWS_PLATFORM 1
#elif defined(__linux__) || defined(__linux) || defined(linux)
    #define MTEST_LINUX_PLATFORM 1
#else
    #error "Unsupported platform"
#endif

#if !defined(MTEST_CONFIG_NO_COLOR) && defined(MTEST_WINDOWS_PLATFORM)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

//// Utility

#define MTEST_INTERNAL_MACRO_CONCAT(x, y) x##y
/// Needed because __LINE__ or __COUNTER__ itself is a macro so it needs two level expanding.
#define MTEST_MACRO_CONCAT(x, y) MTEST_INTERNAL_MACRO_CONCAT(x, y)

//// Assertions

#define MTEST_INTERNAL_CHECK_TRUE(Condition, Type) MTest::GetTestManager().GetActiveTest()->CheckTrue( Condition, #Condition, Type )
#define MTEST_INTERNAL_CHECK_FALSE(Condition, Type) MTest::GetTestManager().GetActiveTest()->CheckFalse( Condition, #Condition, Type )
#define MTEST_INTERNAL_CHECK_VALUE(Value, Wanted, Type) MTest::GetTestManager().GetActiveTest()->CheckEqual( Value, Wanted, #Value, Type )
#define MTEST_INTERNAL_CHECK_NOT_VALUE(Value, Wanted, Type) MTest::GetTestManager().GetActiveTest()->CheckNotEqual( Value, Wanted, #Value, Type )
#define MTEST_INTERNAL_CHECK_POINTER(Value, Wanted, Type) MTest::GetTestManager().GetActiveTest()->CheckPointer( Value, Wanted, #Value, Type )
#define MTEST_INTERNAL_CHECK_NOT_POINTER(Value, Wanted, Type) MTest::GetTestManager().GetActiveTest()->CheckNotPointer( Value, Wanted, #Value, Type )
#define MTEST_INTERNAL_CHECK_NULL(Value, Type) MTest::GetTestManager().GetActiveTest()->CheckNull( Value, #Value, Type )
#define MTEST_INTERNAL_CHECK_NOT_NULL(Value, Type) MTest::GetTestManager().GetActiveTest()->CheckNotNull( Value, #Value, Type )
#define MTEST_INTERNAL_CHECK_NEAR(Value, Wanted, Epsilon, Type) MTest::GetTestManager().GetActiveTest()->CheckNear( Value, Wanted, Epsilon, #Value, Type )
#define MTEST_INTERNAL_CHECK_THROW(Statement, Exception, Type) MTest::GetTestManager().GetActiveTest()->CheckThrow< Exception >( [&](){ Statement; }, #Statement, #Exception, Type )
#define MTEST_INTERNAL_CHECK_ANY_THROW(Statement, Type) MTest::GetTestManager().GetActiveTest()->CheckAnyThrow( [&](){ Statement; }, #Statement, Type )
#define MTEST_INTERNAL_CHECK_NO_THROW(Statement, Type) MTest::GetTestManager().GetActiveTest()->CheckNoThrow( [&](){ Statement; }, #Statement, Type )
#define MTEST_INTERNAL_CHECK_CUSTOM(Result, Type) MTest::GetTestManager().GetActiveTest()->CheckCustom( Result, Type )

/// It must evaluate to true statement, if not test will fail and continue execution.
#define MTEST_CHECK_TRUE(Condition) MTEST_INTERNAL_CHECK_TRUE(Condition, MTest::EFailType::Check)
/// It must evaluate to false statement, if not test will fail and continue execution.
#define MTEST_CHECK_FALSE(Condition) MTEST_INTERNAL_CHECK_FALSE(Condition, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail and continue execution.
#define MTEST_CHECK_VALUE(Value, Wanted) MTEST_INTERNAL_CHECK_VALUE(Value, Wanted, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail and continue execution.
#define MTEST_CHECK_NOT_VALUE(Value, Wanted) MTEST_INTERNAL_CHECK_NOT_VALUE(Value, Wanted, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail and continue execution.
#define MTEST_CHECK_POINTER(Value, Wanted) MTEST_INTERNAL_CHECK_POINTER(Value, Wanted, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail and continue execution.
#define MTEST_CHECK_NOT_POINTER(Value, Wanted) MTEST_INTERNAL_CHECK_NOT_POINTER(Value, Wanted, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail and continue execution.
#define MTEST_CHECK_NULL(Value) MTEST_INTERNAL_CHECK_NULL(Value, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail and continue execution.
#define MTEST_CHECK_NOT_NULL(Value) MTEST_INTERNAL_CHECK_NOT_NULL(Value, MTest::EFailType::Check)
/// Check if floating point is near given value, if not test will fail and continue execution.
#define MTEST_CHECK_NEAR(Value, Wanted, Epsilon) MTEST_INTERNAL_CHECK_NEAR(Value, Wanted, Epsilon, MTest::EFailType::Check)
/// Check if given exception is throw, if not test will fail and continue execution.
#define MTEST_CHECK_THROW(Statement, Exception) MTEST_INTERNAL_CHECK_THROW(Statement, Exception, MTest::EFailType::Check)
/// Check if any exception is throw, if not test will fail and continue execution.
#define MTEST_CHECK_ANY_THROW(Statement) MTEST_INTERNAL_CHECK_ANY_THROW(Statement, MTest::EFailType::Check)
/// Check if exception is not throw, if not test will fail and continue execution.
#define MTEST_CHECK_NO_THROW(Statement) MTEST_INTERNAL_CHECK_NO_THROW(Statement, MTest::EFailType::Check)
/// User defined check, if not evaluated to true statement test will fail and continue execution.
#define MTEST_CHECK_CUSTOM(Result) MTEST_INTERNAL_CHECK_CUSTOM(Result, MTest::EFailType::Check)

/// Must evaluate to true statement, if not test will fail and abort execution.
#define MTEST_ASSERT_TRUE(Condition) MTEST_INTERNAL_CHECK_TRUE(Condition, MTest::EFailType::Assert)
/// Must evaluate to false statement, if not test will fail and abort execution.
#define MTEST_ASSERT_FALSE(Condition) MTEST_INTERNAL_CHECK_FALSE(Condition, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and abort execution.
#define MTEST_ASSERT_VALUE(Value, Wanted) MTEST_INTERNAL_CHECK_VALUE(Value, Wanted, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and abort execution.
#define MTEST_ASSERT_NOT_VALUE(Value, Wanted) MTEST_INTERNAL_CHECK_NOT_VALUE(Value, Wanted, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and abort execution.
#define MTEST_ASSERT_POINTER(Value, Wanted) MTEST_INTERNAL_CHECK_POINTER(Value, Wanted, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and abort execution.
#define MTEST_ASSERT_NOT_POINTER(Value, Wanted) MTEST_INTERNAL_CHECK_NOT_POINTER(Value, Wanted, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and abort execution.
#define MTEST_ASSERT_NULL(Value) MTEST_INTERNAL_CHECK_NULL(Value, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and abort execution.
#define MTEST_ASSERT_NOT_NULL(Value) MTEST_INTERNAL_CHECK_NOT_NULL(Value, MTest::EFailType::Assert)
/// Check if floating point is near given value, if not test will fail and abort execution.
#define MTEST_ASSERT_NEAR(Value, Wanted, Epsilon) MTEST_INTERNAL_CHECK_NEAR(Value, Wanted, Epsilon, MTest::EFailType::Assert)
/// Check if given exception is throw, if not test will fail and abort execution.
#define MTEST_ASSERT_THROW(Statement, Exception) MTEST_INTERNAL_CHECK_THROW(Statement, Exception, MTest::EFailType::Assert)
/// Check if any exception is throw, if not test will fail and abort execution.
#define MTEST_ASSERT_ANY_THROW(Statement) MTEST_INTERNAL_CHECK_ANY_THROW(Statement, MTest::EFailType::Assert)
/// Check if exception is not throw, if not test will fail and abort execution.
#define MTEST_ASSERT_NO_THROW(Statement) MTEST_INTERNAL_CHECK_NO_THROW(Statement, MTest::EFailType::Assert)
/// User defined assert, if not evaluated to true statement test will fail and abort execution.
#define MTEST_ASSERT_CUSTOM(Result) MTEST_INTERNAL_CHECK_CUSTOM(Result, MTest::EFailType::Assert)

//// Logs & Utility

/// Skip test
#define MTEST_SKIP(Reason) MTest::GetTestManager().GetActiveTest()->Skip( Reason )

/// Print some information to stdout.
#define MTEST_INFO(Msg) MTest::GetLog().Write( "[Message] {}\n", Msg )

//// Test Setup

#define MTEST_INTERNAL_UNIT_TEST_FX_T(Section, Name, DataArray, ParentFixture, ConcreteFixture) \
struct ConcreteFixture final : ParentFixture, MTest::IFixtureWrapper \
{ \
    static_assert(MTest::IsTableDataArray<decltype(DataArray)>, "Data Array must be of type TableDataArray"); \
    static_assert(MTest::IsTableFixture<ParentFixture>, "Must be base of TableFixture"); \
    ConcreteFixture(const DataType& testData, const std::size_t testIndex): \
        MTest_TestData(testData), MTest_TestIndex(testIndex) \
    {} \
    std::string MTest_GenerateName() { return ParentFixture::GenerateName(MTest_TestData, MTest_TestIndex); } \
    void MTest_Run(const DataType& testData); \
    void MTest_Run() override { MTest_Run(MTest_TestData); } \
    bool MTest_Skip() override { return ParentFixture::Skip(MTest_TestData); } \
    void MTest_Setup() override { ParentFixture::Setup(MTest_TestData); } \
    void MTest_Cleanup() override { ParentFixture::Cleanup(MTest_TestData); } \
private: \
    const DataType MTest_TestData; \
    const std::size_t MTest_TestIndex; \
}; \
namespace \
{ \
    const MTest::Registrar MTEST_MACRO_CONCAT(MTest_Registrar_, __COUNTER__) \
    { \
        []() \
        { \
            const auto numCases = DataArray.size(); \
            for(std::size_t i{0uz}; i < numCases; ++i) \
            { \
                auto fixture = std::make_unique<ConcreteFixture>(DataArray[i], i); \
                const std::string caseName = std::format("{}[{}]", #Name, fixture->MTest_GenerateName()); \
                MTest::GetTestManager().AddTest \
                ( \
                    #Section, caseName, std::source_location::current(), std::move(fixture) \
                ); \
            } \
        } \
    }; \
} \
void ConcreteFixture::MTest_Run([[maybe_unused]] const ConcreteFixture::DataType& testData)

/// Define test case, give section name, test case name, data array and fixture name. Tests are executed by section, test case name
/// must be unique in given section.
#define MTEST_UNIT_TEST_FX_T(Section, Name, DataArray, ParentFixture) \
    MTEST_INTERNAL_UNIT_TEST_FX_T(Section, Name, DataArray, ParentFixture, MTEST_MACRO_CONCAT(MTest_##ParentFixture, __COUNTER__))

/// Define test case, give section name, test case name and data array. Fixture name is inferred from section name eg. 'Section' + TableFixture
/// Tests are executed by section, test case name must be unique in given section.
#define MTEST_UNIT_TEST_F_T(Section, Name, DataArray) MTEST_UNIT_TEST_FX_T(Section, Name, DataArray, Section##TableFixture )

#define MTEST_INTERNAL_UNIT_TEST_FX(Section, Name, ParentFixture, ConcreteFixture) \
struct ConcreteFixture final : ParentFixture, MTest::IFixtureWrapper \
{ \
    static_assert(std::derived_from<ParentFixture, MTest::Fixture>, "Must be base of Fixture"); \
    void MTest_Run() override; \
    bool MTest_Skip() override { return ParentFixture::Skip(); } \
    void MTest_Setup() override { ParentFixture::Setup(); } \
    void MTest_Cleanup() override { ParentFixture::Cleanup(); } \
}; \
namespace \
{ \
    const MTest::Registrar MTEST_MACRO_CONCAT(MTest_Registrar_, __COUNTER__) \
    { \
        []() \
        { \
            MTest::GetTestManager().AddTest \
            ( \
                #Section, #Name, std::source_location::current(), std::make_unique<ConcreteFixture>() \
            ); \
        } \
    }; \
} \
void ConcreteFixture::MTest_Run()

/// Define test case, give section name, test case name and fixture name. Tests are executed by section, test case name
/// must be unique in given section.
#define MTEST_UNIT_TEST_FX(Section, Name, ParentFixture) MTEST_INTERNAL_UNIT_TEST_FX(Section, Name, ParentFixture, MTEST_MACRO_CONCAT(MTest_##ParentFixture, __COUNTER__))

/// Define test case, give section name, test case name. Fixture name is inferred from section name eg. 'Section' + Fixture
/// Tests are executed by section, test case name must be unique in given section.
#define MTEST_UNIT_TEST_F(Section, Name) MTEST_UNIT_TEST_FX(Section, Name, Section##Fixture )

/// Define test case, give section name and test case name. Tests are executed by section, test case name
/// must be unique in given section.
#define MTEST_UNIT_TEST(Section, Name) MTEST_INTERNAL_UNIT_TEST_FX(Section, Name, MTest::Fixture, MTEST_MACRO_CONCAT(MTest_Fixture, __COUNTER__))

/// Add STD sink.
#define MTEST_CREATE_STD_SINK MTest::GetLog().CreateSink<MTest::CStdSink>()
/// Add File sink.
#define MTEST_CREATE_FILE_SINK(File) MTest::GetLog().CreateSink<MTest::CFileSink>( File )
/// Use to launch unit test application.
#define MTEST_RUN_TESTS(...) MTest::GetTestManager().Run( __VA_ARGS__ )

/// Use to Implement Unit test main().
#define MTEST_MAIN \
int main(int argc, char* argv[]) \
{ \
    MTEST_CREATE_STD_SINK; \
	return MTEST_RUN_TESTS(argc, argv) ? 0 : 1; \
}

namespace MTest
{
    template<class F, class R, class...Args>
    concept IsInvocable = std::is_invocable_r_v<R, F, Args...>;

    template<class T>
    concept IsEnumeration = std::is_enum_v<T>;

    enum class EConsoleColor
    {
        Default,
        Red,
        Yellow,
        Green,
        Blue,
        Magenta
    };

    class ISink
    {
    public:
        ISink() = default;
        virtual ~ISink() = default;

        ISink(const ISink&) = delete;
        ISink& operator=(const ISink&) = delete;

        ISink(ISink&&) = delete;
        ISink& operator=(ISink&&) = delete;

        virtual void SetColor(const EConsoleColor) = 0;
        virtual void Write(const std::string&) = 0;
    };

    class CStdSink final: public ISink
    {
    public:
    #if !defined(MTEST_CONFIG_NO_COLOR) && defined(MTEST_WINDOWS_PLATFORM)
        CStdSink() { Initialize_Windows(); }
        ~CStdSink() { Cleanup_Windows(); }
    #else
        CStdSink() = default;
        ~CStdSink() = default;
    #endif

    #ifndef MTEST_CONFIG_NO_COLOR
        void SetColor(const EConsoleColor value) override
        {
        #ifdef MTEST_WINDOWS_PLATFORM
            switch(value)
            {
            case EConsoleColor::Red:
                SetColor_Windows(12);
                break;
            case EConsoleColor::Yellow:
                SetColor_Windows(14);
                break;
            case EConsoleColor::Green:
                SetColor_Windows(10);
                break;
            case EConsoleColor::Blue:
                SetColor_Windows(9);
                break;
            case EConsoleColor::Magenta:
                SetColor_Windows(13);
                break;
            case EConsoleColor::Default:
                [[fallthrough]];
            default:
                SetColor_Windows(7);
                break;
            }
        #else
            switch(value)
            {
            case EConsoleColor::Red:
                std::fputs("\033[31m", stdout);
                break;
            case EConsoleColor::Yellow:
                std::fputs("\033[33m", stdout);
                break;
            case EConsoleColor::Green:
                std::fputs("\033[32m", stdout);
                break;
            case EConsoleColor::Blue:
                std::fputs("\033[34m", stdout);
                break;
            case EConsoleColor::Magenta:
                std::fputs("\033[35m", stdout);
                break;
            case EConsoleColor::Default:
                [[fallthrough]];
            default:
                std::fputs("\033[0m", stdout);
                break;
            }
        #endif
        }
    #else
        void SetColor(const EConsoleColor) override {}
    #endif

        void Write(const std::string& msg) override { std::fputs(msg.c_str(), stdout); }
    
    #if !defined(MTEST_CONFIG_NO_COLOR) && defined(MTEST_WINDOWS_PLATFORM)
    private:
        void Initialize_Windows()
        {
            Handle = GetStdHandle(STD_OUTPUT_HANDLE);
            CONSOLE_SCREEN_BUFFER_INFO consoleInfo{};
            std::ignore = GetConsoleScreenBufferInfo(Handle, &consoleInfo);
            ConsoleAttributes = consoleInfo.wAttributes;
        }
        void Cleanup_Windows()
        {
            std::ignore = SetConsoleTextAttribute(Handle, ConsoleAttributes);
        }
        void SetColor_Windows(WORD color)
        {
            constexpr WORD COLOR_MASK = 0x000F;
            // Integer promotion when use smaller types.
            // See: https://wiki.sei.cmu.edu/confluence/display/c/EXP14-C.+Beware+of+integer+promotion+when+performing+bitwise+operations+on+integer+types+smaller+than+int
            constexpr WORD INV_COLOR_MASK = static_cast<WORD>(~COLOR_MASK);
            const WORD newAttributes = (ConsoleAttributes & INV_COLOR_MASK) | (color & COLOR_MASK);
            std::ignore = SetConsoleTextAttribute(Handle, newAttributes);
        }
    private:
        HANDLE Handle{};
        WORD ConsoleAttributes{};
    #endif
    };

    class CFileSink final: public ISink
    {
    public:
        explicit CFileSink(const std::string& path):
            Handle(std::fopen(path.c_str(), "w"))
        {
        }

        ~CFileSink()
        {
            if( Handle )
            {
                std::fclose(Handle);
                Handle = nullptr;
            }
        }

        void SetColor(const EConsoleColor) override {}

        void Write(const std::string& x) override
        {
            if( Handle )
            {
                std::fputs(x.c_str(), Handle);
            }
        }
    private:
        std::FILE* Handle{};
    };

    class CLog final
    {
    private:
        CLog() = default;
    public:
        ~CLog() = default;

        CLog(const CLog&) = delete;
        CLog& operator=(const CLog&) = delete;

        CLog(CLog&&) = delete;
        CLog& operator=(CLog&&) = delete;

        static CLog& Instance()
        {
            static CLog log;
            return log;
        }

        // Create sink for test output
        template<std::derived_from<ISink> T, class...Args>
        void CreateSink(Args&&...args)
        {
            Sinks.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
        }

        template<class...Args>
        void Write(const EConsoleColor textColor, const std::format_string<Args...> fmt, Args&&...args)
        {
            Write(textColor, std::format(fmt, std::forward<Args>(args)...));
        }

        void Write(const EConsoleColor textColor, const std::string& string)
        {
            SetColor(textColor);
            Write(string);
            SetColor(EConsoleColor::Default);
        }

        template<class...Args>
        void Write(const std::format_string<Args...> fmt, Args&&...args)
        {
            Write(std::format(fmt, std::forward<Args>(args)...));
        }

        void Write(const std::string& string)
        {
            for(const auto& i: Sinks)
            {
                i->Write(string);
            }
        }
    private:
        void SetColor(const EConsoleColor textColor)
        {
            for(const auto& i: Sinks)
            {
                i->SetColor(textColor);
            }
        }
    private:
        std::vector<std::unique_ptr<ISink>> Sinks{};
    };
    inline CLog& GetLog() { return CLog::Instance(); }

    /// Can be used to init some data before each test. Test function has access to its internals (excluding private) - prefer structs (public default access).
    struct Fixture
    {
        Fixture() = default;
        virtual ~Fixture() = default;

        Fixture(const Fixture&) = delete;
        Fixture& operator=(const Fixture&) = delete;

        Fixture(Fixture&&) = delete;
        Fixture& operator=(Fixture&&) = delete;

        /// Should return true if test case should be skipped.
        virtual bool Skip() { return false; }
        /// Use Assertions to check state.
        virtual void Setup() {}
        /// Cleanup if needed.
        virtual void Cleanup() {}
    };

    /// Table test data array
    template<class T>
    using TableDataArray = std::vector<T>;

    // Type traits
    namespace Details
    {
        template<class T>
        struct IsTableDataArrayTrait: std::false_type {};
        template<class T>
        struct IsTableDataArrayTrait<TableDataArray<T>>: std::true_type {};
    }
    template<class T>
    concept IsTableDataArray = Details::IsTableDataArrayTrait<T>::value;

    /// Fixture to use with Table tests.
    template<class Type, std::derived_from<Fixture> Base>
    struct TableFixture: Base
    {
        using DataType = Type;
        using BaseClass = Base;

        TableFixture() = default;
        virtual ~TableFixture() = default;

        /// Get Test case name
        virtual std::string GenerateName(const DataType&, const std::size_t i) { return std::to_string(i); }

        /// Should return true if test case should be skipped.
        virtual bool Skip(const DataType&) { return BaseClass::Skip(); }
        using BaseClass::Skip;

        /// Use Assertions to check state.
        virtual void Setup(const DataType&) { BaseClass::Setup(); }
        using BaseClass::Setup;

        /// Cleanup if needed.
        virtual void Cleanup(const DataType&) { BaseClass::Cleanup(); }
        using BaseClass::Cleanup;
    };

    // Type traits
    // https://en.cppreference.com/w/cpp/types/is_base_of
    // https://stackoverflow.com/questions/36632897/type-trait-to-check-whether-some-type-is-derived-from-a-class-template?rq=3
    namespace Details
    {
        template<class T, class U>
        constexpr std::true_type IsTableFixtureHelper(const TableFixture<T, U>*) { return {}; }
        constexpr std::false_type IsTableFixtureHelper(const void*) { return {}; }
        //
        template<class Derived>
        struct IsTableFixtureTrait: std::bool_constant<decltype(IsTableFixtureHelper(std::declval<typename std::remove_cv<Derived*>::type>()))::value> {};
    }
    template<class T>
    concept IsTableFixture = Details::IsTableFixtureTrait<T>::value;

    struct IFixtureWrapper
    {
        IFixtureWrapper() = default;
        virtual ~IFixtureWrapper() = default;

        IFixtureWrapper(const IFixtureWrapper&) = delete;
        IFixtureWrapper& operator=(const IFixtureWrapper&) = delete;

        IFixtureWrapper(IFixtureWrapper&&) = delete;
        IFixtureWrapper& operator=(IFixtureWrapper&&) = delete;

        virtual void MTest_Run() = 0;
        virtual bool MTest_Skip() = 0;
        virtual void MTest_Setup() = 0;
        virtual void MTest_Cleanup() = 0;
    };
    using FixtureWrapperPtr = std::unique_ptr<IFixtureWrapper>;

    enum class EFailType
    {
        Check,
        Assert
    };

    enum class ETestResult
    {
        Success,
        Fail,
        Skip
    };

    namespace Details
    {
        inline std::string FailTypeToString(const EFailType type)
        {
            switch(type)
            {
            case EFailType::Check:
                return "[Check  ]";
            case EFailType::Assert:
                return "[Assert ]";
            default:
                return "[???????]";
            }
        }

        inline EConsoleColor TestResultToColor(const ETestResult result)
        {
            switch(result)
            {
            case ETestResult::Success:
                return EConsoleColor::Green;
            case ETestResult::Fail:
                return EConsoleColor::Red;
            case ETestResult::Skip:
                return EConsoleColor::Magenta;
            default:
                return EConsoleColor::Default;
            }
        }

        inline std::string TestResultToString(const ETestResult result)
        {
            switch(result)
            {
            case ETestResult::Success:
                return "[Success]";
            case ETestResult::Fail:
                return "[Failure]";
            case ETestResult::Skip:
                return "[Skipped]";
            default:
                return "[???????]";
            }
        }

        inline std::string FilenameFromPath(const std::string& path)
        {
            const auto idx = path.find_last_of("\\/");
            if( idx == std::string::npos )
            {
                return path;
            }
            return path.substr(idx+1u);
        }

        inline std::string FormatTime(const float timeInMS)
        {
            const bool asSeconds = timeInMS >= 100.0f;
            return std::format("({:.4f} {})", asSeconds ? timeInMS/1000.0f : timeInMS, asSeconds ? "s" : "ms");
        }
    }

    class CTestAssertionException final: public std::logic_error
    {
    public:
        CTestAssertionException():
            std::logic_error("Test Assertion")
        {
        }
    };

    class CTestSkippedException final: public std::logic_error
    {
    public:
        CTestSkippedException():
            std::logic_error("Test Skipped")
        {
        }
    };

    /// User defined check result
    using CheckResult = std::optional<std::string>;
    /// Call to pass check
    inline CheckResult CheckSuccess()
    {
        return std::nullopt;
    }
    /// Call to fail check
    template<class...Args>
    CheckResult CheckFailure(const std::format_string<Args...> fmt, Args&&...args)
    {
        return std::format(fmt, std::forward<Args>(args)...);
    }

    /// Represents one test case
    class CTestCase final
    {
        using TestClock = std::chrono::steady_clock;
        using TestClockStamp = std::chrono::steady_clock::time_point;
        using TestClockDuration = std::chrono::duration<float, std::milli>;
    public:
        CTestCase(const std::string& section, const std::string& name, const std::source_location location, FixtureWrapperPtr fixture):
            Section(section),
            Name(name),
            Fullname(MakeFullname(section, name)),
            File(Details::FilenameFromPath(location.file_name())),
            Line(location.line()),
            Fixture(std::move(fixture))
        {
        }
        ~CTestCase() = default;

        CTestCase(const CTestCase&) = delete;
        CTestCase& operator=(const CTestCase&) = delete;

        CTestCase(CTestCase&&) = delete;
        CTestCase& operator=(CTestCase&&) = delete;

        const std::string& GetSection() const { return Section; }
        const std::string& GetName() const { return Name; }
        const std::string& GetFullname() const { return Fullname; }
        const std::string& GetFile() const { return File; }
        std::uint_least32_t GetLine() const { return Line; }
        bool IsFailed() const { return Result == ETestResult::Fail; }
        bool IsSkipped() const { return Result == ETestResult::Skip; }
        ETestResult GetResult() const { return Result; }
        float GetDuration() const { return Duration; }

        static std::string MakeFullname(const std::string& section, const std::string& name) { return std::format("{}.{}", section, name); }

        bool IsRun(const std::string& filter) const
        {
            if( !filter.empty() )
            {
                return GetFullname().contains(filter);
            }
            return true;
        }

        void Run(const std::string& filter)
        {
            const TestClockStamp Start = TestClock::now();
            GetLog().Write(EConsoleColor::Blue, "[Start  ] {}\n", GetFullname());
            // Run
            bool needCleanup{false};
            Runner([&]()
            {
                if( !IsRun(filter) )
                {
                    Skip("Test case is skipped by Command-line");
                }
                if( Fixture->MTest_Skip() )
                {
                    Skip("Test case is skipped by Fixture");
                }
                // Setup test case
                needCleanup = true;
                Fixture->MTest_Setup();
                //
                Fixture->MTest_Run();
            });
            // Clear if needed
            if( needCleanup )
            {
                Runner([&]()
                {
                    Fixture->MTest_Cleanup();
                });
            }
            //
            Duration = TestClockDuration(TestClock::now()-Start).count();
            PrintResult(true);
        }

        template<IsEnumeration T>
        bool CheckEqual(const T& value, const T& wanted, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            return CheckEqual(std::to_underlying(value), std::to_underlying(wanted), message, type, location);
        }

        template<class T, class U>
        bool CheckEqual(const T& value, const U& wanted, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            if( value == wanted )
            {
                return true;
            }
            HandleFailure(std::format("Value '{}' is '{}' but should be '{}'", message, value, wanted), type, location);
            return false;
        }

        template<IsEnumeration T>
        bool CheckNotEqual(const T& value, const T& wanted, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            return CheckNotEqual(std::to_underlying(value), std::to_underlying(wanted), message, type, location);
        }

        template<class T, class U>
        bool CheckNotEqual(const T& value, const U& wanted, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            if( value != wanted )
            {
                return true;
            }
            HandleFailure(std::format("Value '{}' should not be '{}'", message, wanted), type, location);
            return false;
        }

        template<class T>
        bool CheckPointer(const T* value, const T* wanted, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            if( value == wanted )
            {
                return true;
            }
            HandleFailure(std::format("Pointer '{}' should be {}", message, static_cast<const void*>(wanted)), type, location);
            return false;
        }

        template<class T>
        bool CheckNotPointer(const T* value, const T* wanted, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            if( value != wanted )
            {
                return true;
            }
            HandleFailure(std::format("Pointer '{}' should not be {}", message, static_cast<const void*>(wanted)), type, location);
            return false;
        }

        template<class T>
        bool CheckNull(const T& pointer, const std::string& message, const EFailType type, const std::source_location location = std::source_location::current())
        {
            if( pointer == nullptr )
            {
                return true;
            }
            HandleFailure(std::format("Pointer '{}' should be null", message), type, location);
            return false;
        }

        template<class T>
        bool CheckNotNull(const T& pointer, const std::string& message, const EFailType type, const std::source_location location = std::source_location::current())
        {
            if( pointer != nullptr )
            {
                return true;
            }
            HandleFailure(std::format("Pointer '{}' should not be null", message), type, location);
            return false;
        }

        bool CheckTrue(const bool result, const std::string& message, const EFailType type, const std::source_location location = std::source_location::current())
        {
            if( result )
            {
                return true;
            }
            HandleFailure(std::format("Expression '{}' should evaluate to true", message), type, location);
            return false;
        }

        bool CheckFalse(const bool result, const std::string& message, const EFailType type, const std::source_location location = std::source_location::current())
        {
            if( !result )
            {
                return true;
            }
            HandleFailure(std::format("Expression '{}' should evaluate to false", message), type, location);
            return false;
        }

        template<std::floating_point T>
        bool CheckNear(const T& value, const T& wanted, const T& epsilon, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            if( std::abs(value-wanted) < epsilon )
            {
                return true;
            }
            HandleFailure(std::format("Value '{}' is '{}' but should be '{}' with delta '{}'", message, value, wanted, epsilon), type, location);
            return false;
        }

        template<class E, IsInvocable<void> Invocable>
        bool CheckThrow(Invocable invocable, const std::string& message, const std::string& exception, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            try
            {
                invocable();
                HandleFailure(std::format("Expression '{}' should throw '{}' but none exception was thrown", message, exception), type, location);
                return false;
            }
            catch(E&)
            {
                return true;
            }
            catch(...)
            {
                HandleFailure(std::format("Expression '{}' should throw '{}' but another exception was thrown", message, exception), type, location);
                return false;
            }
        }

        template<IsInvocable<void> Invocable>
        bool CheckAnyThrow(Invocable invocable, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            try
            {
                invocable();
                HandleFailure(std::format("Expression '{}' should throw exception", message), type, location);
                return false;
            }
            catch(...)
            {
                return true;
            }
        }

        template<IsInvocable<void> Invocable>
        bool CheckNoThrow(Invocable invocable, const std::string& message, const EFailType type,
            const std::source_location location = std::source_location::current())
        {
            try
            {
                invocable();
                return true;
            }
            catch(...)
            {
                HandleFailure(std::format("Expression '{}' should not throw exception", message), type, location);
                return false;
            }
        }

        bool CheckCustom(const CheckResult& result, const EFailType type, const std::source_location location = std::source_location::current())
        {
            if( !result.has_value() )
            {
                return true;
            }
            HandleFailure(result.value(), type, location);
            return false;
        }

        void Skip(const std::string& reason)
        {
            MarkSkipped();
            GetLog().Write(EConsoleColor::Magenta, "[Skipped] {}\n", reason);
            throw CTestSkippedException{};
        }

        void PrintResult(const bool isCompact) const
        {
            if( isCompact )
            {
                GetLog().Write(Details::TestResultToColor(Result), "{} {} {}\n", Details::TestResultToString(Result),
                    GetFullname(), Details::FormatTime(Duration));
            }
            else
            {
                GetLog().Write(Details::TestResultToColor(Result), "{} {} in File: {}, Line: {}\n", Details::TestResultToString(Result),
                    GetFullname(), GetFile(), GetLine());
            }
        }
    private:
        void MarkFailed() { Result = ETestResult::Fail; }
        void MarkSkipped() { Result = ETestResult::Skip; }

        void HandleFailure(const std::string& message, const EFailType type, const std::source_location location)
        {
            MarkFailed();
            GetLog().Write(EConsoleColor::Red, "{} {} in File: {}, Line: {}\n", Details::FailTypeToString(type), message,
                Details::FilenameFromPath(location.file_name()), location.line());
            if( type == EFailType::Assert )
            {
                throw CTestAssertionException{};
            }
        }

        void HandleException(const std::string& what)
        {
            MarkFailed();
            GetLog().Write(EConsoleColor::Red, "[Fatal  ] {}\n", what);
        }

        template<IsInvocable<void> Invocable>
        void Runner(Invocable invocable)
        {
            try
            {
                invocable();
            }
            catch(const CTestAssertionException&) {}
            catch(const CTestSkippedException&) {}
            catch(const std::exception& e)
            {
                HandleException(e.what());
            }
            catch(...)
            {
                HandleException("Unknown exception was thrown");
            }
        }
    private:
        std::string Section{};
        std::string Name{};
        std::string Fullname{};
        std::string File{};
        std::uint_least32_t Line{};
        ETestResult Result{ETestResult::Success};
        FixtureWrapperPtr Fixture{};
        float Duration{0.0f}; // In miliseconds
    };

    /// Manages tests and runs them
    class CTestManager final
    {
    private:
        CTestManager() = default;
    public:
        ~CTestManager() = default;

        CTestManager(const CTestManager&) = delete;
        CTestManager& operator=(const CTestManager&) = delete;

        CTestManager(CTestManager&&) = delete;
        CTestManager& operator=(CTestManager&&) = delete;

        static CTestManager& Instance()
        {
            static CTestManager manager;
            return manager;
        }

        CTestCase* GetActiveTest() const { return ActiveTest; }

        void AddTest(const std::string& section, const std::string& name, const std::source_location location, FixtureWrapperPtr fixture)
        {
            auto& sectionTests = Tests[section];
            auto it = std::find_if(sectionTests.begin(), sectionTests.end(), [&](const std::unique_ptr<CTestCase>& other)
            {
                return other->GetName() == name;
            });
            if( it != sectionTests.end() )
            {
                GetLog().Write(EConsoleColor::Red, "[Error  ] {} already exists\n", CTestCase::MakeFullname(section, name));
                return;
            }
            sectionTests.push_back(std::make_unique<CTestCase>(section, name, location, std::move(fixture)));
        }

        bool Run(int argc, const char* const argv[])
        {
            std::vector<std::string> cmd{};
            for(int i{1}; i < argc; ++i)
            {
                cmd.push_back(argv[i]);
            }
            return Run(cmd);
        }

        bool Run(const std::vector<std::string>& cmd)
        {
            std::string filter{};
            for(const auto& i: cmd)
            {
                if( i.starts_with("-F=") || i.starts_with("--Filter=") )
                {
                    filter = i.substr(i.find_first_of('=') + 1uz);
                }
                else
                {
                    GetLog().Write(EConsoleColor::Red, "[Manager] Invalid argument: {}\n", i);
                    return false;
                }
            }
            const auto [totalTestsCount, filteredTestsCount] = GetTestCount(filter);
            if(filteredTestsCount != totalTestsCount)
            {
                GetLog().Write(EConsoleColor::Blue, "[Manager] Running {} from {} tests\n", filteredTestsCount, totalTestsCount);
            }
            else
            {
                GetLog().Write(EConsoleColor::Blue, "[Manager] Running {} tests\n", totalTestsCount);
            }
            if( !filter.empty() )
            {
                GetLog().Write(EConsoleColor::Blue, "[Manager] Test filter: '{}'\n", filter);
            }
            GetLog().Write("\n");
            // Run tests now.
            float totalTime{0.0f};
            std::vector<CTestCase*> failedTests{};
            std::vector<CTestCase*> skippedTests{};
            std::vector<CTestCase*> successfulTests{};
            for(const auto& i: Tests)
            {
                float sectionTime{0.0f};
                GetLog().Write(EConsoleColor::Blue, "[-------] Running section {} which has {} tests\n", i.first, i.second.size());
                for(const auto& j: i.second)
                {
                    auto testCase = j.get();
                    ActiveTest = testCase;
                    testCase->Run(filter);
                    ActiveTest = nullptr;
                    // Update result.
                    sectionTime += testCase->GetDuration();
                    if( testCase->IsFailed() )
                    {
                        failedTests.push_back(testCase);
                    }
                    else if( testCase->IsSkipped() )
                    {
                        skippedTests.push_back(testCase);
                    }
                    else
                    {
                        successfulTests.push_back(testCase);
                    }
                }
                GetLog().Write(EConsoleColor::Blue, "[-------] Section {} finished {}\n\n", i.first, Details::FormatTime(sectionTime));
                totalTime += sectionTime;
            }
            // Print result
            GetLog().Write(EConsoleColor::Blue, "[Manager] Running finished {}\n", Details::FormatTime(totalTime));
            if( totalTestsCount != 0uz )
            {
                // Print failed tests for quick lookup.
                if( !failedTests.empty() )
                {
                    GetLog().Write(EConsoleColor::Red, "[Manager] Failed test list:\n");
                    for(const auto* i: failedTests)
                    {
                        i->PrintResult(false);
                    }
                }
                GetLog().Write(EConsoleColor::Blue, "[Manager] Test summary:\n");
                PrintSummary(EConsoleColor::Green, "Successful", successfulTests);
                PrintSummary(EConsoleColor::Red, "Failed", failedTests);
                PrintSummary(EConsoleColor::Magenta, "Skipped", skippedTests);
            }
            else
            {
                GetLog().Write(EConsoleColor::Blue, "[Manager] No tests are present\n");
            }
            Tests.clear();
            // Return
            return failedTests.empty();
        }
    private:
        std::pair<std::size_t, std::size_t> GetTestCount(const std::string& filter) const
        {
            std::size_t totalTests{0uz};
            std::size_t filteredTests{0uz};
            for(const auto& i: Tests)
            {
                totalTests += i.second.size();
                for(const auto& j: i.second)
                {
                    if( j->IsRun(filter) )
                    {
                        ++filteredTests;
                    }
                }
            }
            return {totalTests, filteredTests};
        }

        void PrintSummary(const EConsoleColor color, const std::string& caption, const std::vector<CTestCase*>& array)
        {
            if( array.size() )
            {
                GetLog().Write(color, "[Manager] {} tests: {}\n", caption, array.size());
            }
        }
    private:
        std::unordered_map<std::string, std::vector<std::unique_ptr<CTestCase>>> Tests{};
        CTestCase* ActiveTest{};
    };
    inline CTestManager& GetTestManager() { return CTestManager::Instance(); }

    struct Registrar final
    {
        template<IsInvocable<void> Invocable>
        explicit Registrar(Invocable invocable)
        {
            invocable();
        }
    };

    template<std::floating_point T>
    inline constexpr T EPSILON = std::numeric_limits<T>::epsilon();

    template<std::floating_point T>
    inline constexpr T EPSILON_SMALL = T{0.0001};
}
