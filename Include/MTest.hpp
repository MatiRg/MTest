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
#define MTEST_LOG std::cout
#endif // MTEST_LOG

#ifndef MTEST_NEW_LINE
#define MTEST_NEW_LINE std::endl
#endif // MTEST_NEW_LINE

//! Print some information to stdout.
#define MTEST_INFO(Msg) MTEST_LOG << MTest::EConsoleColor::Default << "[Info]: " << Msg << MTEST_NEW_LINE

#define MTEST_DETAIL_ASSERT_BASE(Cond) throw MTest::CAssert( #Cond , __FILE__ , __LINE__ )

//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT_TRUE(Cond) \
do \
{ \
    if( !(Cond) ) \
    { \
        MTEST_DETAIL_ASSERT_BASE(Cond); \
    } \
} \
while(false)

//! Must evaluate to false statement, if not test will fail and will exit.
#define MTEST_ASSERT_FALSE(Cond) \
do \
{ \
    if( (Cond) ) \
    { \
        MTEST_DETAIL_ASSERT_BASE(Cond); \
    } \
} \
while(false)

//! Must evaluate to true statement, if not test will fail and will exit.
#define MTEST_ASSERT(Cond) MTEST_ASSERT_TRUE(Cond)

#define MTEST_DETAIL_CHECK_BASE(Cond) \
MTest::CTestManager::Instance().GetActiveUnitTest()->OnCheckFail(); \
MTEST_LOG << MTest::EConsoleColor::Yellow << MTest::CCheck( #Cond , __FILE__ , __LINE__ ) << MTest::EConsoleColor::Default << MTEST_NEW_LINE

//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK_TRUE(Cond) \
do \
{ \
    if( !(Cond) ) \
    { \
        MTEST_DETAIL_CHECK_BASE(Cond); \
    } \
} \
while(false)

//! It must evaluate to false statement, if not test will fail but will continue execution.
#define MTEST_CHECK_FALSE(Cond) \
do \
{ \
    if( (Cond) ) \
    { \
        MTEST_DETAIL_CHECK_BASE(Cond); \
    } \
} \
while(false)

//! It must evaluate to true statement, if not test will fail but will continue execution.
#define MTEST_CHECK(Cond) MTEST_CHECK_TRUE(Cond)

#define MTEST_DETAIL_GENERATE_TEST_NAME(Section, Name) Section##Name##TestMethod
#define MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name) CFixtureImpl##Section##Name
#define MTEST_DETAIL_GENERATE_TEST_FULL_NAME(Section, Name) MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name) :: MTEST_DETAIL_GENERATE_TEST_NAME(Section, Name)

//! Define test case, give section name, test case name and fixture name. Tests are executed by section, test case name
//! must be unique in given section.
#define MTEST_UNIT_TEST_F(Section, Name, Fixture) \
namespace MTest \
{ \
    class MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name): public Fixture \
    { \
    public: \
        void MTEST_DETAIL_GENERATE_TEST_NAME(Section, Name)(); \
    }; \
    STestProxy ut##Section##Name##Inst( #Section, #Name , __FILE__ , __LINE__ , \
        Detail::CreateUnitTestInfoPair< MTEST_DETAIL_GENERATE_TEST_FIXTURE_NAME(Section, Name) >( &MTEST_DETAIL_GENERATE_TEST_FULL_NAME(Section, Name) ) ); \
} \
void MTest:: MTEST_DETAIL_GENERATE_TEST_FULL_NAME(Section, Name)()

//! Define test case, give section name and test case name. Tests are executed by section, test case name
//! must be unique in given section.
#define MTEST_UNIT_TEST(Section, Name) MTEST_UNIT_TEST_F(Section, Name, MTest::IFixture)

//! Use to launch test application.
#define MTEST_IMPLEMENT_MAIN MTest::CTestManager::Instance().Run()

//! Use to Implement main().
#define MTEST_MAIN \
int main(int, char*[]) \
{ \
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

    class ICheck
    {
    public:
        ICheck(const std::string& condition, const std::string& file, const int line):
            Condition( condition ),
            File( file ),
            Line( line )
        {
        }

        virtual ~ICheck() = default;

        virtual std::string ToString() const
        {
            return "Failed: "+Condition+", File: "+Detail::GetFileName(File)+", Line: "+std::to_string(Line);
        }
    protected:
        std::string Condition;
        std::string File;
        int Line;
    };

    inline std::ostream& operator<<(std::ostream& Stream, const ICheck& Assert)
    {
        Stream << Assert.ToString();
        return Stream;
    }

    class CCheck: public ICheck
    {
    public:
        CCheck(const std::string& condition, const std::string& file, const int line):
            ICheck( condition, file, line )
        {
        }

        std::string ToString() const override
        {
            return "[Check]: " + ICheck::ToString();
        }
    };

    class CAssert: public ICheck
    {
    public:
        CAssert(const std::string& condition, const std::string& file, const int line):
            ICheck( condition, file, line )
        {
        }

        std::string ToString() const override
        {
            return "[Assert]: " + ICheck::ToString();
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

    class CTestManager final
    {
    public:
        class CUnitTest
        {
        public:
            CUnitTest(const std::string& section, const std::string& name, const std::string& file, const int line, UnitTestInfoPair&& infoPair):
                Section(section),
                Name( name ),
                File( file ),
                Line( line ),
                Fixture( std::move(infoPair.first) ),
                Func( std::move(infoPair.second) )
            {
            }

            CUnitTest(const CUnitTest&) = delete;
            CUnitTest(CUnitTest&&) = default;

            CUnitTest& operator=(const CUnitTest&) = delete;
            CUnitTest& operator=(CUnitTest&&) = default;

            void Run()
            {
                MTEST_LOG << MTEST_NEW_LINE;
                MTEST_LOG << EConsoleColor::Blue << "[Test]: " << Name << EConsoleColor::Default << MTEST_NEW_LINE;
                MTEST_LOG << EConsoleColor::Blue << "[Test]: File: " << Detail::GetFileName(File) << ", Line: " << Line << EConsoleColor::Default << MTEST_NEW_LINE;
                try
                {
                    if( !Fixture->Setup() )
                    {
                        throw std::logic_error{"Error during fixture setup"};
                    }
                    //
                    Func();
                }
                catch(const CAssert& As)
                {
                    Fail();
                    MTEST_LOG << EConsoleColor::Red << As << EConsoleColor::Default << MTEST_NEW_LINE;
                }
                catch(const std::exception& Exception)
                {
                    Fail();
                    MTEST_LOG << EConsoleColor::Red << "[Exception]: " << Exception.what() << EConsoleColor::Default << MTEST_NEW_LINE;
                }
                catch(...)
                {
                    Fail();
                    MTEST_LOG << EConsoleColor::Red << "[Exception]: Unknown" << EConsoleColor::Default << MTEST_NEW_LINE;
                }
                //
                if( !IsFailed() )
                {
                    MTEST_LOG << EConsoleColor::Green << "[Test]: " << Name << " - Passed" << EConsoleColor::Default << MTEST_NEW_LINE;
                }
                else
                {
                    if( !ChecksFailed )
                    {
                        MTEST_LOG << EConsoleColor::Red << "[Test]: " << Name << " - Failed" << EConsoleColor::Default << MTEST_NEW_LINE;
                    }
                    else
                    {
                        MTEST_LOG << EConsoleColor::Red << "[Test]: " << Name << " - Failed, Failed Checks - " << ChecksFailed << EConsoleColor::Default << MTEST_NEW_LINE;
                    }
                }
                //
                Fixture->Cleanup();
            }

            void OnCheckFail()
            {
                if( !Failed )
                {
                    Fail();
                }
                ++ChecksFailed;
            }

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

            std::string GetDisplayString() const
            {
                return MakeDisplayString(Section, Name);
            }

            int GetLine() const
            {
                return Line;
            }

            bool IsFailed() const
            {
                return Failed;
            }

            static std::string MakeDisplayString(const std::string& section, const std::string& name)
            {
                return section + "::" + name;
            }
        private:
            void Fail()
            {
                Failed = true;
            }
        private:
            std::string Section;
            std::string Name;
            std::string File;
            int Line;
            FixturePtr Fixture;
            UnitTestCallback Func;
            int ChecksFailed = 0;
            bool Failed = false;
        };
    public:
        static CTestManager& Instance()
        {
            static CTestManager Manager;
            return Manager;
        }

        CUnitTest* GetActiveUnitTest() const { return ActiveUnitTest; }

        void Add(const std::string& Section, const std::string& Name, const std::string& File, const int Line, UnitTestInfoPair&& infoPair)
        {
            auto& SectionContainer = Sections[Section];

            auto It = std::find_if(SectionContainer.begin(), SectionContainer.end(), [&](const std::unique_ptr<CUnitTest>& Other) {
                return Other->GetName() == Name;
            });

            if( It != SectionContainer.end() )
            {
                MTEST_LOG << EConsoleColor::Red << "[Error]: Test: " << CUnitTest::MakeDisplayString(Section, Name) << " Already Exists"
                    << EConsoleColor::Default << MTEST_NEW_LINE;
                return;
            }

            SectionContainer.emplace_back( std::make_unique<CUnitTest>(Section, Name, File, Line, std::forward<UnitTestInfoPair>(infoPair) ) );
        }

        void Run()
        {
            int UnitTestCount = 0;
            for(const auto& i: Sections)
            {
                UnitTestCount += i.second.size();
            }
            MTEST_LOG << EConsoleColor::Blue << "[Tests]: Count: " << UnitTestCount << EConsoleColor::Default << MTEST_NEW_LINE;

            for(const auto& i: Sections)
            {
                MTEST_LOG << MTEST_NEW_LINE;
                MTEST_LOG << EConsoleColor::Blue << "--------------------------------" << MTEST_NEW_LINE;
                MTEST_LOG << EConsoleColor::Blue << "[Section]: " << i.first << EConsoleColor::Default << MTEST_NEW_LINE;
                for(const auto& j: i.second)
                {
                    ActiveUnitTest = j.get();
                    j->Run();
                    ActiveUnitTest = nullptr;

                    if( j->IsFailed() )
                    {
                        FailedUnitTest.push_back( j.get() );
                    }
                }
                MTEST_LOG << EConsoleColor::Blue << "--------------------------------" << MTEST_NEW_LINE;
            }
            MTEST_LOG << MTEST_NEW_LINE;
            if( FailedUnitTest.size() )
            {
                MTEST_LOG << EConsoleColor::Red << "[Tests]: Failed Unit Tests: " << FailedUnitTest.size() << EConsoleColor::Default << MTEST_NEW_LINE;
                for(const auto& i: FailedUnitTest)
                {
                    MTEST_LOG << EConsoleColor::Red << "[Test]: " << i->GetDisplayString() << ", File: " <<
                        Detail::GetFileName(i->GetFile()) << ", Line: " << i->GetLine() << EConsoleColor::Default << MTEST_NEW_LINE;
                }
            }
            else
            {
                MTEST_LOG << EConsoleColor::Green << "[Tests]: All Passed" << EConsoleColor::Default << MTEST_NEW_LINE;
            }
        }

        CTestManager(const CTestManager&) = delete;
        CTestManager(CTestManager&&) = delete;
        CTestManager& operator=(const CTestManager&) = delete;
        CTestManager& operator=(CTestManager&&) = delete;
    private:
        CTestManager()
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            #endif // MTEST_WINDOWS_PLATFORM
        }

        ~CTestManager()
        {
            #ifdef MTEST_WINDOWS_PLATFORM
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), csbi.wAttributes);
            #endif // MTEST_WINDOWS_PLATFORM
        }
    private:
        std::unordered_map<std::string, std::vector<std::unique_ptr<CUnitTest>>> Sections;
        std::vector<CUnitTest*> FailedUnitTest;
        CUnitTest* ActiveUnitTest = nullptr;
        #ifdef MTEST_WINDOWS_PLATFORM
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        #endif // MTEST_WINDOWS_PLATFORM
    };

    namespace Detail
    {
        template<class C, class F>
        UnitTestInfoPair CreateUnitTestInfoPair(F func)
        {
            static_assert(std::is_base_of<IFixture, C>::value, "Must be base of IFixture");
            std::unique_ptr<C> fixture = std::make_unique<C>();
            UnitTestCallback callback = std::bind(func, fixture.get());
            return {std::move(fixture), callback};
        }
    }

    struct STestProxy
    {
        STestProxy(const std::string& Section, const std::string& Name, const std::string& File, const int Line, UnitTestInfoPair&& infoPair)
        {
            CTestManager::Instance().Add( Section, Name, File, Line, std::forward<UnitTestInfoPair>(infoPair) );
        }
    };
}
