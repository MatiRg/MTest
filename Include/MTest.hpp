/*
MIT License

Copyright (c) 2023 Mateusz Rugor

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
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <ostream>
#include <unordered_map>
#include <memory>
#include <utility>
#include <type_traits>
#include <stdexcept>
#include <source_location>
#include <cstdint>
#include <sstream>
#include <optional>
#include <concepts>
#include <fstream>
#include <chrono>
#include <numeric>

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

#define MTEST_DETAIL_MACRO_CONCAT(x, y) x##y
//! Needed because __LINE__ or __COUNTER__ itself is a macro so it needs two level expanding.
#define MTEST_MACRO_CONCAT(x, y) MTEST_DETAIL_MACRO_CONCAT(x, y)

//// Assertions

#define MTEST_DETAIL_CHECK_TRUE(Cond, IsAssert) MTest::ITestManager::GetActiveCase()->Check( #Cond , (Cond) , IsAssert )
#define MTEST_DETAIL_CHECK_FALSE(Cond, IsAssert) MTest::ITestManager::GetActiveCase()->Check( #Cond , !(Cond) , IsAssert )
#define MTEST_DETAIL_CHECK_VALUE(Value, Wanted, IsAssert) MTEST_DETAIL_CHECK_TRUE(Value == Wanted, IsAssert)
#define MTEST_DETAIL_CHECK_NOT_VALUE(Value, Wanted, IsAssert) MTEST_DETAIL_CHECK_TRUE(Value != Wanted, IsAssert)
#define MTEST_DETAIL_CHECK_NULL(Value, IsAssert) MTEST_DETAIL_CHECK_VALUE(Value, nullptr, IsAssert)
#define MTEST_DETAIL_CHECK_NOT_NULL(Value, IsAssert) MTEST_DETAIL_CHECK_NOT_VALUE(Value, nullptr, IsAssert)

//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_TRUE(Cond) MTEST_DETAIL_CHECK_TRUE(Cond, false)
//! It must evaluate to false statement, if not test will fail but will continue execution.
#define MTEST_CHECK_FALSE(Cond) MTEST_DETAIL_CHECK_FALSE(Cond, false)
//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK(Cond) MTEST_DETAIL_CHECK_TRUE(Cond, false)
//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_VALUE(Value, Wanted) MTEST_DETAIL_CHECK_VALUE(Value, Wanted, false)
//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NOT_VALUE(Value, Wanted) MTEST_DETAIL_CHECK_NOT_VALUE(Value, Wanted, false)
//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NULL(Value) MTEST_DETAIL_CHECK_NULL(Value, false)
//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NOT_NULL(Value) MTEST_DETAIL_CHECK_NOT_NULL(Value, false)

//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_TRUE(Cond) MTEST_DETAIL_CHECK_TRUE(Cond, true)
//! Must evaluate to false statement, if not test will fail and will exit.
#define MTEST_ASSERT_FALSE(Cond) MTEST_DETAIL_CHECK_FALSE(Cond, true)
//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT(Cond) MTEST_DETAIL_CHECK_TRUE(Cond, true)
//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_VALUE(Value, Wanted) MTEST_DETAIL_CHECK_VALUE(Value, Wanted, true)
//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NOT_VALUE(Value, Wanted) MTEST_DETAIL_CHECK_NOT_VALUE(Value, Wanted, true)
//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NULL(Value) MTEST_DETAIL_CHECK_NULL(Value, true)
//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NOT_NULL(Value) MTEST_DETAIL_CHECK_NOT_NULL(Value, true)

//// Logs & Utility

//! Skip test
#define MTEST_SKIP(Reason) MTest::ITestManager::GetActiveCase()->Skip( Reason )

//! Print some information to stdout.
#define MTEST_INFO(Msg) MTest::CLog::Instance() << MTest::EConsoleColor::Default << "[Message] " << Msg << "\n"

//// Test Setup

#define MTEST_DETAIL_UNIT_TEST_FX(Section, Name, ParentFixture, ConcreteFixture) \
namespace MTest::Internal \
{ \
    struct ConcreteFixture final : public ParentFixture \
    { \
        void MTest_TestMethod(); \
    }; \
    SRegistrar MTEST_MACRO_CONCAT(MTest_Registrar_, __COUNTER__) \
    { \
        []() \
        { \
            auto fixture = std::make_unique<ConcreteFixture>(); \
            TestCallback callback = std::bind(&ConcreteFixture::MTest_TestMethod, fixture.get()); \
            CTestManager::Instance().Add<CTestManager::CUnitTest> \
            ( \
                #Section, #Name, std::source_location::current(), \
                std::move(callback), std::move(fixture) \
            ); \
        } \
    }; \
} \
void MTest::Internal::ConcreteFixture::MTest_TestMethod()

//! Define test case, give section name, test case name and fixture name. Tests are executed by section, test case name
//! must be unique in given section.
#define MTEST_UNIT_TEST_FX(Section, Name, ParentFixture) MTEST_DETAIL_UNIT_TEST_FX(Section, Name, ParentFixture, MTEST_MACRO_CONCAT(MTest_##ParentFixture, __COUNTER__))

//! Define test case, give section name, test case name. Fixture name is deffered from section name eg. 'Section' + Fixture
//! Tests are executed by section, test case name must be unique in given section.
#define MTEST_UNIT_TEST_F(Section, Name) MTEST_UNIT_TEST_FX(Section, Name, Section##Fixture )

//! Define test case, give section name and test case name. Tests are executed by section, test case name
//! must be unique in given section.
#define MTEST_UNIT_TEST(Section, Name) MTEST_UNIT_TEST_FX(Section, Name, Fixture)

//! Add STD Logger
#define MTEST_IMPLEMENT_STD_LOGGER MTest::CLog::Instance().AddWriter<MTest::CStdWriter>()
//! Add File Logger
#define MTEST_IMPLEMENT_FILE_LOGGER(File) MTest::CLog::Instance().AddWriter<MTest::CFileWriter>( File )
//! Use to launch unit test application.
#define MTEST_IMPLEMENT_MAIN MTest::CTestManager::Instance().Run()

//! Use to Implement Unit test main().
#define MTEST_MAIN \
int main(int, char*[]) \
{ \
    MTEST_IMPLEMENT_STD_LOGGER; \
    MTEST_IMPLEMENT_MAIN; \
	return 0; \
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

    namespace Details
    {
        inline std::string GetFileName(const std::string& Path)
        {
            const auto Pos = Path.find_last_of("\\/");
            if( Pos == std::string::npos )
            {
                return Path;
            }
            else
            {
                return Path.substr(Pos+1u);
            }
        }

        inline std::string FormatTime(const float timeInMS)
        {
            const bool toSeconds = timeInMS >= 100.0f;
            std::ostringstream Stream;
            Stream << "(";
            Stream << (toSeconds ? timeInMS/1000.0f : timeInMS);
            Stream << (toSeconds ? " s" : " ms");
            Stream << ")";
            return Stream.str();
        }

        #ifdef MTEST_WINDOWS_PLATFORM
        // https://learn.microsoft.com/en-us/dotnet/api/system.consolecolor?view=net-7.0
        inline int ToWin32Color(const EConsoleColor x)
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
        inline std::string ToAnsiColor(const EConsoleColor x)
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
    }

    inline std::ostream& operator<<(std::ostream& Stream, const EConsoleColor x)
    {
        #ifdef MTEST_WINDOWS_PLATFORM
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Details::ToWin32Color(x));
        #else
        Stream << Details::ToAnsiColor(x);
        #endif // MTEST_WINDOWS_PLATFORM
        return Stream;
    }

    class IWriter
    {
    public:
        IWriter() {}
        virtual ~IWriter() {}

        virtual void Write(const EConsoleColor) = 0;
        virtual void Write(const std::string&) = 0;
    };

    class CStdWriter: public IWriter
    {
    public:
        void Write(const EConsoleColor x) override { std::cout << x; }
        void Write(const std::string& x) override { std::cout << x; }
    };

    class CFileWriter: public IWriter
    {
    public:
        CFileWriter(const std::string& path):
            Stream(path)
        {
        }

        void Write(const EConsoleColor) override
        {
            // Nothing
        }

        void Write(const std::string& x) override
        {
            if( Stream.is_open() && Stream.good() )
            {
                Stream << x;
            }
        }
    private:
        std::ofstream Stream;
    };

    class CLog final
    {
    public:
        static CLog& Instance()
        {
            static CLog Log;
            return Log;
        }

        template<std::derived_from<IWriter> T, class...Args>
        void AddWriter(Args&&...args)
        {
            Writers.emplace_back( std::make_unique<T>(std::forward<Args>(args)...) );
        }

        CLog& operator<<(const std::string& Value)
        {
            for(const auto& i: Writers)
            {
                i->Write(Value);
            }
            return *this;
        }

        CLog& operator<<(const char* Value)
        {
            for(const auto& i: Writers)
            {
                i->Write(Value);
            }
            return *this;
        }

        CLog& operator<<(const EConsoleColor Value)
        {
            for(const auto& i: Writers)
            {
                i->Write(Value);
            }
            return *this;
        }

        template<class T>
        CLog& operator<<(const T& Value)
        {
            for(const auto& i: Writers)
            {
                Stream << Value;
                i->Write(Stream.str());
                Stream.str({});
                Stream.clear();
            }
            return *this;
        }
    private:
        std::vector<std::unique_ptr<IWriter>> Writers;
        std::ostringstream Stream;
    };

    struct SCheckInfo
    {
        SCheckInfo(const bool isAssert, const std::string& condition, const std::source_location& location):
            IsAssert(isAssert),
            Condition(condition),
            File(Details::GetFileName(location.file_name())),
            Line(location.line())
        {
        }

        bool IsAssertion() const { return IsAssert; }
        const std::string& GetCondition() const { return Condition; }
        const std::string& GetFile() const { return File; }
        std::uint_least32_t GetLine() const { return Line; }

        //! Gets formated string for display
        std::string ToString() const
        {
            std::ostringstream stream;
            stream << (IsAssert ? "[Assert ] " : "[Check  ] ") << Condition  << ", File: " << File << ", Line: " << Line;
            return stream.str();
        }

        bool IsAssert;
        std::string Condition;
        std::string File;
        std::uint_least32_t Line;
    };

    inline std::ostream& operator<<(std::ostream& Stream, const SCheckInfo& Info)
    {
        Stream << Info.ToString();
        return Stream;
    }

    class CTestAssertionException: public std::logic_error
    {
    public:
        CTestAssertionException():
            std::logic_error("Test Assertion")
        {
        }
    };

    class CTestSkippedException: public std::logic_error
    {
    public:
        CTestSkippedException():
            std::logic_error("Test Skipped")
        {
        }
    };

    //! Can be used to init some data before each test. Test function has access to its internals (excluding private) - prefer structs (public default access).
    struct Fixture
    {
        Fixture() = default;
        virtual ~Fixture() = default;

        //! Should return true if test case should be skipped.
        virtual bool Skip() { return false; }
        //! Use Assertions to check state.
        virtual void Setup() {}
        //! Cleanup if needed.
        virtual void Cleanup() {}
    };

    //! Fixture to use with Table tests.
    template<class DataType>
    struct TableFixture: public Fixture
    {
        TableFixture() = default;
        virtual ~TableFixture() = default;

        //! Get Test case name
        virtual std::string GetTestName(const DataType&, const std::size_t i) { return std::to_string(i); }
        //! Should return true if test case should be skipped.
        virtual bool Skip(const DataType&, const std::size_t) { return Skip(); }
        //! Use Assertions to check state.
        virtual void Setup(const DataType&, const std::size_t) { Setup(); }
        //! Cleanup if needed.
        virtual void Cleanup(const DataType&, const std::size_t) { Cleanup(); }
    };

    using FixturePtr = std::unique_ptr<Fixture>;
    using TestCallback = std::function<void()>;

    class ITestCase
    {
        using TestClock = std::chrono::steady_clock;
        using TestClockStamp = std::chrono::steady_clock::time_point;
        using TestClockDuration = std::chrono::duration<float, std::milli>;
    public:
        ITestCase(const std::string& section, const std::string& name, const std::source_location& location, TestCallback&& callback, FixturePtr fixture):
            Section(section),
            Name(name),
            File(Details::GetFileName(location.file_name())),
            Line(location.line()),
            Callback(std::move(callback)),
            Fixture(std::move(fixture))
        {
        }
        virtual ~ITestCase() = default;

        ITestCase(const ITestCase&) = delete;
        ITestCase& operator=(const ITestCase&) = delete;

        ITestCase(ITestCase&&) = default;
        ITestCase& operator=(ITestCase&&) = default;

        virtual void Run() = 0;

        const std::string& GetSection() const { return Section; }
        const std::string& GetName() const { return Name; }
        const std::string& GetFile() const { return File; }
        std::uint_least32_t GetLine() const { return Line; }
        bool IsFailed() const { return Failed; }
        const std::vector<SCheckInfo>& GetFailedChecks() const { return FailedChecks; }
        const std::optional<SCheckInfo>& GetFailedAssertion() const { return FailedAssertion; }
        Fixture* GetFixture() const { return Fixture.get(); }
        bool IsSkipped() const { return Skipped; }
        float GetDuration() const { return Duration; }

        std::string GetDisplayString() const { return MakeDisplayString(Section, Name); }
        static std::string MakeDisplayString(const std::string& section, const std::string& name)
        {
            return section + "." + name;
        }

        //! Returns true when pass
        bool Check(const std::string& Condition, const bool IsSuccess, const bool IsAssertion, std::source_location Location = std::source_location::current())
        {
            if( !IsSuccess )
            {
                MarkFailed();
                const SCheckInfo Info{IsAssertion, Condition, Location};
                CLog::Instance() << MTest::EConsoleColor::Red << Info << MTest::EConsoleColor::Default << "\n";
                if( !IsAssertion )
                {
                    FailedChecks.push_back(Info);
                }
                else
                {
                    FailedAssertion = Info;
                    throw CTestAssertionException{};
                }
                return false;
            }
            return true;
        }

        void Skip(const std::string& Reason)
        {
            MarkSkipped();
            CLog::Instance() << EConsoleColor::Magenta << "[Skipped] " << Reason << EConsoleColor::Default << "\n";
            throw CTestSkippedException{};
        }

        void OnException(const std::string& What)
        {
            MarkFailed();
            CLog::Instance() << EConsoleColor::Red << "[Fatal  ] " << What << EConsoleColor::Default << "\n";
        }

        void OnExecutionStart()
        {
            Start = TestClock::now();
            CLog::Instance() << EConsoleColor::Blue << "[Start  ] " << GetDisplayString() << EConsoleColor::Default << "\n";
        }

        void OnExecutionEnd()
        {
            Duration = TestClockDuration(TestClock::now()-Start).count();
            PrintResult(false);
        }

        void PrintResult(const bool fromManager)
        {
            if( IsFailed() )
            {
                CLog::Instance() << EConsoleColor::Red << "[Failure] " << GetDisplayString();
            }
            else if( IsSkipped() )
            {
                CLog::Instance() << EConsoleColor::Magenta << "[Skipped] " << GetDisplayString();
            }
            else
            {
                CLog::Instance() << EConsoleColor::Green << "[Success] " << GetDisplayString();
            }
            //
            if( !fromManager )
            {
                CLog::Instance() << " " << Details::FormatTime(Duration);
            }
            else
            {
                CLog::Instance() << ", File: " << GetFile() << ", Line: " << GetLine();
            }
            //
            CLog::Instance() << EConsoleColor::Default << "\n";
        }
    protected:
        void MarkFailed()
        {
            Failed = true;
        }

        void MarkSkipped()
        {
            Skipped = true;
        }

        template<class F>
        void Runner(F Function)
        {
            try
            {
                Function();
            }
            catch(const CTestAssertionException&) {}
            catch(const CTestSkippedException&) {}
            catch(const std::exception& Exception)
            {
                OnException(Exception.what());
            }
            catch(...)
            {
                OnException("Unknown");
            }
        }
    protected:
        std::string Section;
        std::string Name;
        std::string File;
        std::uint_least32_t Line;
        std::vector<SCheckInfo> FailedChecks;
        std::optional<SCheckInfo> FailedAssertion;
        bool Failed = false;
        TestCallback Callback;
        FixturePtr Fixture;
        bool Skipped = false;
        TestClockStamp Start = {};
        float Duration = 0.0f; //! In miliseconds
    };

    //! Common Functionality that can be used to implement eg. Integration Tests
    class ITestManager
    {
    public:
        ITestManager()
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Details::ToWin32Color(EConsoleColor::Default));
            #endif // MTEST_WINDOWS_PLATFORM
        }

        virtual ~ITestManager()
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), csbi.wAttributes);
            #endif // MTEST_WINDOWS_PLATFORM
        }

        ITestManager(const ITestManager&) = delete;
        ITestManager& operator=(const ITestManager&) = delete;

        ITestManager(ITestManager&&) = delete;
        ITestManager& operator=(ITestManager&&) = delete;

        virtual void Run() = 0;

        template<std::derived_from<ITestCase> T>
        void Add(const std::string& Section, const std::string& Name, const std::source_location& location, TestCallback&& callback, FixturePtr fixture)
        {
            auto& SectionContainer = Sections[Section];
            auto It = std::find_if(SectionContainer.begin(), SectionContainer.end(), [&](const std::unique_ptr<ITestCase>& Other)
            {
                return Other->GetName() == Name;
            });
            if( It != SectionContainer.end() )
            {
                CLog::Instance() << EConsoleColor::Red << "[Error  ] " << ITestCase::MakeDisplayString(Section, Name) << " Already Exists"
                    << EConsoleColor::Default << "\n";
                return;
            }
            auto tmp = std::make_unique<T>(Section, Name, location, std::move(callback), std::move(fixture));
            if( !tmp->GetFixture() )
            {
                CLog::Instance() << EConsoleColor::Red << "[Error  ] Invalid fixture in: " << tmp->GetDisplayString() << "\n";
                return;
            }
            SectionContainer.emplace_back(std::move(tmp));
        }

        void Clear()
        {
            Sections.clear();
            SuccessfulTestCases.clear();
            FailedTestCases.clear();
            SkippedTestCases.clear();
            HasTests = false;
            SectionTime = 0.0f;
            TotalTime = 0.0f;
            SetActiveCase(nullptr);
        }

        std::size_t GetSectionTestCount(const std::string& section) const
        {
            if( !Sections.count(section) )
            {
                return 0u;
            }
            return Sections.at(section).size();
        }

        const auto& GetSections() const { return Sections; }
        bool HasAnyTest() const { return HasTests; }
        float GetTotalTime() const { return TotalTime; }

        void OnExecutionStart()
        {
            // Use literal when migrate to C++23 - 0uz
            std::size_t testCount = std::accumulate(Sections.begin(), Sections.end(), std::size_t{0}, [](const std::size_t val, const auto& item)
            {
                return val + item.second.size();
            });
            HasTests = (testCount != 0u);
            CLog::Instance() << EConsoleColor::Blue << "[Manager] Running " << testCount  << " tests grouped into "
                << Sections.size() << " sections" << EConsoleColor::Default << "\n";
        }

        void OnSectionStart(const std::string& name)
        {
            SectionTime = 0.0f;
            CLog::Instance() << EConsoleColor::Blue << "[-------] Running section " << name << " which has " << GetSectionTestCount(name) << " tests"
                << EConsoleColor::Default << "\n";
        }

        void UpdateStatus(ITestCase* testCase)
        {
            if( testCase )
            {
                SectionTime += testCase->GetDuration();
                TotalTime += testCase->GetDuration();
                if( testCase->IsFailed() )
                {
                    FailedTestCases.push_back( testCase );
                }
                else if( testCase->IsSkipped() )
                {
                    SkippedTestCases.push_back( testCase );
                }
                else
                {
                    SuccessfulTestCases.push_back( testCase );
                }
            }
        }

        void OnSectionEnd(const std::string& name)
        {
            CLog::Instance() << EConsoleColor::Blue << "[-------] Section " << name << " finished " << Details::FormatTime(SectionTime) << EConsoleColor::Default << "\n";
        }

        void OnExecutionEnd()
        {
            CLog::Instance() << EConsoleColor::Blue << "[Manager] Running finished " << Details::FormatTime(TotalTime) << EConsoleColor::Default << "\n";
            if( HasAnyTest() )
            {
                CLog::Instance() << EConsoleColor::Blue << "[Manager] Test summary:" << EConsoleColor::Default << "\n";
                PrintTestResults(SkippedTestCases, EConsoleColor::Magenta, "Skipped");
                PrintTestResults(FailedTestCases, EConsoleColor::Red, "Failed");
                PrintTestResults(SuccessfulTestCases, EConsoleColor::Green, "Successful");
            }
            else
            {
                CLog::Instance() << EConsoleColor::Blue << "[Manager] No Test Run or Registered" << EConsoleColor::Default << "\n";
            }
        }

        void PrintTestResults(const std::vector<ITestCase*>& array, const EConsoleColor color, const std::string& caption)
        {
            if( array.size() )
            {
                CLog::Instance() << color << "[Manager] " << caption << " Unit Tests: " << array.size() << EConsoleColor::Default << "\n";
                for(const auto& i: array)
                {
                    i->PrintResult(true);
                }
            }
        }

        static void SetActiveCase(ITestCase* testCase)
        {
            ActiveCase = testCase;
        }

        static ITestCase* GetActiveCase()
        {
            return ActiveCase;
        }

        template<std::derived_from<ITestCase> T>
        static T* GetActiveCaseAs()
        {
            return dynamic_cast<T*>(ActiveCase);
        }
    protected:
        std::unordered_map<std::string, std::vector<std::unique_ptr<ITestCase>>> Sections;
        std::vector<ITestCase*> SuccessfulTestCases;
        std::vector<ITestCase*> FailedTestCases;
        std::vector<ITestCase*> SkippedTestCases;
        bool HasTests = false;
        float SectionTime = 0.0f;
        float TotalTime = 0.0f;
        static inline ITestCase* ActiveCase = nullptr;
    private:
        #ifdef MTEST_WINDOWS_PLATFORM
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        #endif // MTEST_WINDOWS_PLATFORM
    };

    //! Unit test manager
    class CTestManager final: public ITestManager
    {
    public:
        class CUnitTest final: public ITestCase
        {
        public:
            CUnitTest(const std::string& section, const std::string& name, const std::source_location& location, TestCallback&& callback, FixturePtr fixture):
                ITestCase(section, name, location, std::move(callback), std::move(fixture))
            {
            }

            void Run() override
            {
                OnExecutionStart();
                //
                Runner([&]()
                {
                    if( Fixture->Skip() )
                    {
                        Skip("Test case is skipped by Fixture");
                    }
                    Fixture->Setup();
                    //
                    Callback();
                });
                //
                Fixture->Cleanup();
                //
                OnExecutionEnd();
            }
        };
    public:
        static CTestManager& Instance()
        {
            static CTestManager Manager;
            return Manager;
        }

        void Run() override
        {
            OnExecutionStart();
            CLog::Instance() << "\n";
            for(const auto& i: Sections)
            {
                OnSectionStart(i.first);
                for(const auto& j: i.second)
                {
                    auto testCase = j.get();
                    SetActiveCase(testCase);
                    testCase->Run();
                    SetActiveCase(nullptr);
                    //
                    UpdateStatus(testCase);
                }
                OnSectionEnd(i.first);
                CLog::Instance() << "\n";
            }
            OnExecutionEnd();
            //
            Clear();
        }
    private:
        CTestManager() = default;
        ~CTestManager() = default;
    };

    struct SRegistrar
    {
        SRegistrar(const std::function<void()>& callback)
        {
            if( callback )
            {
                callback();
            }
        }
    };
}
