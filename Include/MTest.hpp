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
#pragma once
#include <string>
#include <iostream>
#include <functional>
#include <vector>
#include <algorithm>
#include <ostream>

// _WIN64   - 64 bit
// _WIN32   - 32 i 64 bit
// _M_X64   - x64 Visual Studio
// __CYGWIN__   - Windows with Cygwin (POSIX)
//#if defined(__WIN32__) || defined(WIN32) || defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
// __linux__     - Linux kernel
// __x86_64__    - x64 GNUC
//#elif defined(__LINUX__) || defined(linux) || defined(__linux) || defined(__linux__)

#if defined(_WIN64) || defined(_WIN32)
    #define WINDOWS_PLATFORM 1
#elif defined(__linux__) || defined(linux)
    #define LINUX_PLATFORM 1
#else
    #error "Unsupported platform"
#endif

#ifdef WINDOWS_PLATFORM
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif // WINDOWS_PLATFORM

// Macros for output
#ifndef LOG
#define LOG std::cout
#endif // LOG

#ifndef NEW_LINE
#define NEW_LINE std::endl
#endif // NEW_LINE

#define INFO(Msg) LOG << MTest::EConsoleColor::Default << "[Info]: " << Msg << NEW_LINE

/// Stops Unit test Execution when fails. Must evaluate to true expression.
#define ASSERT_TRUE(Cond) \
do \
{ \
    if( !(Cond) ) \
    { \
        throw MTest::CAssert( #Cond , __FILE__ , __LINE__ ); \
    } \
} \
while(false)

/// Stops Unit test Execution when fails. Must evaluate to false expression.
#define ASSERT_FALSE(Cond) \
do \
{ \
    if( (Cond) ) \
    { \
        throw MTest::CAssert( #Cond , __FILE__ , __LINE__ ); \
    } \
} \
while(false)

/// Execution continues when fails. Must evaluate to true expression.
#define CHECK_TRUE(Cond) \
do \
{ \
    if( !(Cond) ) \
    { \
        TEST_REF_UT.OnCheckFail(); \
        LOG << MTest::EConsoleColor::Yellow << MTest::CCheck( #Cond , __FILE__ , __LINE__ ) << MTest::EConsoleColor::Default << NEW_LINE; \
    } \
} \
while(false)

/// Execution continues when fails. Must evaluate to false expression.
#define CHECK_FALSE(Cond) \
do \
{ \
    if( (Cond) ) \
    { \
        TEST_REF_UT.OnCheckFail(); \
        LOG << MTest::EConsoleColor::Yellow << MTest::CCheck( #Cond , __FILE__ , __LINE__ )<< MTest::EConsoleColor::Default << NEW_LINE; \
    } \
} \
while(false)

#define UNIT_TEST(Name) \
void Name##Test(MTest::CTestManager::CUnitTest& TEST_REF_UT); \
namespace MTest \
{ \
    STestProxy ut##Name##Inst( #Name , __FILE__ , __LINE__ , Name##Test ); \
} \
void Name##Test(MTest::CTestManager::CUnitTest& TEST_REF_UT)

#define UNIT_TEST2(Name, Sec) \
void Name##Sec##Test(MTest::CTestManager::CUnitTest& TEST_REF_UT); \
namespace MTest \
{ \
    STestProxy ut##Name##Sec##Inst( #Name ": " #Sec ,  __FILE__ , __LINE__ , Name##Sec##Test ); \
} \
void Name##Sec##Test(MTest::CTestManager::CUnitTest& TEST_REF_UT)

/// Use to Implement main()
#define MTEST_MAIN \
int main(int, char*[]) \
{ \
    MTest::CTestManager::Instance().Run(); \
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

        int ToWin32Color(const EConsoleColor x)
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

        std::string ToAnsiColor(const EConsoleColor x)
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
    }

    std::ostream& operator<<(std::ostream& Stream, const EConsoleColor x)
    {
        #ifdef WINDOWS_PLATFORM
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), Detail::ToWin32Color(x));
        #else
        Stream << Detail::ToAnsiColor(x);
        #endif // WINDOWS_PLATFORM
        return Stream;
    }

    class ICheck
    {
    public:
        ICheck(const std::string& aCondition, const std::string& aFile, const int aLine):
            Condition( aCondition ),
            File( aFile ),
            Line( aLine )
        {
        }

        virtual ~ICheck() = default;

        virtual std::string ToString() const = 0;
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
        CCheck(const std::string& aCondition, const std::string& aFile, const int aLine):
            ICheck( aCondition, aFile, aLine )
        {
        }

        std::string ToString() const override
        {
            return "[Check]: Failed: "+Condition+" File: "+Detail::GetFileName(File)+" Line: "+std::to_string(Line);
        }
    };

    class CAssert: public ICheck
    {
    public:
        CAssert(const std::string& aCondition, const std::string& aFile, const int aLine):
            ICheck( aCondition, aFile, aLine )
        {
        }

        std::string ToString() const override
        {
            return "[Assert]: Failed: "+Condition+" File: "+Detail::GetFileName(File)+" Line: "+std::to_string(Line);
        }
    };

    class CTestManager final
    {
    public:
        class CUnitTest;
        using UnitTestCallback = std::function<void(CUnitTest&)>;
        //
        class CUnitTest
        {
        public:
            CUnitTest(const std::string& aName, const std::string& aFile, const int aLine, const UnitTestCallback& aFunc):
                Name( aName ),
                File( aFile ),
                Line( aLine ),
                Func( aFunc )
            {
            }

            bool IsFailed() const
            {
                return Failed;
            }

            void Run()
            {
                LOG << NEW_LINE;
                LOG << EConsoleColor::Blue << "[Test]: " << Name << EConsoleColor::Default << NEW_LINE;
                LOG << EConsoleColor::Blue << "[Test]: File: " << Detail::GetFileName(File) << " Line: " << Line << EConsoleColor::Default << NEW_LINE;
                try
                {
                    Func(*this);
                }
                catch(const CAssert& As)
                {
                    Fail();
                    LOG << EConsoleColor::Red << As << EConsoleColor::Default << NEW_LINE;
                }
                catch(const std::exception& Exception)
                {
                    Fail();
                    LOG << EConsoleColor::Red << "[Exception]: " << Exception.what() << EConsoleColor::Default << NEW_LINE;
                }
                catch(...)
                {
                    Fail();
                    LOG << EConsoleColor::Red << "[Exception]: Unknown" << EConsoleColor::Default << NEW_LINE;
                }
                if( !IsFailed() )
                {
                    LOG << EConsoleColor::Green << "[Test]: " << Name << " - Success" << EConsoleColor::Default << NEW_LINE;
                }
                else
                {
                    if( !ChecksFailed )
                    {
                        LOG << EConsoleColor::Red << "[Test]: " << Name << " - Fail" << EConsoleColor::Default << NEW_LINE;
                    }
                    else
                    {
                        LOG << EConsoleColor::Red << "[Test]: " << Name << " - Fail - Failed Checks - " << ChecksFailed << EConsoleColor::Default << NEW_LINE;
                    }
                }
            }

            const std::string& GetName() const
            {
                return Name;
            }

            const std::string& GetFile() const
            {
                return File;
            }

            int GetLine() const
            {
                return Line;
            }

            void OnCheckFail()
            {
                if( !Failed )
                {
                    Fail();
                }
                ++ChecksFailed;
            }

            CUnitTest(const CUnitTest&) = delete;
            CUnitTest(CUnitTest&&) = default;
            CUnitTest& operator=(const CUnitTest&) = delete;
            CUnitTest& operator=(CUnitTest&&) = default;
        private:
            void Fail()
            {
                Failed = true;
            }
        private:
            std::string Name;
            std::string File;
            int Line;
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

        void Add(const std::string& Name, const std::string& File, const int Line, const UnitTestCallback& Func)
        {
            auto It = std::find_if(UnitTests.begin(), UnitTests.end(), [&](const CUnitTest& Other) { return Other.GetName() == Name; });
            if( It != UnitTests.end() )
            {
                LOG << EConsoleColor::Red << "[Error]: Test: " << Name << " Already Exists" << EConsoleColor::Default << NEW_LINE;
                return;
            }
            UnitTests.emplace_back( Name, File, Line, Func );
        }

        void Run()
        {
            LOG << EConsoleColor::Blue << "[Tests]: Count: " << UnitTests.size() << EConsoleColor::Default << NEW_LINE;
            for(auto& i: UnitTests)
            {
                i.Run();
                if( i.IsFailed() )
                {
                    FailedUnitTest.push_back( i.GetName()+"; File: "+Detail::GetFileName(i.GetFile())+" Line: "+std::to_string(i.GetLine()) );
                }
            }
            if( FailedUnitTest.size() )
            {
                LOG << NEW_LINE;
                LOG << EConsoleColor::Red << "[Tests]: Failed Unit Tests: " << FailedUnitTest.size() << EConsoleColor::Default << NEW_LINE;
                for(const auto& i: FailedUnitTest)
                {
                    LOG << EConsoleColor::Red << "[Test]: " << i << EConsoleColor::Default << NEW_LINE;
                }
            }
        }

        CTestManager(const CTestManager&) = delete;
        CTestManager(CTestManager&&) = delete;
        CTestManager& operator=(const CTestManager&) = delete;
        CTestManager& operator=(CTestManager&&) = delete;
    private:
        CTestManager()
        {
            #ifdef WINDOWS_PLATFORM
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
            #endif // WINDOWS_PLATFORM
        }

        ~CTestManager()
        {
            #ifdef WINDOWS_PLATFORM
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), csbi.wAttributes);
            #endif // WINDOWS_PLATFORM
        }
    private:
        std::vector<CUnitTest> UnitTests;
        std::vector<std::string> FailedUnitTest;
        #ifdef WINDOWS_PLATFORM
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        #endif // WINDOWS_PLATFORM
    };

    struct STestProxy
    {
        STestProxy(const std::string& Name, const std::string& File, const int Line, const CTestManager::UnitTestCallback& Func)
        {
            CTestManager::Instance().Add( Name, File, Line, Func );
        }
    };
}