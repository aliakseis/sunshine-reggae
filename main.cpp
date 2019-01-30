//That's an example of its usage:
#include "MockObject.h"

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

    A* volatile pA = (A*) &test; // volatile to prevent superfluous optimization 

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
