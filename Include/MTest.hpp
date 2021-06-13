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

// Macros for output
#ifndef LOG
#define LOG std::cout
#endif // LOG

#ifndef NEW_LINE
#define NEW_LINE std::endl
#endif // NEW_LINE

#define INFO(Msg) LOG << "[Info]: " << Msg << NEW_LINE

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
        LOG << MTest::CCheck( #Cond , __FILE__ , __LINE__ ) << NEW_LINE; \
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
        LOG << MTest::CCheck( #Cond , __FILE__ , __LINE__ ) << NEW_LINE; \
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
int main(int argc, char* argv[]) \
{ \
    MTest::CTestManager::Instance().Run(); \
	return 0; \
}

namespace MTest
{
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

    class CTestManager
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
                LOG << "[Test]: " << Name << NEW_LINE;
                LOG << "[Test]: File: " << Detail::GetFileName(File) << " Line: " << Line << NEW_LINE;
                try
                {
                    Func(*this);
                }
                catch(const CAssert& As)
                {
                    Fail();
                    LOG << As << NEW_LINE;
                }
                catch(const std::exception& Exception)
                {
                    Fail();
                    LOG << "[Exception]: " << Exception.what() << NEW_LINE;
                }
                catch(...)
                {
                    Fail();
                    LOG << "[Exception]: " << "Unknown" << NEW_LINE;
                }
                if( !IsFailed() ) 
                {
                    LOG << "[Test]: " << Name << " - Success" << NEW_LINE;
                }
                else
                {
                    if( !ChecksFailed ) 
                    {
                        LOG << "[Test]: " << Name << " - Fail" << NEW_LINE;
                    }
                    else 
                    {
                        LOG << "[Test]: " << Name << " - Fail - Failed Checks - " << ChecksFailed << NEW_LINE;
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
                LOG << "[Error]: Test: " << Name << " Already Exists" << NEW_LINE;
                return;
            }
            UnitTests.emplace_back( Name, File, Line, Func );
        }

        void Run()
        {
            LOG << "[Tests]: Count: " << UnitTests.size() << NEW_LINE;
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
                LOG << "[Tests]: Failed Unit Tests: " << FailedUnitTest.size() << NEW_LINE;
                for(const auto& i: FailedUnitTest)
                {
                    LOG << "[Test]: " << i << NEW_LINE;
                }
            }
        }

        CTestManager(const CTestManager&) = delete;
        CTestManager(CTestManager&&) = delete;
        CTestManager& operator=(const CTestManager&) = delete;
        CTestManager& operator=(CTestManager&&) = delete;
    private:
        CTestManager() = default;
    private:
        std::vector<CUnitTest> UnitTests;
        std::vector<std::string> FailedUnitTest;
    };

    struct STestProxy
    {
        STestProxy(const std::string& Name, const std::string& File, const int Line, const CTestManager::UnitTestCallback& Func)
        {
            CTestManager::Instance().Add( Name, File, Line, Func );
        }
    };
}
