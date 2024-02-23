/*
MIT License

Copyright (c) 2024 Mateusz Rugor

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

#ifdef MTEST_WINDOWS_PLATFORM
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif // MTEST_WINDOWS_PLATFORM

//// Utils

#define MTEST_IMPL_MACRO_CONCAT(x, y) x##y
/// Needed because __LINE__ or __COUNTER__ itself is a macro so it needs two level expanding.
#define MTEST_MACRO_CONCAT(x, y) MTEST_IMPL_MACRO_CONCAT(x, y)

//// Assertions

#define MTEST_IMPL_CHECK_TRUE(Condition, Type) MTest::CTestManager::Instance().GetActiveTest()->Check( #Condition, (Condition), Type )
#define MTEST_IMPL_CHECK_FALSE(Condition, Type) MTest::CTestManager::Instance().GetActiveTest()->Check( #Condition, !(Condition), Type )
#define MTEST_IMPL_CHECK_VALUE(Value, Wanted, Type) MTEST_IMPL_CHECK_TRUE(Value == Wanted, Type)
#define MTEST_IMPL_CHECK_NOT_VALUE(Value, Wanted, Type) MTEST_IMPL_CHECK_TRUE(Value != Wanted, Type)
#define MTEST_IMPL_CHECK_NULL(Value, Type) MTEST_IMPL_CHECK_VALUE(Value, nullptr, Type)
#define MTEST_IMPL_CHECK_NOT_NULL(Value, Type) MTEST_IMPL_CHECK_NOT_VALUE(Value, nullptr, Type)
#define MTEST_IMPL_CHECK_THROW(Statement, Exception, Type) MTest::CTestManager::Instance().GetActiveTest()->Throw<Exception>( [&](){ Statement; }, Type )
#define MTEST_IMPL_CHECK_ANY_THROW(Statement, Type) MTest::CTestManager::Instance().GetActiveTest()->AnyThrow( [&](){ Statement; }, Type )
#define MTEST_IMPL_CHECK_NO_THROW(Statement, Type) MTest::CTestManager::Instance().GetActiveTest()->NoThrow( [&](){ Statement; }, Type )
#define MTEST_IMPL_CHECK_NEAR(Value, Wanted, Epsilon, Type) MTEST_IMPL_CHECK_TRUE(MTest::Equal(Value, Wanted, Epsilon), Type)

/// It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_TRUE(Condition) MTEST_IMPL_CHECK_TRUE(Condition, MTest::EFailType::Check)
/// It must evaluate to false statement, if not test will fail but will continue execution.
#define MTEST_CHECK_FALSE(Condition) MTEST_IMPL_CHECK_FALSE(Condition, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK(Condition) MTEST_IMPL_CHECK_TRUE(Condition, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_VALUE(Value, Wanted) MTEST_IMPL_CHECK_VALUE(Value, Wanted, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NOT_VALUE(Value, Wanted) MTEST_IMPL_CHECK_NOT_VALUE(Value, Wanted, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NULL(Value) MTEST_IMPL_CHECK_NULL(Value, MTest::EFailType::Check)
/// It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NOT_NULL(Value) MTEST_IMPL_CHECK_NOT_NULL(Value, MTest::EFailType::Check)
/// Check if given exception is throw
#define MTEST_CHECK_THROW(Statement, Exception) MTEST_IMPL_CHECK_THROW(Statement, Exception, MTest::EFailType::Check)
/// Check if any exception is throw
#define MTEST_CHECK_ANY_THROW(Statement) MTEST_IMPL_CHECK_ANY_THROW(Statement, MTest::EFailType::Check)
/// Check if exception is not throw
#define MTEST_CHECK_NO_THROW(Statement) MTEST_IMPL_CHECK_NO_THROW(Statement, MTest::EFailType::Check)
/// Check if floating point is near given value
#define MTEST_CHECK_NEAR(Value, Wanted, Epsilon) MTEST_IMPL_CHECK_NEAR(Value, Wanted, Epsilon, MTest::EFailType::Check)

/// Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_TRUE(Condition) MTEST_IMPL_CHECK_TRUE(Condition, MTest::EFailType::Assert)
/// Must evaluate to false statement, if not test will fail and will exit.
#define MTEST_ASSERT_FALSE(Condition) MTEST_IMPL_CHECK_FALSE(Condition, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT(Condition) MTEST_IMPL_CHECK_TRUE(Condition, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_VALUE(Value, Wanted) MTEST_IMPL_CHECK_VALUE(Value, Wanted, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NOT_VALUE(Value, Wanted) MTEST_IMPL_CHECK_NOT_VALUE(Value, Wanted, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NULL(Value) MTEST_IMPL_CHECK_NULL(Value, MTest::EFailType::Assert)
/// Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NOT_NULL(Value) MTEST_IMPL_CHECK_NOT_NULL(Value, MTest::EFailType::Assert)
/// Check if given exception is throw
#define MTEST_ASSERT_THROW(Statement, Exception) MTEST_IMPL_CHECK_THROW(Statement, Exception, MTest::EFailType::Assert)
/// Check if any exception is throw
#define MTEST_ASSERT_ANY_THROW(Statement) MTEST_IMPL_CHECK_ANY_THROW(Statement, MTest::EFailType::Assert)
/// Check if exception is not throw
#define MTEST_ASSERT_NO_THROW(Statement) MTEST_IMPL_CHECK_NO_THROW(Statement, MTest::EFailType::Assert)
/// Check if floating point is near given value
#define MTEST_ASSERT_NEAR(Value, Wanted, Epsilon) MTEST_IMPL_CHECK_NEAR(Value, Wanted, Epsilon, MTest::EFailType::Assert)

//// Logs & Utility

/// Skip test
#define MTEST_SKIP(Reason) MTest::CTestManager::Instance().GetActiveTest()->Skip( Reason )

/// Print some information to stdout.
#define MTEST_INFO(Msg) MTest::CLog::Instance() << MTest::EConsoleColor::Default << "[Message] " << Msg << "\n"

//// Test Setup

#define MTEST_IMPL_UNIT_TEST_FX_T(Section, Name, DataArray, ParentFixture, ConcreteFixture) \
struct ConcreteFixture final : ParentFixture, MTest::IFixtureWrapper \
{ \
    static_assert(MTest::IsTableDataArray<decltype(DataArray)>::value, "Data Array must be of type TableDataArray"); \
    static_assert(MTest::IsTableFixture<ParentFixture>::value, "Must be base of TableFixture"); \
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
            for(std::size_t i = std::size_t{0}; i < numCases; ++i) \
            { \
                auto fixture = std::make_unique<ConcreteFixture>(DataArray[i], i); \
                const std::string caseName = std::format("{}[{}]", #Name, fixture->MTest_GenerateName()); \
                MTest::CTestManager::Instance().AddTest \
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
    MTEST_IMPL_UNIT_TEST_FX_T(Section, Name, DataArray, ParentFixture, MTEST_MACRO_CONCAT(MTest_##ParentFixture, __COUNTER__))

/// Define test case, give section name, test case name and data array. Fixture name is inferred from section name eg. 'Section' + TableFixture
/// Tests are executed by section, test case name must be unique in given section.
#define MTEST_UNIT_TEST_F_T(Section, Name, DataArray) MTEST_UNIT_TEST_FX_T(Section, Name, DataArray, Section##TableFixture )

#define MTEST_IMPL_UNIT_TEST_FX(Section, Name, ParentFixture, ConcreteFixture) \
struct ConcreteFixture final : ParentFixture, MTest::IFixtureWrapper \
{ \
    static_assert(std::is_base_of<MTest::Fixture, ParentFixture>::value, "Must be base of Fixture"); \
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
            MTest::CTestManager::Instance().AddTest \
            ( \
                #Section, #Name, std::source_location::current(), std::make_unique<ConcreteFixture>() \
            ); \
        } \
    }; \
} \
void ConcreteFixture::MTest_Run()

/// Define test case, give section name, test case name and fixture name. Tests are executed by section, test case name
/// must be unique in given section.
#define MTEST_UNIT_TEST_FX(Section, Name, ParentFixture) MTEST_IMPL_UNIT_TEST_FX(Section, Name, ParentFixture, MTEST_MACRO_CONCAT(MTest_##ParentFixture, __COUNTER__))

/// Define test case, give section name, test case name. Fixture name is inferred from section name eg. 'Section' + Fixture
/// Tests are executed by section, test case name must be unique in given section.
#define MTEST_UNIT_TEST_F(Section, Name) MTEST_UNIT_TEST_FX(Section, Name, Section##Fixture )

/// Define test case, give section name and test case name. Tests are executed by section, test case name
/// must be unique in given section.
#define MTEST_UNIT_TEST(Section, Name) MTEST_IMPL_UNIT_TEST_FX(Section, Name, MTest::Fixture, MTEST_MACRO_CONCAT(MTest_Fixture, __COUNTER__))

/// Add STD sink.
#define MTEST_CREATE_STD_SINK MTest::CLog::Instance().CreateSink<MTest::CStdSink>()
/// Add File sink.
#define MTEST_CREATE_FILE_SINK(File) MTest::CLog::Instance().CreateSink<MTest::CFileSink>( File )
/// Use to launch unit test application.
#define MTEST_RUN_TESTS MTest::CTestManager::Instance().Run()

/// Use to Implement Unit test main().
#define MTEST_MAIN \
int main(int, char*[]) \
{ \
    MTEST_CREATE_STD_SINK; \
	return MTEST_RUN_TESTS; \
}

namespace MTest
{
    enum class EConsoleColor
    {
        Default,
        Red,
        Yellow,
        Green,
        Blue,
        Magenta
    };

    enum class EFailType
    {
        Check,
        Assert
    };

    namespace Details
    {
        #ifdef MTEST_WINDOWS_PLATFORM
        // https://learn.microsoft.com/en-us/dotnet/api/system.consolecolor?view=net-7.0
        inline int ColorToWin32(const EConsoleColor x)
        {
            switch(x)
            {
            case EConsoleColor::Red:
                return 12;
            case EConsoleColor::Yellow:
                return 14;
            case EConsoleColor::Green:
                return 10;
            case EConsoleColor::Blue:
                return 9;
            case EConsoleColor::Magenta:
                return 13;
            case EConsoleColor::Default:
                [[fallthrough]];
            default:
                return 7;
            }
        }
        #else
        // https://www.codeproject.com/Tips/5255355/How-to-Put-Color-on-Windows-Console
        inline std::string ColorToAnsi(const EConsoleColor x)
        {
            switch(x)
            {
            case EConsoleColor::Red:
                return "\033[31m";
            case EConsoleColor::Yellow:
                return "\033[33m";
            case EConsoleColor::Green:
                return "\033[32m";
            case EConsoleColor::Blue:
                return "\033[34m";
            case EConsoleColor::Magenta:
                return "\033[35m";
            case EConsoleColor::Default:
                [[fallthrough]];
            default:
                return "\033[0m";
            }
        }
        #endif // MTEST_WINDOWS_PLATFORM

        inline std::string FailTypeToString(const EFailType x)
        {
            switch(x)
            {
            case EFailType::Check:
                return "[Check  ]";
            case EFailType::Assert:
                return "[Assert ]";
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
    }

    struct SCheckInfo final
    {
        SCheckInfo(const EFailType type, const std::string& message, const std::source_location location):
            Type(type),
            Message(message),
            File(Details::FilenameFromPath(location.file_name())),
            Line(location.line())
        {
        }

        EFailType GetType() const { return Type; }
        const std::string& GetMessage() const { return Message; }
        const std::string& GetFile() const { return File; }
        std::uint_least32_t GetLine() const { return Line; }

        std::string ToString() const
        {
            return std::format("{} {}, File: {}, Line: {}", Details::FailTypeToString(Type), Message, File, Line);
        }

        EFailType Type;
        std::string Message;
        std::string File;
        std::uint_least32_t Line;
    };

    class ISink
    {
    public:
        ISink() = default;
        virtual ~ISink() = default;

        virtual void Write(const EConsoleColor) = 0;
        virtual void Write(const std::string&) = 0;
    };

    class CStdSink final: public ISink
    {
    public:
        void Write(const EConsoleColor x) override
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Details::ColorToWin32(x));
            #else
            std::fputs(Details::ColorToAnsi(x).c_str(), stdout);
            #endif // MTEST_WINDOWS_PLATFORM
        }
        void Write(const std::string& x) override { std::fputs(x.c_str(), stdout); }
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

        void Write(const EConsoleColor) override {}
        void Write(const std::string& x) override
        {
            if( Handle )
            {
                std::fputs(x.c_str(), Handle);
            }
        }
    private:
        std::FILE* Handle = {};
    };

    class CLog final
    {
    private:
        CLog() = default;
    public:
        ~CLog() = default;

        static CLog& Instance()
        {
            static CLog Log;
            return Log;
        }

        /// Create sink for test output
        template<std::derived_from<ISink> T, class...Args>
        void CreateSink(Args&&...args)
        {
            Sinks.emplace_back( std::make_unique<T>(std::forward<Args>(args)...) );
        }

        CLog& operator<<(const std::string& v)
        {
            for(const auto& i: Sinks)
            {
                i->Write(v);
            }
            return *this;
        }

        CLog& operator<<(const char* v)
        {
            for(const auto& i: Sinks)
            {
                i->Write(v);
            }
            return *this;
        }

        CLog& operator<<(const EConsoleColor v)
        {
            for(const auto& i: Sinks)
            {
                i->Write(v);
            }
            return *this;
        }

        CLog& operator<<(const SCheckInfo& v)
        {
            const auto temp = v.ToString();
            for(const auto& i: Sinks)
            {
                i->Write(temp);
            }
            return *this;
        }

        template<class T>
        CLog& operator<<(const T& v)
        {
            const auto temp = std::format("{}", v);
            for(const auto& i: Sinks)
            {
                i->Write(temp);
            }
            return *this;
        }
    private:
        std::vector<std::unique_ptr<ISink>> Sinks;
    };

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

    /// Can be used to init some data before each test. Test function has access to its internals (excluding private) - prefer structs (public default access).
    struct Fixture
    {
        Fixture() = default;
        virtual ~Fixture() = default;

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
    template<class T>
    struct IsTableDataArray: std::false_type
    {};
    template<class T>
    struct IsTableDataArray<TableDataArray<T>>: std::true_type
    {};

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
        constexpr std::true_type IsTableFixture(const TableFixture<T, U>*) { return {}; }
        constexpr std::false_type IsTableFixture(const void*) { return {}; }
    }
    template<class Derived>
    struct IsTableFixture: std::bool_constant<decltype(Details::IsTableFixture(std::declval<typename std::remove_cv<Derived*>::type>()))::value>
    {};

    struct IFixtureWrapper
    {
        IFixtureWrapper() = default;
        virtual ~IFixtureWrapper() = default;

        virtual void MTest_Run() = 0;
        virtual bool MTest_Skip() = 0;
        virtual void MTest_Setup() = 0;
        virtual void MTest_Cleanup() = 0;
    };
    using FixtureWrapperPtr = std::unique_ptr<IFixtureWrapper>;

    template<class F, class R, class...Args>
    concept IsInvocable = std::is_invocable_r<R, F, Args...>::value;

    enum class ETestResult
    {
        Success,
        Fail,
        Skip
    };
    namespace Details
    {
        inline std::string FormatTime(const float timeInMS)
        {
            const bool asSeconds = timeInMS >= 100.0f;
            return std::format("({:.4f} {})", asSeconds ? timeInMS/1000.0f : timeInMS, asSeconds ? "s" : "ms");
        }
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
        const std::string& GetFile() const { return File; }
        std::uint_least32_t GetLine() const { return Line; }
        const std::vector<SCheckInfo>& GetFailedChecks() const { return FailedChecks; }
        const std::optional<SCheckInfo>& GetFailedAssertion() const { return FailedAssertion; }
        bool IsFailed() const { return Result == ETestResult::Fail; }
        bool IsSkipped() const { return Result == ETestResult::Skip; }
        ETestResult GetResult() const { return Result; }
        float GetDuration() const { return Duration; }

        std::string GetDisplayString() const { return MakeDisplayString(Section, Name); }
        static std::string MakeDisplayString(const std::string& section, const std::string& name) { return std::format("{}.{}", section, name); }

        void Run()
        {
            Start = TestClock::now();
            CLog::Instance() << EConsoleColor::Blue << "[Start  ] " << GetDisplayString() << EConsoleColor::Default << "\n";
            // Run
            Runner([&]()
            {
                if( Fixture->MTest_Skip() )
                {
                    Skip("Test case is skipped by Fixture");
                }
                Fixture->MTest_Setup();
                //
                Fixture->MTest_Run();
            });
            // Clear if needed
            Runner([&]()
            {
                Fixture->MTest_Cleanup();
            });
            //
            Duration = TestClockDuration(TestClock::now()-Start).count();
            PrintResult(true);
        }

        /// Returns true when pass
        bool Check(const std::string& message, const bool result, const EFailType type, const std::source_location location = std::source_location::current())
        {
            if( !result )
            {
                HandleFailure(type, message, location);
            }
            return result;
        }

        template<class E, IsInvocable<void> Invocable>
        void Throw(Invocable invocable, const EFailType type, const std::source_location location = std::source_location::current())
        {
            try
            {
                invocable();
                // Failure below (no exception)
                HandleFailure(type, "Failed with no exception", location);
            }
            // Success
            catch(E&) {}
            // Failure
            catch(...)
            {
                HandleFailure(type, "Failed with different exception", location);
            }
        }

        template<IsInvocable<void> Invocable>
        void AnyThrow(Invocable invocable, const EFailType type, const std::source_location location = std::source_location::current())
        {
            try
            {
                invocable();
                // Failure below (no exception)
                HandleFailure(type, "Failed with no exception", location);
            }
            // Success
            catch(...) {}
        }

        template<IsInvocable<void> Invocable>
        void NoThrow(Invocable invocable, const EFailType type, const std::source_location location = std::source_location::current())
        {
            try
            {
                invocable();
                // Success
            }
            // Failure
            catch(...)
            {
                HandleFailure(type, "Failed with exception", location);
            }
        }

        void Skip(const std::string& reason)
        {
            MarkSkipped();
            CLog::Instance() << EConsoleColor::Magenta << "[Skipped] " << reason << EConsoleColor::Default << "\n";
            throw CTestSkippedException{};
        }

        void PrintResult(const bool showTime) const
        {
            CLog::Instance() << ResultToColor() << ResultToString() << " " << GetDisplayString();
            if( showTime )
            {
                CLog::Instance() << " " << Details::FormatTime(Duration);
            }
            else
            {
                CLog::Instance() << ", File: " << GetFile() << ", Line: " << GetLine();
            }
            CLog::Instance() << EConsoleColor::Default << "\n";
        }
    private:
        void MarkFailed() { Result = ETestResult::Fail; }
        void MarkSkipped() { Result = ETestResult::Skip; }

        void HandleFailure(const EFailType type, const std::string& message, const std::source_location location)
        {
            MarkFailed();
            const SCheckInfo info{type, message, location};
            CLog::Instance() << MTest::EConsoleColor::Red << info << MTest::EConsoleColor::Default << "\n";
            if( type == EFailType::Check )
            {
                FailedChecks.push_back(info);
            }
            else
            {
                FailedAssertion = info;
                throw CTestAssertionException{};
            }
        }

        void HandleException(const std::string& what)
        {
            MarkFailed();
            CLog::Instance() << EConsoleColor::Red << "[Fatal  ] " << what << EConsoleColor::Default << "\n";
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
                HandleException("Unknown");
            }
        }

        EConsoleColor ResultToColor() const
        {
            switch(Result)
            {
            case ETestResult::Success:
                return EConsoleColor::Green;
            case ETestResult::Fail:
                return EConsoleColor::Red;
            case ETestResult::Skip:
                return EConsoleColor::Magenta;
            default:
                return EConsoleColor::Default; // Should not happen
            }
        }

        std::string ResultToString() const
        {
            switch(Result)
            {
            case ETestResult::Success:
                return "[Success]";
            case ETestResult::Fail:
                return "[Failure]";
            case ETestResult::Skip:
                return "[Skipped]";
            default:
                return "[???????]"; // Should not happen
            }
        }
    private:
        std::string Section;
        std::string Name;
        std::string File;
        std::uint_least32_t Line;
        std::vector<SCheckInfo> FailedChecks;
        std::optional<SCheckInfo> FailedAssertion;
        ETestResult Result = ETestResult::Success;
        FixtureWrapperPtr Fixture;
        TestClockStamp Start = {};
        float Duration = 0.0f; // In miliseconds
    };

    /// Manages tests and runs them
    class CTestManager final
    {
    private:
        CTestManager()
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Details::ColorToWin32(EConsoleColor::Default));
            #endif // MTEST_WINDOWS_PLATFORM
        }
    public:
        ~CTestManager()
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), csbi.wAttributes);
            #endif // MTEST_WINDOWS_PLATFORM
        }

        CTestManager(const CTestManager&) = delete;
        CTestManager& operator=(const CTestManager&) = delete;

        CTestManager(CTestManager&&) = delete;
        CTestManager& operator=(CTestManager&&) = delete;

        static CTestManager& Instance()
        {
            static CTestManager Manager;
            return Manager;
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
                CLog::Instance() << EConsoleColor::Red << "[Error  ] " << CTestCase::MakeDisplayString(section, name) << " Already Exists"
                    << EConsoleColor::Default << "\n";
                return;
            }
            sectionTests.push_back(std::make_unique<CTestCase>(section, name, location, std::move(fixture)));
        }

        int Run()
        {
            // Use literal when migrate to C++23 - 0uz
            std::size_t testCount = std::accumulate
            (
                Tests.begin(), Tests.end(), std::size_t{0},
                [](const std::size_t val, const auto& item)
                {
                    return val+item.second.size();
                }
            );
            std::vector<CTestCase*> failedTests;
            std::vector<CTestCase*> skippedTests;
            std::vector<CTestCase*> successfulTests;
            float totalTime{0.0f};
            CLog::Instance() << EConsoleColor::Blue << "[Manager] Running " << testCount  << " tests grouped into "
                << Tests.size() << " sections" << EConsoleColor::Default << "\n\n";
            for(const auto& i: Tests)
            {
                float sectionTime{0.0f};
                CLog::Instance() << EConsoleColor::Blue << "[-------] Running section " << i.first << " which has " 
                    << i.second.size() << " tests" << EConsoleColor::Default << "\n";
                for(const auto& j: i.second)
                {
                    auto testCase = j.get();
                    ActiveTest = testCase;
                    testCase->Run();
                    ActiveTest = nullptr;
                    // Update result
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
                CLog::Instance() << EConsoleColor::Blue << "[-------] Section " << i.first << " finished "
                    << Details::FormatTime(sectionTime) << EConsoleColor::Default << "\n\n";
                totalTime += sectionTime;
            }
            // Print result
            CLog::Instance() << EConsoleColor::Blue << "[Manager] Running finished " << Details::FormatTime(totalTime) << EConsoleColor::Default << "\n";
            if( testCount != std::size_t{0} )
            {
                // Print failed or skipped tests for quick lookup
                if( !failedTests.empty() || !skippedTests.empty() )
                {
                    CLog::Instance() << EConsoleColor::Blue << "[Manager] Test list:" << EConsoleColor::Default << "\n";
                    PrintResult(failedTests);
                    PrintResult(skippedTests);
                }
                CLog::Instance() << EConsoleColor::Blue << "[Manager] Test summary:" << EConsoleColor::Default << "\n";
                PrintSummary(successfulTests, EConsoleColor::Green, "Successful");
                PrintSummary(failedTests, EConsoleColor::Red, "Failed");
                PrintSummary(skippedTests, EConsoleColor::Magenta, "Skipped");
            }
            else
            {
                CLog::Instance() << EConsoleColor::Blue << "[Manager] No Test Run or Registered" << EConsoleColor::Default << "\n";
            }
            Tests.clear();
            // Return
            return failedTests.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
        }
    private:
        void PrintResult(const std::vector<CTestCase*>& array)
        {
            for(const auto* i: array)
            {
                i->PrintResult(false);
            }
        }

        void PrintSummary(const std::vector<CTestCase*>& array, const EConsoleColor color, const std::string& caption)
        {
            if( array.size() )
            {
                CLog::Instance() << color << "[Manager] " << caption << " tests: " << array.size() << EConsoleColor::Default << "\n";
            }
        }
    private:
        std::unordered_map<std::string, std::vector<std::unique_ptr<CTestCase>>> Tests;
        CTestCase* ActiveTest = nullptr;
        #ifdef MTEST_WINDOWS_PLATFORM
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        #endif // MTEST_WINDOWS_PLATFORM
    };

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

    template<std::floating_point T>
    bool Equal(const T value, const T wanted, const T epsilon)
    {
        return std::abs(value-wanted) < epsilon;
    }
}
