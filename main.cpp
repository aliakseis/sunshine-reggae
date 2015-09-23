// MockObject.h
//#pragma once
#include <memory.h>
#include <assert.h>


template<typename M>
class CMockObject
{

#define DECL_TEN(hundreds, tens)    \
    FINAL_V_DECL(hundreds, tens, 0) \
    FINAL_V_DECL(hundreds, tens, 1) \
    FINAL_V_DECL(hundreds, tens, 2) \
    FINAL_V_DECL(hundreds, tens, 3) \
    FINAL_V_DECL(hundreds, tens, 4) \
    FINAL_V_DECL(hundreds, tens, 5) \
    FINAL_V_DECL(hundreds, tens, 6) \
    FINAL_V_DECL(hundreds, tens, 7) \
    FINAL_V_DECL(hundreds, tens, 8) \
    FINAL_V_DECL(hundreds, tens, 9)

#define DECL_HUNDRED(hundreds)\
    DECL_TEN(hundreds, 0) \
    DECL_TEN(hundreds, 1) \
    DECL_TEN(hundreds, 2) \
    DECL_TEN(hundreds, 3) \
    DECL_TEN(hundreds, 4) \
    DECL_TEN(hundreds, 5) \
    DECL_TEN(hundreds, 6) \
    DECL_TEN(hundreds, 7) \
    DECL_TEN(hundreds, 8) \
    DECL_TEN(hundreds, 9)

#define DECL_RETRIEVER \
    DECL_HUNDRED(0) \
    DECL_HUNDRED(1) \
    DECL_HUNDRED(2) \
    DECL_HUNDRED(3) \
    DECL_HUNDRED(4) \
    DECL_HUNDRED(5) \
    DECL_HUNDRED(6) \
    DECL_HUNDRED(7) \
    DECL_HUNDRED(8) \
    DECL_HUNDRED(9)

#define FINAL_V_DECL(hundreds, tens, units) \
    virtual int V##hundreds##tens##units() { return hundreds * 100 + tens * 10 + units; };

    class CThisCallVtIdxRetriever
    {
        DECL_RETRIEVER
    };

#if defined(_WIN32) && !defined(_WIN64)
    #undef FINAL_V_DECL
    #define FINAL_V_DECL(hundreds, tens, units) \
        virtual int __cdecl V##hundreds##tens##units() { return hundreds * 100 + tens * 10 + units; };

    class CCdeclVtIdxRetriever
    {
        DECL_RETRIEVER
    };

    #undef FINAL_V_DECL
    #define FINAL_V_DECL(hundreds, tens, units) \
        virtual int __stdcall V##hundreds##tens##units() { return hundreds * 100 + tens * 10 + units; };

    class CStdCallVtIdxRetriever
    {
        DECL_RETRIEVER
    };

    #undef FINAL_V_DECL
    #define FINAL_V_DECL(hundreds, tens, units) \
        virtual int __fastcall V##hundreds##tens##units() { return hundreds * 100 + tens * 10 + units; };

    class CFastCallVtIdxRetriever
    {
        DECL_RETRIEVER
    };
#endif

#undef DECL_RETRIEVER
#undef DECL_HUNDRED
#undef DECL_TEN
#undef FINAL_V_DECL

protected:
    void** const m_ppVT;

    CMockObject()
        : m_ppVT(new void*[1000])
    {
        memset(m_ppVT, 0, sizeof(void*) * 1000);
        ExecuteStructors(true);
    }
    ~CMockObject()
    {
        ExecuteStructors(false);
        delete[] m_ppVT;
    }

private:
    template <typename SRC_FUN, typename MOCK_FUN>
    void DoInstantiateThisCall(SRC_FUN pFun, MOCK_FUN pSubst)
    {
        int (CThisCallVtIdxRetriever::*pV)() = (int (CThisCallVtIdxRetriever::*)()) pFun;
        const int idx = (CThisCallVtIdxRetriever().*pV)();
        memcpy(m_ppVT + idx, &pSubst, sizeof(void*));
    }
 #if defined(_WIN32) && !defined(_WIN64)
    template <typename SRC_FUN, typename MOCK_FUN>
    void DoInstantiateCdecl(SRC_FUN pFun, MOCK_FUN pSubst)
    {
        int (__cdecl CCdeclVtIdxRetriever::*pV)() = (int (__cdecl CCdeclVtIdxRetriever::*)()) pFun;
        const int idx = (CCdeclVtIdxRetriever().*pV)();
        ((MOCK_FUN*) m_ppVT)[idx] = pSubst;
    }
    template <typename SRC_FUN, typename MOCK_FUN>
    void DoInstantiateStdCall(SRC_FUN pFun, MOCK_FUN pSubst)
    {
        int (__stdcall CStdCallVtIdxRetriever::*pV)() = (int (__stdcall CStdCallVtIdxRetriever::*)()) pFun;
        const int idx = (CStdCallVtIdxRetriever().*pV)();
        ((MOCK_FUN*) m_ppVT)[idx] = pSubst;
    }
    template <typename SRC_FUN, typename MOCK_FUN>
    void DoInstantiateFastCall(SRC_FUN pFun, MOCK_FUN pSubst)
    {
        int (__fastcall CFastCallVtIdxRetriever::*pV)() = (int (__fastcall CFastCallVtIdxRetriever::*)()) pFun;
        const int idx = (CFastCallVtIdxRetriever().*pV)();
        ((MOCK_FUN*) m_ppVT)[idx] = pSubst;
    }
#endif

protected:

#define DO_DECL_INSTANTIATE_WITH_PARAMS(convention, modifier, DoInstantiate, Instantiate, ...) \
    template<typename R, typename SRC, typename MOCK, typename... Args> \
    void Instantiate(R (convention SRC::*pFun)(Args... args __VA_ARGS__) modifier \
        , R (convention MOCK::*pSubst)(Args... args __VA_ARGS__) modifier) \
    { \
        DoInstantiate(pFun, pSubst); \
    }

#define DECL_EMPTY
#define DECL_VARARGS_X , ...

#define DECL_INSTANTIATE_WITH_PARAMS(args) DO_DECL_INSTANTIATE_WITH_PARAMS args

#define DECL_INSTANTIATE(...) \
    DECL_INSTANTIATE_WITH_PARAMS((__VA_ARGS__))

#define DECL_INSTANTIATE_CONVENTION(convention, CustomInstantiate) \
    DECL_INSTANTIATE(DECL_EMPTY convention, DECL_EMPTY,	CustomInstantiate, Instantiate, DECL_EMPTY) \
    DECL_INSTANTIATE(DECL_EMPTY convention, const,	CustomInstantiate, Instantiate, DECL_EMPTY)

DECL_INSTANTIATE_CONVENTION(DECL_EMPTY, DoInstantiateThisCall)    
#if defined(_WIN32) && !defined(_WIN64)
    DECL_INSTANTIATE_CONVENTION(__cdecl, DoInstantiateCdecl)
    DECL_INSTANTIATE_CONVENTION(__stdcall, DoInstantiateStdCall)
    DECL_INSTANTIATE_CONVENTION(__fastcall, DoInstantiateFastCall)
#endif

#if defined(_WIN32) && !defined(_WIN64)
    DECL_INSTANTIATE(__cdecl, DECL_EMPTY,   DoInstantiateCdecl, InstantiateVA, DECL_VARARGS_X)
    DECL_INSTANTIATE(__cdecl, const,        DoInstantiateCdecl, InstantiateVA, DECL_VARARGS_X)
#else
    DECL_INSTANTIATE(DECL_EMPTY, DECL_EMPTY,DoInstantiateThisCall, InstantiateVA, DECL_VARARGS_X)
    DECL_INSTANTIATE(DECL_EMPTY, const,     DoInstantiateThisCall, InstantiateVA, DECL_VARARGS_X)
#endif

#undef DO_DECL_INSTANTIATE_WITH_PARAMS
#undef DECL_INSTANTIATE_WITH_PARAMS
#undef DECL_INSTANTIATE_CONVENTION
#undef DECL_INSTANTIATE
#undef DECL_EMPTY
#undef DECL_VARARGS_X

    template<typename T, typename C> void ExecuteStructor(T C::*r, bool construct)
    {
        C* other = (C*) this;

        if (construct)
        {
            ::new(&((*other).*r)) T();
        }
        else
        {
            ((*other).*r).~T();
        }
    }
    
    void ExecuteStructors(bool construct)				
    {									
        int iMemberCountN = -1;						
        static_cast<M*>(this)->EnumerateStructors(iMemberCountN, construct);
        iMemberCountN = ~iMemberCountN;					
        if (construct)							
        {								
            int i = 0;							
            try								
            {								
                for (; i < iMemberCountN; ++i)				
                {							
                    ExecuteNthStructor(i, construct);			
                }							
            }								
            catch (...)							
            {								
                for (; i >= 0; --i)					
                {							
                    ExecuteNthStructor(i, false);			
                }							
                throw;							
            }								
        }								
        else								
            for (int i = iMemberCountN; --i >= 0; )			
            {								
                ExecuteNthStructor(i, construct);			
            }								
    }									
    void ExecuteNthStructor(int iMemberToCall, bool construct)		
    {									
        static_cast<M*>(this)->EnumerateStructors(iMemberToCall, construct);			
        assert(0 == iMemberToCall);					
    }									
    
}; // class CMockObject



#define DECLARE_MOCK_MEMBERS(src, mock)					        \
public:									                        \
    typedef src TMockOrigin;						            \
    char m_membersAllocBuffer[sizeof(src)];				        \
public:									                        \
    void EnumerateStructors(int& iMemberToCall, bool construct)


// It is not necessary to declare POD types
#define DECLARE_MOCK_MEMBER(member)					            \
        if (0 == iMemberToCall)						            \
        {								                        \
            ExecuteStructor(&TMockOrigin::member, construct);   \
            return;							                    \
        }								                        \
        --iMemberToCall;

//That's an example of its usage:
//#include "MockObject.h"
#include <iostream>
#include <sstream>

using std::string;
using std::ostringstream;
using std::cerr;

class A
{
public:
    virtual string Dummy0(int*);

    virtual string Dummy0(int) const;

    virtual void Dummy2(const char*);

    virtual int Dummy3();
    virtual int Dummy3Const() const;

    virtual string SomeOtherTest(int i, int& j);

    virtual int TakeThree(int i, int j, int k);

    virtual int TakeFour(int i, int j, int k, int l);

    virtual int TakeThreeConst(int i, int j, int k) const;

    virtual int TakeFourConst(int i, int j, int k, int l) const;

    virtual int TakeFive(int i, int j, int k, int l, int m);
    virtual int TakeFiveConst(int i, int j, int k, int l, int m) const;

    virtual double TakeFive2(int i, int j, int k, int l, int m);

    virtual double Test2ThisCall(int i, int& j);

#if defined(_WIN32)
    virtual void __stdcall StdCall(char*);
    virtual double __cdecl Test2Cdecl(int i, int& j);
    virtual double __stdcall Test2StdCall(int i, int& j);
    virtual double __fastcall Test2FastCall(int i, int& j);
#endif

    virtual double TestVarArgsThisCall(int i, ...);

    virtual double TestVarArgs0ThisCall(...);

    int dummy;

    string buffer2;

    int dummy2;

    string buffer;
};

class CXTest : public CMockObject<CXTest>
{
public:

    CXTest()
    {
        Instantiate(static_cast<string (A::*)(int) const>(&A::Dummy0), &CXTest::DoTest);
        Instantiate(static_cast<string (A::*)(int*)>(&A::Dummy0), &CXTest::DoAnotherTest);
        Instantiate(&A::Dummy2, &CXTest::YetAnotherTest);

        Instantiate(&A::Dummy3, &CXTest::NoParamsTest);
        Instantiate(&A::Dummy3Const, &CXTest::NoParamsTestConst);
        Instantiate(&A::TakeThree, &CXTest::TakeThree);
        Instantiate(&A::TakeFour, &CXTest::TakeFour);
        Instantiate(&A::TakeThreeConst, &CXTest::TakeThreeConst);
        Instantiate(&A::TakeFourConst, &CXTest::TakeFourConst);
        Instantiate(&A::TakeFive, &CXTest::TakeFive);
        Instantiate(&A::TakeFive2, &CXTest::TakeFive2);
        Instantiate(&A::TakeFiveConst, &CXTest::TakeFiveConst);

        Instantiate(&A::SomeOtherTest, &CXTest::SomeOtherTest);

        Instantiate(&A::Test2ThisCall, &CXTest::Test2ThisCall);
#if defined(_WIN32)
        Instantiate(&A::StdCall, &CXTest::StdCall);
        Instantiate(&A::Test2Cdecl, &CXTest::Test2Cdecl);
        Instantiate(&A::Test2StdCall, &CXTest::Test2StdCall);
        Instantiate(&A::Test2FastCall, &CXTest::Test2FastCall);
#endif

        InstantiateVA(&A::TestVarArgsThisCall, &CXTest::TestVarArgsThisCall);
        InstantiateVA(&A::TestVarArgs0ThisCall, &CXTest::TestVarArgs0ThisCall);
    }

    DECLARE_MOCK_MEMBERS(A, CXTest)
    {
        DECLARE_MOCK_MEMBER(buffer)
        DECLARE_MOCK_MEMBER(buffer2)
    }

    string DoTest(int i) const
    {
        ostringstream s;
        s << i;
        return s.str();
    }

    string DoAnotherTest(int* i)
    {
        ostringstream s;
        s << -*i;
        return s.str();
    }

    void YetAnotherTest(const char* p)
    {
        ((A*)this)->buffer = p;
    }

    int NoParamsTest()
    {
        return ((A*)this)->dummy;
    }
    int NoParamsTestConst() const
    {
        return ((A*)this)->dummy;
    }

    string SomeOtherTest(int i, int& j)
    {
        j = i;
        return ((A*)this)->buffer2;
    }

    int TakeThree(int i, int j, int k)
    {
        return i + j + k;
    }

    int TakeFour(int i, int j, int k, int l)
    {
        return i + j + k + l;
    }

    int TakeThreeConst(int i, int j, int k) const
    {
        return i + j + k;
    }

    int TakeFourConst(int i, int j, int k, int l) const
    {
        return i + j + k + l;
    }

    int TakeFive(int i, int j, int k, int l, int m)
    {
        return i + j + k + l + m;
    }

    double TakeFive2(int i, int j, int k, int l, int m)
    {
        return i + j + k + l + m;
    }

    int TakeFiveConst(int i, int j, int k, int l, int m) const
    {
        return i + j + k + l + m;
    }

    double Test2ThisCall(int i, int& j)
    {
        return 0.1;
    }

#if defined(_WIN32)
    void __stdcall StdCall(char* p)
    {
        ((A*)this)->buffer = p;
    }
    double __cdecl Test2Cdecl(int i, int& j)
    {
        return 0.2;
    }
    double __stdcall Test2StdCall(int i, int& j)
    {
        return 0.3;
    }
    double __fastcall Test2FastCall(int i, int& j)
    {
        return 0.4;
    }
#endif

    double TestVarArgsThisCall(int i, ...)
    {
        return 11.;
    }

    double TestVarArgs0ThisCall(...)
    {
        return 21.;
    }
};

#define CHECK_EQUAL(a, b) \
    if ((a) == (b)); else cerr << "CHECK_EQUAL failed at " __FILE__ "(" << __LINE__ << ")\n";

int main()
{
    CXTest test;

    A* pA = (A*) &test;

    CHECK_EQUAL("777", pA->Dummy0(777));

    int i = 999;
    CHECK_EQUAL("-999", pA->Dummy0(&i));

    pA->Dummy2("YetAnotherTest");
    CHECK_EQUAL("YetAnotherTest", pA->buffer);


    pA->dummy = 3456;
    CHECK_EQUAL(3456, pA->Dummy3());
    CHECK_EQUAL(3456, pA->Dummy3Const());

    int j = 0;
    pA->buffer2 = "This would be to miss an opportunity.";
    CHECK_EQUAL("This would be to miss an opportunity.", pA->SomeOtherTest(i, j));
    CHECK_EQUAL(i, j);

    CHECK_EQUAL(6, pA->TakeThree(1, 2, 3));
    CHECK_EQUAL(10, pA->TakeFour(1, 2, 3, 4));

    CHECK_EQUAL(6, pA->TakeThreeConst(1, 2, 3));
    CHECK_EQUAL(10, pA->TakeFourConst(1, 2, 3, 4));

    CHECK_EQUAL(15, pA->TakeFive(1, 2, 3, 4, 5));
    CHECK_EQUAL(15, pA->TakeFiveConst(1, 2, 3, 4, 5));

    CHECK_EQUAL(0.1, pA->Test2ThisCall(1, j));
#if defined(_WIN32)
    pA->StdCall("StdCallTest");
    CHECK_EQUAL("StdCallTest", pA->buffer);
    CHECK_EQUAL(0.2, pA->Test2Cdecl(1, j));
    CHECK_EQUAL(0.3, pA->Test2StdCall(1, j));
    CHECK_EQUAL(0.4, pA->Test2FastCall(1, j));
#endif

    CHECK_EQUAL(11., pA->TestVarArgsThisCall(1, j));
    CHECK_EQUAL(21., pA->TestVarArgs0ThisCall(1, j));

    std::cout << "ok.\n";
}
