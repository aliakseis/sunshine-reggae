#pragma once

// github.com/aliakseis/sunshine-reggae

#include <memory.h>
#include <assert.h>

class CMockObjectBase
{
protected:
    void** const m_ppVT;

    CMockObjectBase()
        : m_ppVT(new void*[1000])
    {
        memset(m_ppVT, 0, sizeof(void*) * 1000);
    }
    ~CMockObjectBase()
    {
        delete[] m_ppVT;
    }
};

template<typename M>
class CMockObject : public CMockObjectBase
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
    CMockObject()
    {
        ExecuteStructors(true);
    }
    ~CMockObject()
    {
        ExecuteStructors(false);
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



#define DECLARE_MOCK_MEMBERS(src, mock)                         \
public:                                                         \
    typedef src TMockOrigin;                                    \
    char m_membersAllocBuffer[sizeof(src)];                     \
public:                                                         \
    void EnumerateStructors(int& iMemberToCall, bool construct)


// It is not necessary to declare POD types
#define DECLARE_MOCK_MEMBER(member)                             \
        if (0 == iMemberToCall)                                 \
        {                                                       \
            ExecuteStructor(&TMockOrigin::member, construct);   \
            return;                                             \
        }                                                       \
        --iMemberToCall;
