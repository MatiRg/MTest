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

// _WIN64   - 64 bit
// _WIN32   - 32 i 64 bit
// _M_X64   - x64 Visual Studio
// __CYGWIN__   - Windows with Cygwin (POSIX)
//#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
// __linux__     - Linux kernel
// __x86_64__    - x64 GNUC
//#elif defined(__LINUX__) || defined(linux) || defined(__linux) || defined(__linux__)

#if defined(_WIN64) || defined(_WIN32)
    #define MTEST_WINDOWS_PLATFORM 1
#elif defined(__linux__) || defined(linux)
    #define MTEST_LINUX_PLATFORM 1
#else
    #error "Unsupported platform"
#endif

#ifdef MTEST_WINDOWS_PLATFORM
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif // MTEST_WINDOWS_PLATFORM

// Macros for output
#ifndef MTEST_LOG
#define MTEST_LOG MTest::CLog::Instance()
#endif // MTEST_LOG

#ifndef MTEST_NEW_LINE
#define MTEST_NEW_LINE "\n"
#endif // MTEST_NEW_LINE

//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_TRUE(Cond) MTest::ITestManager::GetActiveCase()->Check( #Cond , (Cond) )

//! It must evaluate to false statement, if not test will fail but will continue execution.
#define MTEST_CHECK_FALSE(Cond) MTest::ITestManager::GetActiveCase()->Check( #Cond , !(Cond) )

//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK(Cond) MTEST_CHECK_TRUE(Cond)

//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NULL(Value) MTEST_CHECK(Value == nullptr)

//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_NOT_NULL(Value) MTEST_CHECK(Value != nullptr)

//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_TRUE(Cond) MTest::ITestManager::GetActiveCase()->Assert( #Cond , (Cond) )

//! Must evaluate to false statement, if not test will fail and will exit.
#define MTEST_ASSERT_FALSE(Cond) MTest::ITestManager::GetActiveCase()->Assert( #Cond , !(Cond) )

//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT(Cond) MTEST_ASSERT_TRUE(Cond)

//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NULL(Value) MTEST_ASSERT(Value == nullptr)

//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_NOT_NULL(Value) MTEST_ASSERT(Value != nullptr)

//

//! Print some information to stdout.
#define MTEST_INFO(Msg) MTEST_LOG << MTest::EConsoleColor::Default << "[Message] " << Msg << MTEST_NEW_LINE

// For No Fixture Tests
#define MTEST_DETAIL_GENERATE_TEST_NAME(Section, Name) Section##Name##TestFunction
#define MTEST_DETAIL_GENERATE_TEST_NAME_FIXTURE(Section, Name) Section##Name##TestMethod
#define MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name) CFixtureImpl##Section##Name
#define MTEST_DETAIL_GENERATE_TEST_FIXTURE_FULL_NAME(Section, Name) MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name) :: MTEST_DETAIL_GENERATE_TEST_NAME_FIXTURE(Section, Name)

//! Define test case, give section name, test case name and fixture name. Tests are executed by section, test case name
//! must be unique in given section.
#define MTEST_UNIT_TEST_FX(Section, Name, Fixture) \
namespace MTest \
{ \
    class MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name): public Fixture \
    { \
    public: \
        void MTEST_DETAIL_GENERATE_TEST_NAME_FIXTURE(Section, Name)(); \
    }; \
    STestProxy ut##Section##Name##Inst( #Section, #Name , std::source_location::current() , \
        CTestManager::CreateUnitTestFixtureInfoPair< MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name) >( &MTEST_DETAIL_GENERATE_TEST_FIXTURE_FULL_NAME(Section, Name) ) ); \
} \
void MTest:: MTEST_DETAIL_GENERATE_TEST_FIXTURE_FULL_NAME(Section, Name)()

//! Define test case, give section name, test case name. Fixture name is deffered from section name eg. C + 'Section Name' + Fixture
//! Tests are executed by section, test case name must be unique in given section.
#define MTEST_UNIT_TEST_F(Section, Name) MTEST_UNIT_TEST_FX(Section, Name, C##Section##Fixture )

//! Define test case, give section name and test case name. Tests are executed by section, test case name
//! must be unique in given section.
#define MTEST_UNIT_TEST(Section, Name) \
namespace MTest \
{ \
    void MTEST_DETAIL_GENERATE_TEST_NAME(Section, Name)(); \
    STestProxy ut##Section##Name##Inst( #Section, #Name , std::source_location::current() , \
        CTestManager::CreateUnitTestInfoPair( &MTEST_DETAIL_GENERATE_TEST_NAME(Section, Name) ) ); \
} \
void MTest:: MTEST_DETAIL_GENERATE_TEST_NAME(Section, Name)()

//! Use to launch unit test application.
#define MTEST_IMPLEMENT_MAIN MTest::CTestManager::Instance().Run()

//! Use to Implement Unit test main().
#define MTEST_MAIN \
int main(int, char*[]) \
{ \
    MTest::CLog::Instance().AddWriter<MTest::CStdWriter>(); \
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
        Blue
    };

    namespace Detail
    {
        inline std::string GetFileName(const std::string& Path)
        {
            const auto Pos = Path.find_last_of( "\\/" );
            if( Pos == std::string::npos )
            {
                return Path;
            }
            else
            {
                return Path.substr( Pos+1 );
            }
        }

        #ifdef MTEST_WINDOWS_PLATFORM
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
            case EConsoleColor::Default:
            default:
                return 15;
            }
        }
        #else
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
            case EConsoleColor::Default:
            default:
                return "\033[0m";
            }
        }
        #endif // MTEST_WINDOWS_PLATFORM
    }

    inline std::ostream& operator<<(std::ostream& Stream, const EConsoleColor x)
    {
        #ifdef MTEST_WINDOWS_PLATFORM
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Detail::ToWin32Color(x));
        #else
        Stream << Detail::ToAnsiColor(x);
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
            // Reuse
            static std::ostringstream stream;
            for(const auto& i: Writers)
            {
                stream << Value;
                i->Write( stream.str() );
                stream.str("");
                stream.clear();
            }
            return *this;
        }
    private:
        std::vector<std::unique_ptr<IWriter>> Writers;
    };

    struct SFailInfo
    {
        SFailInfo(const bool isAssert, const std::string& condition, const std::source_location& location):
            IsAssert(isAssert),
            Condition(condition),
            File(Detail::GetFileName(location.file_name())),
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
            const std::string Prefix = IsAssert ? "[ Abort ] " : "[ Check ] ";
            std::ostringstream stream;
            stream << Prefix << Condition  << ", File: " << File << ", Line: " << Line;
            return stream.str();
        }

        bool IsAssert;
        std::string Condition;
        std::string File;
        std::uint_least32_t Line;
    };

    inline std::ostream& operator<<(std::ostream& Stream, const SFailInfo& Info)
    {
        Stream << Info.ToString();
        return Stream;
    }

    class CTestAbortedException: public std::logic_error
    {
    public:
        CTestAbortedException():
            std::logic_error("Test Aborted")
        {
        }
    };

    //! Can be used to init some data before each test. Test function has access to its internals (excluding private).
    class IFixture
    {
    public:
        IFixture() = default;
        virtual ~IFixture() = default;

        //! Should return false if something bad happens.
        virtual bool Setup() { return true; }
        //! Cleanup if needed.
        virtual void Cleanup() {}
    };

    using FixturePtr = std::unique_ptr<IFixture>;
    using UnitTestCallback = std::function<void()>;
    using UnitTestInfoPair = std::pair<FixturePtr, UnitTestCallback>;

    class ITestCase
    {
    public:
        ITestCase(const std::string& section, const std::string& name, const std::source_location& location):
            Section(section),
            Name( name ),
            File( Detail::GetFileName(location.file_name()) ),
            Line( location.line() )
        {
        }

        virtual ~ITestCase() = default;

        ITestCase(const ITestCase&) = delete;
        ITestCase& operator=(const ITestCase&) = delete;

        ITestCase(ITestCase&&) = default;
        ITestCase& operator=(ITestCase&&) = default;

        const std::string& GetSection() const
        {
            return Section;
        }

        const std::string& GetName() const
        {
            return Name;
        }

        const std::string& GetFile() const
        {
            return File;
        }

        std::uint_least32_t GetLine() const
        {
            return Line;
        }

        bool IsFailed() const
        {
            return Failed;
        }

        const std::vector<SFailInfo>& GetFailedChecks() const
        {
            return FailedChecks;
        }

        const std::optional<SFailInfo>& GetFailedAssertion() const
        {
            return FailedAssertion;
        }

        std::string GetDisplayString() const
        {
            return MakeDisplayString(Section, Name);
        }

        static std::string MakeDisplayString(const std::string& section, const std::string& name)
        {
            return section + "." + name;
        }

        //! Returns true when pass
        bool Check(const std::string& Condition, const bool IsSuccess, std::source_location Location = std::source_location::current())
        {
            if( !IsSuccess )
            {
                Fail();
                FailedChecks.push_back( SFailInfo{false, Condition, Location} );
                MTEST_LOG << MTest::EConsoleColor::Yellow << FailedChecks.back() << MTest::EConsoleColor::Default << MTEST_NEW_LINE;
                return false;
            }
            return true;
        }

        void Assert(const std::string& Condition, const bool IsSuccess, std::source_location Location = std::source_location::current())
        {
            if( !IsSuccess )
            {
                Fail();
                FailedAssertion = SFailInfo{true, Condition, Location};
                MTEST_LOG << EConsoleColor::Red << FailedAssertion.value() << EConsoleColor::Default << MTEST_NEW_LINE;
                throw CTestAbortedException();
            }
        }

        void OnException(const std::string& What)
        {
            Fail();
            MTEST_LOG << EConsoleColor::Red << "[Failure] " << What << EConsoleColor::Default << MTEST_NEW_LINE;
        }

        void PrintTestInfo()
        {
            MTEST_LOG << EConsoleColor::Blue << "[ Start ] " << GetDisplayString() << EConsoleColor::Default << MTEST_NEW_LINE;
        }

        void PrintTestResult()
        {
            if( !IsFailed() )
            {
                MTEST_LOG << EConsoleColor::Green << "[Success] " << GetDisplayString() << EConsoleColor::Default << MTEST_NEW_LINE;
            }
            else
            {
                MTEST_LOG << EConsoleColor::Red << "[Failure] " << GetDisplayString() << EConsoleColor::Default << MTEST_NEW_LINE;
            }
        }
    protected:
        void Fail()
        {
            Failed = true;
        }

        template<class F>
        void PassRunner(F Callback)
        {
            try
            {
                Callback();
            }
            catch(const CTestAbortedException&) // Do Nothing
            {
            }
            catch(const std::exception& Exception)
            {
                OnException( Exception.what() );
            }
            catch(...)
            {
                OnException( "Unknown" );
            }
        }
    protected:
        std::string Section;
        std::string Name;
        std::string File;
        std::uint_least32_t Line;
        std::vector<SFailInfo> FailedChecks;
        std::optional<SFailInfo> FailedAssertion;
        bool Failed = false;
    };

    //! Common Functionality that can be used to implement eg. Integration Tests
    class ITestManager
    {
    public:
        ITestManager()
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Detail::ToWin32Color(EConsoleColor::Default));
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

        template<std::derived_from<ITestCase> T, class...Args>
        void Add(const std::string& Section, const std::string& Name, const std::source_location& location, Args&&...args)
        {
            auto& SectionContainer = Sections[Section];
            auto It = std::find_if(SectionContainer.begin(), SectionContainer.end(), [&](const std::unique_ptr<ITestCase>& Other)
            {
                return Other->GetName() == Name;
            });
            if( It != SectionContainer.end() )
            {
                MTEST_LOG << EConsoleColor::Red << "[ Error ] " << ITestCase::MakeDisplayString(Section, Name) << " Already Exists"
                    << EConsoleColor::Default << MTEST_NEW_LINE;
                return;
            }
            SectionContainer.emplace_back( std::make_unique<T>( Section, Name, location, std::forward<Args>(args)... ) );
        }

        void UpdateCaseStatus(ITestCase* testCase)
        {
            if( testCase )
            {
                if( testCase->IsFailed() )
                {
                    FailedTestCases.push_back( testCase );
                }
            }
        }

        void Clear()
        {
            Sections.clear();
            FailedTestCases.clear();
            ActiveCase = nullptr;
        }

        std::size_t GetSectionTestCount(const std::string& section) const
        {
            if( !Sections.count(section) )
            {
                return 0u;
            }
            return Sections.at(section).size();
        }

        const std::unordered_map<std::string, std::vector<std::unique_ptr<ITestCase>>>& GetSections() const
        {
            return Sections;
        }

        void PrintStart()
        {
            int testCount = 0;
            for(const auto& i: Sections)
            {
                testCount += i.second.size();
            }
            MTEST_LOG << EConsoleColor::Blue << "[Manager] Running " << testCount  << " tests grouped into "
                      << Sections.size() << " sections" << EConsoleColor::Default << MTEST_NEW_LINE;
        }

        void PrintSectionInfo(const std::string& name)
        {
            MTEST_LOG << EConsoleColor::Blue << "[-------] Running section " << name << " which has " << GetSectionTestCount(name) << " tests" << EConsoleColor::Default << MTEST_NEW_LINE;
        }

        void PrintSummary()
        {
            if( FailedTestCases.size() )
            {
                MTEST_LOG << EConsoleColor::Red << "[Manager] Failed Unit Tests: " << FailedTestCases.size() << EConsoleColor::Default << MTEST_NEW_LINE;
                for(const auto& i: FailedTestCases)
                {
                    MTEST_LOG << EConsoleColor::Red << "[Failure] " << i->GetDisplayString() << ", File: " <<
                        i->GetFile() << ", Line: " << i->GetLine() << EConsoleColor::Default << MTEST_NEW_LINE;
                }
            }
            else
            {
                MTEST_LOG << EConsoleColor::Green << "[Manager] All Passed" << EConsoleColor::Default << MTEST_NEW_LINE;
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
        std::vector<ITestCase*> FailedTestCases;
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
            CUnitTest(const std::string& section, const std::string& name, const std::source_location& location, UnitTestInfoPair&& infoPair):
                ITestCase(section, name, location),
                Fixture( std::move(infoPair.first) ),
                Func( std::move(infoPair.second) )
            {
            }

            void Run()
            {
                PrintTestInfo();
                //
                PassRunner([&]()
                {
                    if( Fixture && !Fixture->Setup() )
                    {
                        throw std::runtime_error{"Error during fixture setup"};
                    }
                    //
                    Func();
                });
                //
                if( Fixture )
                {
                    Fixture->Cleanup();
                }
                //
                PrintTestResult();
            }
        private:
            FixturePtr Fixture;
            UnitTestCallback Func;
        };
    public:
        static CTestManager& Instance()
        {
            static CTestManager Manager;
            return Manager;
        }

        template<class F>
        static UnitTestInfoPair CreateUnitTestInfoPair(F func)
        {
            return { FixturePtr{}, UnitTestCallback{func} };
        }

        template<class C, class F>
        static UnitTestInfoPair CreateUnitTestFixtureInfoPair(F func)
        {
            static_assert(std::is_base_of<IFixture, C>::value, "Must be base of IFixture");
            std::unique_ptr<C> fixture = std::make_unique<C>();
            UnitTestCallback callback = std::bind(func, fixture.get());
            return {std::move(fixture), callback};
        }

        void Run()
        {
            PrintStart();
            MTEST_LOG << MTEST_NEW_LINE;
            for(const auto& i: Sections)
            {
                PrintSectionInfo(i.first);
                for(const auto& j: i.second)
                {
                    if( CUnitTest* testCase = dynamic_cast<CUnitTest*>(j.get()); testCase != nullptr )
                    {
                        ActiveCase = testCase;
                        testCase->Run();
                        ActiveCase = nullptr;
                        //
                        UpdateCaseStatus( testCase );
                    }
                }
                PrintSectionInfo(i.first);
                MTEST_LOG << MTEST_NEW_LINE;
            }
            PrintSummary();
            //
            Clear();
        }
    private:
        CTestManager() = default;
        ~CTestManager()  = default;
    };

    struct STestProxy
    {
        STestProxy(const std::string& Section, const std::string& Name, const std::source_location& location, UnitTestInfoPair&& infoPair)
        {
            CTestManager::Instance().Add<CTestManager::CUnitTest>( Section, Name, location, std::forward<UnitTestInfoPair>(infoPair) );
        }
    };
}
