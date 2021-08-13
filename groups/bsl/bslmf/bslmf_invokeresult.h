// bslmf_invokeresult.h                                               -*-C++-*-
#ifndef INCLUDED_BSLMF_INVOKERESULT
#define INCLUDED_BSLMF_INVOKERESULT

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

//@PURPOSE: Determine the result type of an invocable expression.
//
//@CLASSES:
// bsl::invoke_result: Metafunction to determine invocation result type
// bslmf::InvokeResultDeductionFailed: Returned on failed result deduction
//
//@MACROS:
// BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS: defined if SFINAE-friendly
//
//@SEE_ALSO: bslstl_invoke
//
//@DESCRIPTION: This component provides a metafunction 'bsl::invoke_result'
// that determines, at compile time, the type returned by invoking a callable
// type, including pointer-to-function, pointer-to-member function,
// pointer-to-member object (returns the object type), or functor class, and a
// class 'bslmf::InvokeResultDeductionFailed' that is returned when the
// invocation return type cannot be determined (C++03 only).  For a set of
// types 'F', 'T1', 'T2', and 'T3', 'bsl::invoke_result<F, T1, t2, T3>::type'
// is roughly the type of the return value obtained by calling an object of
// type 'F' with arguments of type 'T1', 'T2', and 'T3', respectively.
// However, 'invoke_result' goes beyond function-like objects and deduces a
// return type if 'F' is a pointer to function member or data member of some
// class 'C' and 'T1' is a type (derived from) 'C', pointer to 'C', or
// smart-pointer to 'C'.  (See precise specification, below).
//
// The interface and functionality of 'bsl::invoke_result' is intended to be
// identical to that of the C++17 metafunction, 'std::invoke_result' except
// that invalid argument lists are detected in C++11 and later, but not in
// C++03.  In C++03, invalid arguments lists will result in a compilation error
// (instead of simply missing 'type') in the remaining cases.  Some other
// functionality is lost when compiling with a C++03 compiler -- see the
// precise specification, below.
//
///C++17 Semantics Detection
///-------------------------
// This component defines the macro
// 'BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS' if 'bsl::invoke_result' behaves
// according to the C++17 specification of 'std::invoke_result', which is
// elaborated below.  This macro is defined as long as the compiler supports
// the 'decltype' specifier, which is generally available in C++11 and later
// compilation modes.
//
///Precise specification
///---------------------
// The C++11 and C++14 standard defines the pseudo-expression
// _INVOKE_ '(f, t1, t2, ..., tN)', as follows:
//
//: o '(t1.*f)(t2, ..., tN)' when 'f' is a pointer to a member function of a
//:   class 'T' and 't1' is an object of type 'T' or a reference to an object
//:   of type 'T' or a reference to an object of a type derived from 'T';
//: o '((*t1).*f)(t2, ..., tN)' when 'f' is a pointer to a member function of
//:   a class 'T' and 't1' is not one of the types described in the previous
//:   item;
//: o 't1.*f' when 'N == 1' and 'f' is a pointer to member data of a class 'T'
//:   and 't1' is an object of type 'T' or a reference to an object of type
//:   'T' or a reference to an object of a type derived from 'T';
//: o '(*t1).*f' when 'N == 1' and 'f' is a pointer to member data of a class
//:   'T' and 't1' is not one of the types described in the previous item;
//: o 'f(t1, t2, ..., tN)' in all other cases.
//
// Given types 'F', 'T1', 'T2', ..., 'TN' corresponding to the expressions
// 'f', 't1', 't2', ..., 'tN' in the definition of _INVOKE_, the type produced
// by 'bslmf::ResultType<F, T1, T2, ..., TN>::type' is generally the type of
// the psuedo-expression _INVOKE_ '(f, t1, t2, ..., tN)', with some
// limitations in C++03, as described below.
//
// Because C++03 does not support 'decltype', there are circumstances in which
// 'bsl::invoke_result' is not able to deduce the return type of an invocable
// object of class type (i.e., a functor).  If 'R' is the type of the _INVOKE_
// expression, then ideally 'type' is 'R'.  However the C++03 version of
// 'bsl::invoke_result' determines 'type' as follows:
//
//: 1 If there exists a user-defined specialization of
//:   'bsl::invoke_result<F, T1, T2, ... TN>', then 'type' is determined by the
//:   specialization, regardless of correctness. (This rule is true of C++11
//:   and later, as well.)
//: 2 Otherwise, if 'F' is a function type, pointer to function type, pointer
//:   to member function type, pointer to member object type, or reference to
//:   any of these (i.e, 'F' is anything other than a class type or reference
//:   to class type), then 'type' is 'R'.
//: 3 Otherwise, if 'R' is
//:    o a fundamental type,
//:    o a pointer to (possibly cv-qualified) 'void' or fundamental type,
//:    o an lvalue reference to any of the above types (possibly cv-qualified),
//:    o 'bsl::nullptr_t', or
//:    o 'void',
//:   then 'type' is 'R'.
//: 4 Otherwise, if 'F' is a class type with member 'result_type', then 'type'
//:   is 'F::result_type'.  Note that 'bsl::invoke_result' cannot deduce
//:   different result types for different overloads of 'operator()' in this
//:   case.
//: 5 Otherwise, if 'F' is a class type with member type 'ResultType', then
//:   'type' is 'F::ResultType'.  Note that 'bsl::invoke_result' cannot deduce
//:   different result types for different overloads of 'operator()' in this
//:   case.
//: 6 Otherwise, 'type' is 'bslmf::InvokeResultDeductionFailed'.  The benefit
//:   of this placeholder over a compilation error is that 'invoke_result' is
//:   often used in a context where the return value will eventually be
//:   discarded.  Thus, generating a useless type is often harmless.  In cases
//:   where it is not harmless, the placeholder type will almost certainly
//:   result in a compilation error in the surrounding code.
//
// If the callable type is a pointer-to-member (data or function), invalid
// argument lists are not detected.  Thus, there is a small chance that invalid
// code will compile successfully, though it is hard to see now this would be
// harmful, since determining the return type of an expression is not very
// useful if the expression is not eventually evaluated, which will certainly
// produce the expected compilation error for invalid argument lists.
//
///Usage Example
///-------------
// Suppose we want to create a wrapper that executes an invocable object and
// sets a 'done' flag.  The 'done' flag will not be set if the invocation
// exits via an exception.  The wrapper takes an invocable 'f' and an argument
// 'x' and evaluates 'f(x)', returning the result.  In the absence of C++14
// automatically-deduced function return declarations, we use
// 'bsl::invoke_result' to deduce the return type of 'f(x)'.
//
// First, we write the wrapper template as follows:
//..
//  template <class FT, class XT>
//  typename bsl::invoke_result<FT, XT>::type
//  invokeAndSetFlag(bool *done, FT f, XT x)
//      // Return 'f(x)' and set '*done' to true if no exception.
//  {
//      typedef typename bsl::invoke_result<FT, XT>::type ResultType;
//      *done = false; // Clear flag in case of exception
//      ResultType result = f(x);
//      *done = true;  // Set flag on success
//      return result;
//  }
//..
// Note that additional metaprogramming would be required to make this
// template work for return type 'void'; such metaprogramming is beyond the
// scope of this usage example.
//
// Then we define a couple of simple functors to be used with the wrapper.
// The first functor is a simple template that triples its invocation
// argument:
//..
//  template <class TP>
//  struct Triple {
//      // Functor that triples its argument.
//
//      TP operator()(TP v) const { return static_cast<TP>(v * 3); }
//          // Return three times the specified 'v' value.
//  };
//..
// Next, we define a second functor that returns an enumerator 'ODD' or
// 'EVEN', depending on whether its argument is exactly divisible by 2.  Since
// the return type is not a fundamental type, this functor indicates its
// return type using the 'ResultType' idiom:
//..
//  enum EvenOdd { e_EVEN, e_ODD };
//
//  struct CalcEvenOdd {
//      // Functor that determines whether its argument is odd or even.
//
//      typedef EvenOdd ResultType;
//
//      EvenOdd operator()(int i) const { return (i & 1) ? e_ODD : e_EVEN; }
//          // Return 'e_ODD' if the specified 'i' is odd; otherwise return
//          // 'e_EVEN'
//  };
//..
// Finally, we can invoke these functors through our wrapper:
//..
//  int main()
//      // Run the usage example.
//  {
//      bool done = false;
//
//      Triple<short> ts = {};
//      short         r0 = invokeAndSetFlag(&done, ts, short(9));
//      assert(done && 27 == r0);
//
//      CalcEvenOdd ceo = {};
//      done            = false;
//      EvenOdd r1      = invokeAndSetFlag(&done, ceo, 5);
//      assert(done && e_ODD == r1);
//
//      done = false;
//      EvenOdd r2 = invokeAndSetFlag(&done, ceo, 8);
//      assert(done && e_EVEN == r2);
//
//      return 0;
//  }
//..

#include <bslscm_version.h>

#include <bslmf_addconst.h>
#include <bslmf_addlvaluereference.h>
#include <bslmf_addpointer.h>
#include <bslmf_addrvaluereference.h>
#include <bslmf_addvolatile.h>
#include <bslmf_assert.h>
#include <bslmf_decay.h>
#include <bslmf_enableif.h>
#include <bslmf_functionpointertraits.h>
#include <bslmf_isclass.h>
#include <bslmf_isconvertible.h>
#include <bslmf_islvaluereference.h>
#include <bslmf_ismemberobjectpointer.h>
#include <bslmf_isreference.h>
#include <bslmf_isreferencewrapper.h>
#include <bslmf_isrvaluereference.h>
#include <bslmf_isvoid.h>
#include <bslmf_memberfunctionpointertraits.h>
#include <bslmf_movableref.h>
#include <bslmf_removecv.h>
#include <bslmf_resulttype.h>
#include <bslmf_tag.h>
#include <bslmf_voidtype.h>

#include <bsls_compilerfeatures.h>
#include <bsls_nullptr.h>
#include <bsls_platform.h>

#if   defined(BSLS_COMPILERFEATURES_SUPPORT_DECLTYPE) \
 && !(defined(BSLS_PLATFORM_CMP_MSVC) && BSLS_PLATFORM_CMP_VERSION == 1800)
// The implementation of C++17 semantics in this component depends upon the
// use of 'decltype' for expression SFINAE.  MSVC 2013 (which has version
// number 1800) claims to support 'decltype', but does not have a sufficiently
// functional implementation of expression sfinae to enable C++17 semantics.
#define BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS 1
#endif

#if BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES
// Include version that can be compiled with C++03
// Generated on Thu Feb 18 17:39:32 2021
// Command line: sim_cpp11_features.pl bslmf_invokeresult.h
# define COMPILING_BSLMF_INVOKERESULT_H
# include <bslmf_invokeresult_cpp03.h>
# undef COMPILING_BSLMF_INVOKERESULT_H
#else

namespace BloombergLP {
namespace bslmf {

                 // ==========================================
                 // class template InvokeResultDeductionFailed
                 // ==========================================

struct InvokeResultDeductionFailed {
    // When 'invoke_result' cannot deduce the actual return type of a functor
    // (in C++03 mode), it yields this type as a placeholder.  The advantage of
    // using this placeholder instead of a compilation failure (e.g., using a
    // static assert) is that the return type of an INVOKE() operation is
    // often discarded, so our failure to deduce the return type is often
    // harmless.  Since 'InvokeResultDeductionFailed' is a return type, it must
    // be convertible from the actual return type; this conversion is
    // accomplished by means of a constructor that makes it convertible from
    // *any* type.

    // CREATORS
    template <class TYPE>
    InvokeResultDeductionFailed(const TYPE&) { }
        // Convert from an arbitrary type.  The actual argument value is
        // discarded.
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

template <class FN, class... ARGTYPES>
struct InvokeResult_BaseCalcUtil;
    // Forward declaration

#endif

}  // close package namespace
}  // close enterprise namespace

                        // ============================
                        // class template invoke_result
                        // ============================

namespace bsl {

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

template <class FN, class... ARGTYPES>
class invoke_result
: public BloombergLP::bslmf::InvokeResult_BaseCalcUtil<FN,
                                                       ARGTYPES...>::BaseType {
    // This class is a metafunction that conditionally provides a 'type' member
    // that is the type resulting from invoking an object of the specified 'FN'
    // template parameter with arguments of the specified 'ARGTYPES' template
    // parameters.  More precisely, given types 'F', 'T1', 'T2', ..., 'TN'
    // corresponding to expressions 'f', 't1', 't2', ..., 'tN',
    // 'bslmf::ResultType<F, T1, T2, ..., TN>::type' is usually the type of the
    // psuedo-expression _INVOKE_ '(f, t1, t2, ..., tN)', as defined in section
    // [func.rquire] of the C++11 standard.  If the compiler supports C++11
    // 'decltype' and the psuedo-expression _INVOKE_ '(f, t1, t2, ..., tN)' is
    // not well-formed, this class provides no 'type' member.  If 'FN' is a
    // class (functor) type and the compiler doesn't support C++11 'decltype',
    // the return type is automatically deduced for fundamental types, 'void',
    // pointers or references to those, or 'bsl::nullptr_t' and is deduced by
    // 'bslmf::ResultType<FN>::type' otherwise.  If deduction fails, this
    // metafunction yields 'bslmf::InvokeResultDeductionFailed'.  See
    // component-level documentation for more detail.

    ///Implementation Note
    ///- - - - - - - - - -
    // If, by the rules outlined above in the class documentation, this type
    // defines a member 'type' typedef, that typedef comes from the
    // 'bslmf::InvokeResult_BaseCalcUtil' specialization from which this class
    // inherits.
};

#endif

}  // close namespace bsl

// ============================================================================
//                              TEMPLATE IMPLEMENTATIONS
// ============================================================================

namespace BloombergLP {
namespace bslmf {

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

template <bool  IS_FUNCPTR,
          bool  IS_MEMFUNCPTR,
          bool  IS_MEMOBJPTR,
          class FN,
          class... ARGTYPES>
struct InvokeResult_Imp;
    // Forward declaration

                 // =========================================
                 // struct template InvokeResult_BaseCalcUtil
                 // =========================================

template <class FN, class... ARGTYPES>
struct InvokeResult_BaseCalcUtil {
    // This component-private utility 'struct' template provides a nested
    // typedef, 'BaseType', which is a class type that itself provides a nested
    // typedef 'type' that is the type of the 'INVOKE(fn, args...)' expression
    // given 'fn' is an object of the specified 'FN' type and 'args...' are
    // objects of the specified 'ARGTYPES...' types.  If the
    // 'INVOKE(fn, args...)' expression is not well-formed, 'BaseType' provides
    // no such nested typedef.

  private:
    // PRIVATE TYPES
    typedef typename bslmf::MovableRefUtil::Decay<FN>::type F;
        // Remove references and cv-qualifiers from 'FN', and decay function
        // types and array types to pointers.  In C++03, treat
        // 'bslmf::MovableRef<T>' as a (movable) reference-qualified 'T'.

    enum {
        k_IS_FUNCPTR   = BloombergLP::bslmf::IsFunctionPointer<F>::value,
        k_IS_MEMFUNCPTR= BloombergLP::bslmf::IsMemberFunctionPointer<F>::value,
        k_IS_MEMOBJPTR = bsl::is_member_object_pointer<F>::value
    };

    typedef typename
    bsl::conditional<k_IS_FUNCPTR || k_IS_MEMFUNCPTR || k_IS_MEMOBJPTR,
                     F, FN>::type FwdFn;
        // 'FwdFn' is the type forwarded to 'InvokeResult_Imp'.  It is 'F'
        // (a.k.a., 'decay_t<FN>') if 'F' is a function pointer or
        // pointer-to-member and 'FN' otherwise.

  public:
    // TYPES
    typedef typename BloombergLP::bslmf::InvokeResult_Imp<k_IS_FUNCPTR,
                                                          k_IS_MEMFUNCPTR,
                                                          k_IS_MEMOBJPTR,
                                                          FwdFn,
                                                          ARGTYPES...>
        BaseType;
        // In C++11 and later, conditionally provides a nested typedef 'type'
        // that is the type returned by the expression _INVOKE_ '(f, args...)',
        // where 'f' is an object of type 'FN' and 'args...' is a list of
        // object of types 'ARGTYPES...', if the expression is well formed.  In
        // C++03, provide a nested typed 'type' that is the type returned by
        // the same invoke expression if the type can be deduced, and is
        // 'InvokeResultDeductionFailed' otherwise.  Note that in C++11 and
        // later, 'type' is never 'InvokeResultDeductionFailed'.
};

#endif

                        // ============================
                        // struct InvokeResult_IsBaseOf
                        // ============================

template <class BASE, class DERIVED>
struct InvokeResult_IsBaseOf
: bsl::integral_constant<
    bool,
    bsl::is_class<typename bsl::remove_cv<BASE>::type>::value
 && bsl::is_class<typename bsl::remove_cv<DERIVED>::type>::value
 && bsl::is_convertible<typename bsl::remove_cv<DERIVED>::type *,
                        typename bsl::remove_cv<BASE>::type *>::value> {
    // This component-private 'struct' template provides an implementation
    // of the 'std::is_base_of' type trait that may be used when a C++11
    // baseline library is not necessarily available.
};

               // =============================================
               // struct InvokeResult_MemberObjectPointerTraits
               // =============================================

template <class TYPE>
struct InvokeResult_MemberObjectPointerTraits;
    // This component-private utility 'struct' template provides the following
    // nested typedefs:
    //: 'ClassType': The type of the class for which the specified 'TYPE' is a
    //:              pointer to member object.
    //: 'MemberType': The type of the member object of the class for which the
    //:               specified 'TYPE' is a pointer to member object.
    // Instantiation will fail unless 'TYPE' is a pointer-to-member-object
    // type.  This primary (unspecialized) template is not defined.

template <class MEMBER_TYPE, class CLASS_TYPE>
struct InvokeResult_MemberObjectPointerTraits<MEMBER_TYPE CLASS_TYPE::*> {
    // TYPES
    typedef CLASS_TYPE ClassType;
        // 'ClassType' is an alias to the type of the class for which the
        // specified 'TYPE' is a pointer to member object.

    typedef MEMBER_TYPE MemberType;
        // 'MemberType' is an alias to the type of the member object of the
        // class for which the specified 'TYPE' is a pointer to member object.
};

                      // ===============================
                      // struct InvokeResult_VoidChecker
                      // ===============================

struct InvokeResult_VoidChecker : Tag<true> {
    // Empty type used to detect void expressions.  The size of this type is
    // the same as 'bslmf::Tag<1>'.
};

template <class TYPE>
typename bsl::enable_if<!bsl::is_void<TYPE>::value, Tag<false> >::type
operator,(const TYPE&, InvokeResult_VoidChecker);
    // Return 'InvokeResult_VoidChecker()' if the left argument is of type
    // cv-'void'; otherwise 'bslmf::Tag<false>()'.  This overload of the comma
    // operator is declared but not defined, and is intended to be used in
    // metafunctions in an unevaluated context to detect void expressions.
    // For any non-void expression 'expr', '(expr,InvokeResult_VoidChecker())',
    // will match this overload and produce a result of type
    // 'bslmf::Tag<false>'.  However, 'const TYPE&' will not match 'void', so
    // if 'expr' is a void expression, the built-in comma operator is matched
    // and the result will have type 'InvokeResult_VoidChecker' (i.e., the
    // second argument).
    //
    // Note that Sun CC incorrectly matches this overload for a void
    // expression, then fails hard.  The 'enable_if' prevents this match for
    // Sun CC and any other compilers that may similarly match 'void' and is
    // harmless for compilers that don't.

struct InvokeResult_Index {
    // Metafunction helpers for deducing the return type of an expression.

    enum {
        // Enumeration of possible return types.

        e_VOID,
        e_BOOL,
        e_CHAR,
        e_SCHAR,
        e_UCHAR,
        e_WCHAR_T,
        e_CHAR16_T,
        e_CHAR32_T,
        e_SHORT,
        e_USHORT,
        e_INT,
        e_UNSIGNED,
        e_LONG,
        e_ULONG,
        e_LONG_LONG,
        e_ULONG_LONG,
        e_FLOAT,
        e_DOUBLE,
        e_LONG_DOUBLE,

        // Pointer to void is special among pointers because it cannot be
        // dereferenced.
        e_VOIDPTR,
        e_CONST_VOIDPTR,
        e_VOLATILE_VOIDPTR,
        e_CONST_VOLATILE_VOIDPTR,

        e_NULLPTR_T,
        e_POINTER, // Any pointer type other than 'void *' or 'nullptr_t'
        e_OTHER    // Anything other than above
    };

    // CLASS METHODS
    static bslmf::Tag<e_BOOL>                   fromVal(bool&                );
    static bslmf::Tag<e_CHAR>                   fromVal(char&                );
    static bslmf::Tag<e_SCHAR>                  fromVal(signed char&         );
    static bslmf::Tag<e_UCHAR>                  fromVal(unsigned char&       );
    static bslmf::Tag<e_WCHAR_T>                fromVal(wchar_t&             );
#ifdef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
    static bslmf::Tag<e_CHAR16_T>               fromVal(char16_t&            );
    static bslmf::Tag<e_CHAR32_T>               fromVal(char32_t&            );
#endif
    static bslmf::Tag<e_SHORT>                  fromVal(short&               );
    static bslmf::Tag<e_USHORT>                 fromVal(unsigned short&      );
    static bslmf::Tag<e_INT>                    fromVal(int&                 );
    static bslmf::Tag<e_UNSIGNED>               fromVal(unsigned&            );
    static bslmf::Tag<e_LONG>                   fromVal(long&                );
    static bslmf::Tag<e_ULONG>                  fromVal(unsigned long&       );
    static bslmf::Tag<e_LONG_LONG>              fromVal(long long&           );
    static bslmf::Tag<e_ULONG_LONG>             fromVal(unsigned long long&  );
    static bslmf::Tag<e_FLOAT>                  fromVal(float&               );
    static bslmf::Tag<e_DOUBLE>                 fromVal(double&              );
    static bslmf::Tag<e_LONG_DOUBLE>            fromVal(long double&         );
    static bslmf::Tag<e_VOIDPTR>                fromVal(void *&              );
    static bslmf::Tag<e_CONST_VOIDPTR>          fromVal(const void *&        );
    static bslmf::Tag<e_VOLATILE_VOIDPTR>       fromVal(volatile void *&     );
    static bslmf::Tag<e_CONST_VOLATILE_VOIDPTR> fromVal(const volatile void*&);
    static bslmf::Tag<e_NULLPTR_T>              fromVal(bsl::nullptr_t&      );
    template <class TP>
    static bslmf::Tag<e_POINTER>                fromVal(TP *&                );
    template <class TP>
    static bslmf::Tag<e_OTHER>                  fromVal(TP&                  );
        // Return a tag type representing the argument type.  These functions
        // are declared but not defined and are intended to be used in an
        // unevaluated context (e.g., within 'sizeof') to convert an expression
        // into a compile-time enumeration constant.
};

template <int INDEX> struct InvokeResult_Type;
    // Metafunction to convert a type index back to a type.  For each
    // specialization of this struct, the 'type' member will be the type
    // corresponding to 'index'.  For example, if 'index' is 'e_UCHAR', then
    // 'InvokeResult_Type<index>::type' is 'unsigned char'.

// Turn off bde_verify warnings for "Declaration without tag".  Pedantically,
// every 'type' declared in a metafunction should have the tag '// TYPES', but
// that breaks up the clean 3-line declaration of each specialization, making
// the pattern harder to for the eye to follow.
// BDE_VERIFY pragma: push
// BDE_VERIFY pragma: -KS00
template <>
struct InvokeResult_Type<InvokeResult_Index::e_VOID>
    { typedef void               type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_BOOL>
    { typedef bool               type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_CHAR>
    { typedef char               type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_SCHAR>
    { typedef signed char        type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_UCHAR>
    { typedef unsigned char      type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_WCHAR_T>
    { typedef wchar_t            type; };
#ifdef BSLS_COMPILERFEATURES_SUPPORT_UNICODE_CHAR_TYPES
template <>
struct InvokeResult_Type<InvokeResult_Index::e_CHAR16_T>
    { typedef char16_t           type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_CHAR32_T>
    { typedef char32_t           type; };
#endif
template <>
struct InvokeResult_Type<InvokeResult_Index::e_SHORT>
    { typedef short              type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_USHORT>
    { typedef unsigned short     type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_INT>
    { typedef int                type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_UNSIGNED>
    { typedef unsigned           type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_LONG>
    { typedef long               type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_ULONG>
    { typedef unsigned long      type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_LONG_LONG>
    { typedef long long          type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_ULONG_LONG>
    { typedef unsigned long long type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_FLOAT>
    { typedef float              type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_DOUBLE>
    { typedef double             type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_LONG_DOUBLE>
    { typedef long double        type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_VOIDPTR>
    { typedef void              *type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_CONST_VOIDPTR>
    { typedef const void        *type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_VOLATILE_VOIDPTR>
    { typedef volatile void     *type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_CONST_VOLATILE_VOIDPTR>
    { typedef const volatile void *type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_NULLPTR_T>
    { typedef bsl::nullptr_t     type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_POINTER>
    { typedef void              *type; };
template <>
struct InvokeResult_Type<InvokeResult_Index::e_OTHER>
    { typedef InvokeResultDeductionFailed type; };
// Re-enable warnings for "Declaration without tag"
// BDE_VERIFY pragma: pop

struct InvokeResult_ImpUtils
{
    // Utility metaprogramming functions inherited by other metaprogramming
    // classes.

    // TYPES
    struct AnyLvalue {
        // Type convertible from any lvalue type.  Used for overload resolution
        // in metafunctions.

        // CREATORS
        template <class TP>
        AnyLvalue(volatile TP&);
            // (Declared but not defined) Convert from any lvalue argument.
    };

    struct AnyRvalue {
        // Type convertible from any rvalue type.  Used for overload resolution
        // in metafunctions.

        // CREATORS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
        template <class TP>
        AnyRvalue(TP&&,
                  typename bsl::enable_if<bsl::is_rvalue_reference<TP&&>::value
                                         , int>::type = 0);
            // (Declared but not defined) Convert from any rvalue argument.
#else
        template <class TP>
        AnyRvalue(TP);
            // (Declared but not defined) Convert from any rvalue argument.
            // This constructor will also match lvalue arguments, but is used
            // in a context where 'AnyLValue' is a better conversion path.
#endif
    };

    // CLASS METHODS
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    template <class SOME_TYPE>
    static typename bsl::add_rvalue_reference<SOME_TYPE>::type myDeclval();
        // Return a reference to the specified 'SOME_TYPE' template parameter
        // type; if 'SOME_TYPE' is an rvalue, then the returned reference is
        // an rvalue reference.  This function is declared but not defined and
        // is intended to be called in an unevaluated context.  Because there
        // is no definition, the available constructors for 'SOME_TYPE' are
        // irrelevant.
#else
    template <class SOME_TYPE>
    static SOME_TYPE myDeclval();
        // Return an object of the specified 'SOME_TYPE' template parameter
        // type.  This function is declared but not defined and is intended to
        // be called in an unevaluated context.  Because there is no
        // definition, the available constructors for 'SOME_TYPE' are
        // irrelevant.
#endif

    static bslmf::Tag<false>    checkLvalue(AnyRvalue, ...);
    static bslmf::Tag<true >    checkLvalue(AnyLvalue, int);
        // (Declared but not defined) Return 'bslmf::Tag<false>()' if the
        // first argument is an rvalue and 'bslmf::Tag<true>()' if it is
        // lvalue.  In actual use, the second argument is always a literal
        // 'int', which causes the second overload to be preferred in case of
        // ambiguity.

    template <class TP>
    static bslmf::Tag<false> checkConst(TP&);
    template <class TP>
    static bslmf::Tag<true > checkConst(const TP&);
        // (Declared but not defined) Return 'bslmf::Tag<true>()' if the
        // argument is 'const'-qualified and 'bslmf::Tag<false>()' otherwise.

    template <class TP>
    static bslmf::Tag<false> checkVolatile(TP&);
    template <class TP>
    static bslmf::Tag<false> checkVolatile(const TP&);
    template <class TP>
    static bslmf::Tag<true > checkVolatile(volatile TP&);
    template <class TP>
    static bslmf::Tag<true > checkVolatile(const volatile TP&);
        // (Declared but not defined) Return 'bslmf::Tag<true>()' if the
        // argument is 'volatile'-qualified and 'bslmf::Tag<false>()'
        // otherwise.  Note that if 'TP' is both const- and
        // volatile-qualified, it will not match 'volatile TP&', hence the
        // need for the const overloads.

    template <class TP>
    static TP& uncv(const TP&);
    template <class TP>
    static TP& uncv(const volatile TP&);
        // (Declared but not defined) Return the argument, with cv-qualifiers
        // removed.

    template <class TP>
    static TP& unpoint(TP&);
    template <class TP>
    static const TP& unpoint(const TP&);
    template <class TP>
    static typename bsl::enable_if<!bsl::is_void<TP>::value, TP>::type&
    unpoint(TP *&);
    template <class TP>
    static typename bsl::enable_if<!bsl::is_void<TP>::value, TP>::type&
    unpoint(TP *const&);
    template <class TP>
    static typename bsl::enable_if<!bsl::is_void<TP>::value, TP>::type&
    unpoint(TP *volatile&);
    template <class TP>
    static typename bsl::enable_if<!bsl::is_void<TP>::value, TP>::type&
    unpoint(TP *const volatile&);
        // If the argument type 'TP' is pointer to type 'X', where 'X' is not
        // cv-'void', return a reference to 'X'; otherwise return a reference
        // to 'TP'.  Note that these functions are declared but not defined and
        // are intended to be called only in an unevaluated context.
};


template <class UNQUAL_TYPE, bool IS_CONST, bool IS_VOLATILE, bool IS_LVALUE>
struct InvokeResult_AddCVRef {
    // Starting with type, 'UNQUAL_TYPE', generate a new type by applying the
    // following steps in order:
    //
    //: 1 If the specified 'IS_CONST' parameter is true, apply
    //:   'bsl::add_const'; otherwise leave unchanged.
    //: 2 If the specified 'IS_VOLATILE' parameter is true, apply
    //:   'bsl::add_volatile'; otherwise leave unchanged.
    //: 3 If the specified 'IS_LVALUE' parameter is true, apply
    //:   'bsl::add_lvalue_reference'; otherwise leave unchanged.
    //
    // Set the 'type' member to the resulting type.

  private:
    // PRIVATE TYPES
    typedef typename
    bsl::conditional<IS_CONST,
                     typename bsl::add_const<UNQUAL_TYPE>::type,
                     UNQUAL_TYPE>::type CQualType;

    typedef typename
    bsl::conditional<IS_VOLATILE,
                     typename bsl::add_volatile<CQualType>::type,
                     CQualType>::type CVQualType;

  public:
    // TYPES
    typedef typename
    bsl::conditional<IS_LVALUE,
                     typename bsl::add_lvalue_reference<CVQualType>::type,
                     CVQualType>::type type;
};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

#ifndef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <bool /* IS_VOID */, class FN, class... ARGTYPES>
struct InvokeResult_FunctorDeduction : InvokeResult_ImpUtils {
    // Deduce return type of 'FN(ARGTYPES...)'.  This template is instantiated
    // only when 'FN' is of class type (i.e., a functor).  This primary
    // template is selected when 'FN(ARGTYPES...)' is not 'void'.  Note that
    // this template is not defined in C++11 mode (if 'decltype' exists).

    typedef typename bsl::decay<FN>::type F;
        // Remove references and cv-qualifiers from 'FN', and decay function
        // types and array types to pointers.

    enum {
        // In an unevaluated context ('BSLMF_TAG_TO_INT'), "invoke"
        // 'myDeclval<FN>()(myDeclval<ARGTYPES>()...)' and use overloading to
        // deduce the type and other attributes of the return value.

        k_INDEX       = BSLMF_TAG_TO_INT(InvokeResult_Index::fromVal(
                           uncv(myDeclval<FN>()(myDeclval<ARGTYPES>()...)))),
        k_IS_POINTER  = (k_INDEX == InvokeResult_Index::e_POINTER),
        k_IS_LVALUE   = BSLMF_TAG_TO_INT(checkLvalue(
                                myDeclval<FN>()(myDeclval<ARGTYPES>()...), 0)),
        k_IS_CONST_PTR = k_IS_POINTER && BSLMF_TAG_TO_INT(checkConst(
                                myDeclval<FN>()(myDeclval<ARGTYPES>()...))),
        k_IS_VOLATILE_PTR  = k_IS_POINTER && BSLMF_TAG_TO_INT(checkVolatile(
                                myDeclval<FN>()(myDeclval<ARGTYPES>()...))),
        k_TARGET_INDEX   = BSLMF_TAG_TO_INT(InvokeResult_Index::fromVal(
                   uncv(unpoint(myDeclval<FN>()(myDeclval<ARGTYPES>()...))))),
        k_IS_CONST_TARGET    = BSLMF_TAG_TO_INT(checkConst(
                        unpoint(myDeclval<FN>()(myDeclval<ARGTYPES>()...)))),
        k_IS_VOLATILE_TARGET = BSLMF_TAG_TO_INT(checkVolatile(
                        unpoint(myDeclval<FN>()(myDeclval<ARGTYPES>()...)))),
        k_CANT_DEDUCE_TYPE = (k_TARGET_INDEX==(int)InvokeResult_Index::e_OTHER)
    };

    typedef typename bsl::conditional<
            ! k_CANT_DEDUCE_TYPE,
            typename InvokeResult_Type<k_TARGET_INDEX>::type,
            typename ResultType<F,InvokeResultDeductionFailed>::type
        >::type UnqualTargetType;
        // The deduced result after stripping off pointer, reference, and
        // cv-qualfiers.  The 'TARGET_INDEX' indicates the fundamental type of
        // the target result.  If the target could not be deduced (i.e.,
        // 'TARGET_INDEX == e_OTHER), then attempt to find the result by
        // looking for a 'result_type' or 'ResultType' alias in 'FN'; failing
        // that, use 'InvokeResultDeductionFailed'.

    typedef typename
    InvokeResult_AddCVRef<UnqualTargetType,
                          static_cast<bool>(k_IS_CONST_TARGET)
                          && ! static_cast<bool>(k_CANT_DEDUCE_TYPE),
                          static_cast<bool>(k_IS_VOLATILE_TARGET)
                          && ! static_cast<bool>(k_CANT_DEDUCE_TYPE),
                          false>::type CVQualTargetType;
        // The deduced target after adding back previously-stripped cv
        // qualifiers, if any.  Note that if the expression yielded a pointer
        // type, these cv qualifiers apply to the target of the pointer, not
        // the pointer itself.

    typedef typename
    bsl::conditional<static_cast<bool>(k_IS_POINTER)
                     && ! static_cast<bool>(k_CANT_DEDUCE_TYPE),
                     typename bsl::add_pointer<CVQualTargetType>::type,
                     CVQualTargetType>::type UnqualType;
        // The deduced result after adding back previously-stripped pointers,
        // if any.

    typedef typename
    InvokeResult_AddCVRef<
        UnqualType,
        static_cast<bool>(k_IS_CONST_PTR)
        && ! static_cast<bool>(k_CANT_DEDUCE_TYPE),
        static_cast<bool>(k_IS_VOLATILE_PTR)
        && ! static_cast<bool>(k_CANT_DEDUCE_TYPE),
        static_cast<bool>(k_IS_LVALUE)
        && ! static_cast<bool>(k_CANT_DEDUCE_TYPE)>::type Qtype;
        // The deduced result after adding back previously-stripped cv
        // qualifiers and references.  Note that if the result is a pointer,
        // the cv qualifiers apply to the pointer, not to the target.

    typedef typename bsl::conditional<static_cast<bool>(k_IS_LVALUE), Qtype,
                      typename bsl::remove_cv<Qtype>::type>::type type;
        // The final deduced result type.  If the type is not a reference,
        // top-level cv qualifiers are stripped off.
};

template <class FN, class... ARGTYPES>
struct InvokeResult_FunctorDeduction<true /* IS_VOID */, FN, ARGTYPES...>
{
    // Deduce return type of 'FN(ARGTYPES...)'.  This template is instantiated
    // only when 'FN' is of class type (i.e., a functor).  This specialization
    // is selected when 'FN(ARGTYPES...)' is cv-'void'.

    typedef void type;
};
#endif // !BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

#endif

template <class MEMOF_CLASS,
          class ARG_TYPE,
          bool  IS_DERIVED = bsl::is_convertible<
            typename bsl::decay<ARG_TYPE>::type*,
            typename bsl::decay<MEMOF_CLASS>::type*>::value>
struct InvokeResult_MemPtrArgQualifiers {
    // This metafunction determines which cv qualifiers and reference
    // qualifiers should be propagated from the first argument of
    // 'invoke_result'.  This primary template is instantiated when 'ARG_TYPE'
    // is the same or is derived from 'MEMOF_CLASS'.  The constant
    // 'k_IS_LVALUE' is true iff 'ARG_TYPE' is an lvalue reference; the
    // constant 'k_IS_CONST' is true iff 'ARG_TYPE' is const-qualified; and
    // the constant 'k_IS_VOLATILE' is true iff 'ARG_TYPE' is
    // volatile-qualified.

    // TYPES
    enum {
        k_IS_LVALUE    = bsl::is_lvalue_reference<ARG_TYPE>::value,
        k_IS_CONST     = bsl::is_const<
            typename bsl::remove_reference<ARG_TYPE>::type>::value,
        k_IS_VOLATILE  = bsl::is_volatile<
            typename bsl::remove_reference<ARG_TYPE>::type>::value
    };
};

template <class MEMOF_CLASS, class ARG_TYPE>
struct InvokeResult_MemPtrArgQualifiers<MEMOF_CLASS, ARG_TYPE, false>
    : InvokeResult_ImpUtils {
    // This metafunction determines which cv qualifiers and reference
    // qualifiers should be propagated from the first argument of
    // 'invoke_result'.
    //
    // This specialization is instantiated when 'ARG_TYPE' is not derived from
    // 'MEMOF_CLASS' and is assumed to be a pointer or smart pointer type.  If
    // type 'A' is the result of dereferencing an object of type 'ARG_TYPE',
    // then the constant 'k_IS_LVALUE' is true iff 'A' is an lvalue reference;
    // the constant 'k_IS_CONST' is true iff 'A' is a const-qualified
    // reference; and the constant 'k_IS_VOLATILE' is true iff 'A' is a
    // volatile-qualified reference.

#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
  private:
    // CLASS METHODS
    template <class TP>
    static TP& tolvalue(TP&&);
        // (Declared but not defined.)  Return an lvalue reference
        // corresponding of the specified 'TP' type, which is deduced from the
        // specified unnamed argument.  If the argument is an lvalue, the
        // return type is identical to the argument type.  If the argument is
        // an rvalue, the return type is an lvalue to the argument type with
        // the same cv qualifiers.  This function is useful for avoiding too
        // many redundant overloads in metafunctions that determine cv
        // qualifications, etc.

  public:
    // TYPES
    enum {
        k_IS_LVALUE  = BSLMF_TAG_TO_INT(checkLvalue(*myDeclval<ARG_TYPE>(),0)),
        k_IS_CONST   = BSLMF_TAG_TO_INT(checkConst(
                                           tolvalue(*myDeclval<ARG_TYPE>()))),
        k_IS_VOLATILE= BSLMF_TAG_TO_INT(checkVolatile(
                                           tolvalue(*myDeclval<ARG_TYPE>())))
    };
#else
  public:
    // TYPES
    enum {
        k_IS_LVALUE  = BSLMF_TAG_TO_INT(checkLvalue(*myDeclval<ARG_TYPE>(),0)),
        // In C++03, cv qualifiers are discarded from rvalues.
        k_IS_CONST   = k_IS_LVALUE && BSLMF_TAG_TO_INT(checkConst(
                                                      *myDeclval<ARG_TYPE>())),
        k_IS_VOLATILE= k_IS_LVALUE && BSLMF_TAG_TO_INT(checkVolatile(
                                                      *myDeclval<ARG_TYPE>()))
    };
#endif // BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES

};

#if !BSLS_COMPILERFEATURES_SIMULATE_CPP11_FEATURES // $var-args=13

template <class VOID_TYPE, class FN, class... ARGTYPES>
struct InvokeResult_FunctorImp;
    // Forward declaration

template <class VOID_TYPE, class FN, class... ARGTYPES>
struct InvokeResult_FuncPtrImp;
    // Forward declaration

template <class FN, class... ARGTYPES>
struct InvokeResult_MemFuncPtrImp;
    // Forward declaration

template <class FN, class... ARGTYPES>
struct InvokeResult_MemObjPtrImp;
    // Forward declaration

                      // ================================
                      // struct template InvokeResult_Imp
                      // ================================

template <bool IS_FUNCPTR,
          bool IS_MEMFUNCPTR,
          bool IS_MEMOBJPTR,
          class FN,
          class... ARGTYPES>
struct InvokeResult_Imp
     : InvokeResult_FunctorImp<void, FN, ARGTYPES...> {
    // This component-private, partial 'struct' template specialization
    // provides the implementation of 'InvokeResult_Imp' for types that are
    // neither function pointers, pointers to member functions, nor pointers to
    // member objects.
};

template <class FN, class... ARGTYPES>
struct InvokeResult_Imp<true /* IS_FUNCPTR */, false, false, FN, ARGTYPES...>
     : InvokeResult_FuncPtrImp<void, FN, ARGTYPES...> {
    // This component-private, partial 'struct' template specialization
    // provides the implementation of 'InvokeResult_Imp' for function pointer
    // types.
};

template <class FN, class... ARGTYPES>
struct InvokeResult_Imp<false, true/* IS_MEMFUNCPTR */, false, FN, ARGTYPES...>
     : InvokeResult_MemFuncPtrImp<FN, ARGTYPES...> {
    // This component-private, partial 'struct' template specialization
    // provides the implementation of 'InvokeResult_Imp' for pointer to member
    // function types.
};

template <class FN, class... ARGTYPES>
struct InvokeResult_Imp<false, false, true/* IS_MEMOBJPTR */, FN, ARGTYPES...>
     : InvokeResult_MemObjPtrImp<FN, ARGTYPES...> {
    // This component-private, partial 'struct' template specialization
    // provides the implementation of 'InvokeResult_Imp' for pointer to member
    // object types.
};

                  // =======================================
                  // struct template InvokeResult_FunctorImp
                  // =======================================

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE, class FN, class... ARGTYPES>
struct InvokeResult_FunctorImp {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is neither a
    // pointer-to-function, pointer-to-member-function, nor
    // pointer-to-member-object type, and the 'INVOKE(fn, args...)' expression,
    // is *not* well-formed given 'fn' is an object of the specified 'FN' type
    // and 'args...' are objects of the specified 'ARGTYPES...' types.  The
    // 'INVOKE(fn, args...)' expression in this case is 'fn(args...)'.  Note
    // that this 'struct' does not provide a 'type' typedef.
};

template <class FN, class... ARGTYPES>
struct InvokeResult_FunctorImp<
    typename bslmf::VoidType<decltype(InvokeResult_ImpUtils::myDeclval<FN>()(
        InvokeResult_ImpUtils::myDeclval<ARGTYPES>()...))>::type,
    FN,
    ARGTYPES...> : InvokeResult_ImpUtils {
        // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This
        // specialization is instantiated in C++11 and later when 'FN' is
        // neither a pointer-to-function, pointer-to-member-function, nor
        // pointer-to-member-object type, and the 'INVOKE(fn, args...)'
        // expression is well-formed given 'fn' is an object of the specified
        // 'FN' type and 'args...' are objects of the specified 'ARGTYPES...'
        // types.  The 'INVOKE(fn, args...)' expression in this case is
        // 'fn(args...)'.

    // TYPES
    typedef decltype(myDeclval<FN>()(myDeclval<ARGTYPES>()...)) type;
        // For C++11 and later, the type of the 'INVOKE(fn, args...)'
        // expression given 'fn' is an object of the specified
        // 'FN' type and 'args...' are objects of the specified 'ARGTYPES...'
        // types.  The 'INVOKE(fn, args...)' expression in this case is
        // 'fn(args...)'.
};
#else  // ! BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE, class FN, class... ARGTYPES>
struct InvokeResult_FunctorImp : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++03 when 'FN' is neither a pointer-to-function,
    // pointer-to-member-function, nor pointer-to-member-object type.

    // TYPES
    enum {
        // Determine if 'myDeclval<FN>()(myDeclval<ARGTYPES>()...)' is a void
        // expression by invoking the overloaded comma operator using a
        // 'InvokeResult_VoidChecker' as the second argument.  If the
        // expression is of void type, then the built-in comma operator will
        // yield 'InvokeResult_VoidChecker', otherwise the overloaded comma
        // operator will yield 'bslmf::Tag<false>'
        k_IS_VOID =
            BSLMF_TAG_TO_INT((myDeclval<FN>()(myDeclval<ARGTYPES>()...),
                              InvokeResult_VoidChecker()))
    };

    typedef typename InvokeResult_FunctorDeduction<k_IS_VOID,
                                                   FN,
                                                   ARGTYPES...>::type type;
        // For C++03, the result of invoking
        // 'myDeclval<FN>()(myDeclval<ARGTYPES>()...)' if it can be deduced
        // without 'decltype'; otherwise 'InvokeResultDeductionFailed'.
};
#endif  // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

                  // =======================================
                  // struct template InvokeResult_FuncPtrImp
                  // =======================================

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE, class FN, class... ARGTYPES>
struct InvokeResult_FuncPtrImp {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a pointer-to-function
    // type, and the 'INVOKE(fn, args...)' expression is *not* well-formed
    // given 'fn' is an object of the specified 'FN' type and 'args...' are
    // objects of the specified 'ARGTYPES...' types.  The 'INVOKE(fn, args...)'
    // expression in this case is 'fn(args...)'.  Note that this 'struct' does
    // not provide a 'type' typedef.
};

template <class FN, class... ARGTYPES>
struct InvokeResult_FuncPtrImp<
    typename bslmf::VoidType<decltype(InvokeResult_ImpUtils::myDeclval<FN>()(
        InvokeResult_ImpUtils::myDeclval<ARGTYPES>()...))>::type,
    FN,
    ARGTYPES...> : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a pointer-to-function
    // type, and the 'INVOKE(fn, args...)' expression is well-formed given 'fn'
    // is an object of the specified 'FN' type and 'args...' are objects of the
    // specified 'ARGTYPES...' types.  The 'INVOKE(fn, args...)' expression in
    // this case is 'fn(args...)'.

    // TYPES
    typedef decltype(myDeclval<FN>()(myDeclval<ARGTYPES>()...)) type;
        // For C++11 and later, the type of result of the 'INVOKE(fn, args...)'
        // expression where 'fn' is an object of the specified 'FN'
        // type, and 'args...' are objects of the specified 'ARGTYPES...'
        // types.  The 'INVOKE(fn, args...)' expression in this case is is
        // 'fn(args...)'.
};
#else  // ! BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE, class FN, class... ARGTYPES>
struct InvokeResult_FuncPtrImp {
    // Implementation of 'invoke_result<FN, ARGTYPES...>.  This specialization
    // is instantiated in C++03 when 'FN' is a pointer-to-function type.  Note
    // that this C++03 implementation does not check whether 'ARGTYPES...'
    // are valid for 'FN'.

    typedef typename
      bslmf::FunctionPointerTraits<FN>::ResultType QType;
        // The return value of the function pointed-to by 'FN'.

    typedef typename
      bsl::conditional<bsl::is_reference<QType>::value ||
                       bsl::is_class<QType>::value,
                       QType, typename bsl::remove_cv<QType>::type>::type type;
        // The return value of the function pointed-to by 'FN'.  If the type
        // is a scalar rvalue, cv qualifications are stripped off.
};
#endif // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

                 // ==========================================
                 // struct template InvokeResult_MemFuncPtrImp
                 // ==========================================

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE,
          bool ARG1_DERIVES_FROM_CLASS,
          bool ARG1_IS_REFERENCE_WRAPPER,
          class FN,
          class ARG1TYPE,
          class... ARGTYPES>
struct InvokeResult_MemFuncPtrImpDispatch;
    // Forward declaration.

// SPECIALIZATIONS
template <class FN>
struct InvokeResult_MemFuncPtrImp<FN> {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-function type, and the 'ARGTYPES...' pack is empty.
    // Note that this 'struct' does not provide a 'type' typedef.
};

template <class FN, class ARG1TYPE, class... ARGTYPES>
struct InvokeResult_MemFuncPtrImp<FN, ARG1TYPE, ARGTYPES...>
: InvokeResult_MemFuncPtrImpDispatch<
      void,
      InvokeResult_IsBaseOf<
          typename MemberFunctionPointerTraits<FN>::ClassType,
          typename bsl::remove_reference<ARG1TYPE>::type>::value,
      IsReferenceWrapper<typename bsl::remove_const<
          typename bsl::remove_reference<ARG1TYPE>::type>::type>::value,
      FN,
      ARG1TYPE,
      ARGTYPES...> {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-function type, and the 'ARGTYPES...' pack contains
    // 1 type or more.
};
#else // ! BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class FN, class... ARGTYPES>
struct InvokeResult_MemFuncPtrImp<FN, ARGTYPES...> {
    // Implementation of 'invoke_result<FN, ARGTYPES...>.  This specialization
    // is instantiated in C++03 when 'FN' is a pointer-to-member-function type.

    typedef typename MemberFunctionPointerTraits<FN>::ResultType QType;
        // The return value of the function pointed-to by 'FN'.

    typedef typename
      bsl::conditional<bsl::is_reference<QType>::value ||
                       bsl::is_class<QType>::value,
                       QType, typename bsl::remove_cv<QType>::type>::type type;
        // The return value of the function pointed-to by 'FN'.  If the type
        // is a scalar rvalue, cv qualifications are stripped off.
};
#endif // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

             // ==================================================
             // struct template InvokeResult_MemFuncPtrImpDispatch
             // ==================================================

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE,
          bool ARG1_DERIVES_FROM_CLASS,
          bool ARG1_IS_REFERENCE_WRAPPER,
          class FN,
          class ARG1TYPE,
          class... ARGTYPES>
struct InvokeResult_MemFuncPtrImpDispatch {
    // Implementation of 'invoke_result<FN, ARG1TYPE, ARGTYPES...>'.  This
    // specialization is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-function type, and the 'INVOKE(fn, arg1, args...)'
    // expression is *not* well-formed given 'fn' is an object of the specified
    // 'FN' type and 'arg1, args...' are objects of the specified
    // 'ARG1TYPE, ARGTYPES...' types.  Note that this 'struct' does not provide
    // a 'type' typedef.
};

template <class FN, class ARG1TYPE, class... ARGTYPES>
struct InvokeResult_MemFuncPtrImpDispatch<
    typename bslmf::VoidType<decltype(
        ((*InvokeResult_ImpUtils::myDeclval<ARG1TYPE>()).*
         InvokeResult_ImpUtils::myDeclval<FN>())(
            InvokeResult_ImpUtils::myDeclval<ARGTYPES>()...))>::type,
    /* ARG1_DERIVES_FROM_CLASS */ false,
    /* ARG1_IS_REFERENCE_WRAPPER */ false,
    FN,
    ARG1TYPE,
    ARGTYPES...> : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARG1TYPE, ARGTYPES...>'.  This
    // specialization is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-function type, 'ARG1TYPE' is neither a class type that
    // derives from the class type of 'FN' nor a specialization of
    // 'bsl::reference_wrapper', and the 'INVOKE(fn, arg1, args...)' expression
    // is well-formed given 'fn' is an object of the specified 'FN' type and
    // 'arg1, args...' are objects of the specified 'ARG1TYPE, ARGTYPES...'
    // types.  The 'INVOKE(fn, arg1, args...)' expression in this case is
    // '((*arg1).*fn)(args...)'.

    // TYPES
    typedef decltype(((*myDeclval<ARG1TYPE>()).*
                      myDeclval<FN>())(myDeclval<ARGTYPES>()...)) type;
        // For C++11 and later, the type of the 'INVOKE(fn, args...)'
        // expression where 'fn' is an object of the specified 'FN' type, and
        // 'arg1, args...' are objects of the specified 'ARG1TYPE, ARGTYPES...'
        // types.  The 'INVOKE(fn, arg1, args...)' expression in this case is
        // '((*arg1).*fn)(args...)'.
};

template <class FN, class ARG1TYPE, class... ARGTYPES>
struct InvokeResult_MemFuncPtrImpDispatch<
    typename bslmf::VoidType<decltype(
        (InvokeResult_ImpUtils::myDeclval<ARG1TYPE>().*
         InvokeResult_ImpUtils::myDeclval<FN>())(
            InvokeResult_ImpUtils::myDeclval<ARGTYPES>()...))>::type,
    /* ARG1_DERIVES_FROM_CLASS */ true,
    /* ARG1_IS_REFERENCE_WRAPPER */ false,
    FN,
    ARG1TYPE,
    ARGTYPES...> : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-function type, 'ARG1TYPE' is a class type that derives
    // from the class type of 'FN', and the 'INVOKE(fn, arg1, args...)'
    // expression is well-formed given 'fn' is an object of the specified 'FN'
    // type and 'arg1, args...' are objects of the specified
    // 'ARG1TYPE, ARGTYPES...' types.  The 'INVOKE(fn, arg1, args...)'
    // expression in this case is '(arg1.*fn)(args...)'.

    // TYPES
    typedef decltype(
     (myDeclval<ARG1TYPE>().*myDeclval<FN>())(myDeclval<ARGTYPES>()...)) type;
        // For C++11 and later, the type of the 'INVOKE(fn, arg1, args...)'
        // expression where 'fn' is an object of the specified 'FN' type, and
        // 'arg1, args...' are objects of the specified 'ARG1TYPE, ARGTYPES...'
        // types.  The 'INVOKE(fn, arg1, args...)' expression in this case is
        // '(arg1.*fn)(args...)'.
};

template <class FN, class ARG1TYPE, class... ARGTYPES>
struct InvokeResult_MemFuncPtrImpDispatch<
    typename bslmf::VoidType<decltype(
        (InvokeResult_ImpUtils::myDeclval<ARG1TYPE>().get().*
         InvokeResult_ImpUtils::myDeclval<FN>())(
            InvokeResult_ImpUtils::myDeclval<ARGTYPES>()...))>::type,
    /* ARG1_DERIVES_FROM_CLASS */ false,
    /* ARG1_IS_REFERENCE_WRAPPER */ true,
    FN,
    ARG1TYPE,
    ARGTYPES...> : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-function type, 'ARG1TYPE' is a specialization of
    // 'bsl::reference_wrapper', and the 'INVOKE(fn, arg1, args...)' expression
    // is well-formed given 'fn' is an object of the specified 'FN' type and
    // 'arg1, args...' are objects of the specified 'ARG1TYPE, ARGTYPES...'
    // types.  The 'INVOKE(fn, arg1, args...)' expression in this case is
    // '(arg1.get().*fn)(args...)'.

    // TYPES
    typedef decltype((myDeclval<ARG1TYPE>().get().*
                      myDeclval<FN>())(myDeclval<ARGTYPES>()...)) type;
        // For C++11 and later, the type of the 'INVOKE(fn, arg1, args...)'
        // expression where 'fn' is an object of the specified 'FN' type, and
        // 'arg1, args...' are objects of the specified 'ARG1TYPE, ARGTYPES...'
        // types.  The 'INVOKE(fn, arg1, args...)' expression in this case is
        // '(arg1.get().*fn)(args...)'.
};

#endif // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

                 // =========================================
                 // struct template InvokeResult_MemObjPtrImp
                 // =========================================

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE,
          bool ARG_DERIVES_FROM_CLASS,
          bool ARG_IS_REFERENCE_WRAPPER,
          class FN,
          class ARGTYPE>
struct InvokeResult_MemObjPtrImpDispatch;
    // Forward declaration.

// SPECIALIZATIONS
template <class FN, class... ARGTYPES>
struct InvokeResult_MemObjPtrImp {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-object type, and the 'ARGTYPES...' pack is empty or
    // contains more than 1 type.  Note that this 'struct' does not provide a
    // 'type' typedef.
};

template <class FN, class ARGTYPE>
struct InvokeResult_MemObjPtrImp<FN, ARGTYPE>
: InvokeResult_MemObjPtrImpDispatch<
      void,
      InvokeResult_IsBaseOf<
          typename InvokeResult_MemberObjectPointerTraits<FN>::ClassType,
          typename bsl::remove_reference<ARGTYPE>::type>::value,
      IsReferenceWrapper<typename bsl::remove_const<
          typename bsl::remove_reference<ARGTYPE>::type>::type>::value,
      FN,
      ARGTYPE> {
    // Implementation of 'invoke_result<FN, ARGTYPES...>'.  This specialization
    // is instantiated in C++11 and later when 'FN' is a
    // pointer-to-member-object type and the 'ARGTYPES...' pack contains
    // exactly 1 type.
};

#else // ! BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

template <class FN, class... ARGTYPES>
struct InvokeResult_MemObjPtrImp {
};

template <class CLASS, class RET, class ARGTYPE>
struct InvokeResult_MemObjPtrImp<RET CLASS::*, ARGTYPE> {
    // Implementation of 'invoke_result<FN, ARGTYPES...>.  This specialization
    // is instantated in C++03 when 'FN' is a pointer to data member and
    // 'ARGTYPE' is an rvalue type.

  private:
    typedef InvokeResult_MemPtrArgQualifiers<CLASS, ARGTYPE> ArgQualifiers;
        // Determine the cv qualifications and reference qualifications from
        // 'ARGTYPE' that should be applied to 'RET'.

    typedef typename InvokeResult_AddCVRef<RET,
                                           ArgQualifiers::k_IS_CONST,
                                           ArgQualifiers::k_IS_VOLATILE,
                                           ArgQualifiers::k_IS_LVALUE>::type
        cvtype;
        // The type of member pointed to by the pointer-to-member-object, with
        // cv and reference qualifiers taken from 'ARGTYPE'.

  public:
#ifdef BSLS_COMPILERFEATURES_SUPPORT_RVALUE_REFERENCES
    typedef typename bsl::conditional<
        ArgQualifiers::k_IS_LVALUE,
        cvtype,
        typename bsl::add_rvalue_reference<cvtype>::type>::type type;
        // Result type.  If rvalue references are supported, data members of
        // rvalues are always returned as rvalue references in C++11 and later.
#else
    typedef cvtype type;
        // Rvalue result type.
#endif
};
#endif // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

             // =================================================
             // struct template InvokeResult_MemObjPtrImpDispatch
             // =================================================

#ifdef BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS
template <class VOID_TYPE,
          bool ARG_DERIVES_FROM_CLASS,
          bool ARG_IS_REFERENCE_WRAPPER,
          class FN,
          class ARGTYPE>
struct InvokeResult_MemObjPtrImpDispatch {
    // Implementation of 'invoke_result<FN, ARGTYPE>'.  This specialization is
    // instantiated in C++11 and later when 'FN' is a pointer-to-member-object
    // type, and the 'INVOKE(fn, arg)' expression is *not* well-formed given
    // 'fn' is an object of the specified 'FN' type and 'arg' is an object of
    // the specified 'ARGTYPE'.  Note that this 'struct' does not provide a
    // 'type' typedef.
};

template <class FN, class ARGTYPE>
struct InvokeResult_MemObjPtrImpDispatch<
    typename bslmf::VoidType<decltype(
        (*InvokeResult_ImpUtils::myDeclval<ARGTYPE>()).*
        InvokeResult_ImpUtils::myDeclval<FN>())>::type,
    /* ARG1_DERIVES_FROM_CLASS */ false,
    /* ARG1_IS_REFERENCE_WRAPPER */ false,
    FN,
    ARGTYPE> : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARGTYPE>'.  This specialization is
    // instantiated in C++11 and later when 'FN' is a pointer-to-member-object
    // type, 'ARGTYPE' is neither a class type that derives from the class type
    // of 'FN' nor a specialization of 'bsl::reference_wrapper', and the
    // 'INVOKE(fn, arg)' expression is well-formed given 'fn' is an object of
    // the specified 'FN' type and 'arg' is an object of the specified
    // 'ARGTYPE' type.  The 'INVOKE(fn, arg)' expression in this case is
    // '(*arg).*fn'.

    // TYPES
    typedef decltype((*myDeclval<ARGTYPE>()).*myDeclval<FN>()) type;
        // For C++11 and later, the type of the 'INVOKE(fn, arg)' expression
        // where 'fn' is an object of the specified 'FN' type, and 'arg' is an
        // object of the specified 'ARGTYPE' type.  The 'INVOKE(fn, arg)'
        // expression in this case is '(*arg).*fn'.
};

template <class FN, class ARGTYPE>
struct InvokeResult_MemObjPtrImpDispatch<
    typename bslmf::VoidType<decltype(
        InvokeResult_ImpUtils::myDeclval<ARGTYPE>().*
        InvokeResult_ImpUtils::myDeclval<FN>())>::type,
    /* ARG_DERIVES_FROM_CLASS */ true,
    /* ARG_IS_REFERENCE_WRAPPER */ false,
    FN,
    ARGTYPE> : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARGTYPE>'.  This specialization is
    // instantiated in C++11 and later when 'FN' is a pointer-to-member-object
    // type, 'ARGTYPE' is a class type that derives from the class type of
    // 'FN', and the 'INVOKE(fn, arg)' expression is well-formed given 'fn' is
    // an object of the specified 'FN' type and 'arg' is an object of the
    // specified 'ARGTYPE' type.  The 'INVOKE(fn, arg)' expression in this
    // case is 'arg1.*fn'.

    // TYPES
    typedef decltype(myDeclval<ARGTYPE>().*myDeclval<FN>()) type;
        // For C++11 and later, the type of the 'INVOKE(fn, arg)' expression
        // where 'fn' is an object of the specified 'FN' type, and 'arg' is an
        // object of the specified 'ARGTYPE' type.  The 'INVOKE(fn, arg)'
        // expression in this case is 'arg1.*fn'.
};

template <class FN, class ARGTYPE>
struct InvokeResult_MemObjPtrImpDispatch<
    typename bslmf::VoidType<decltype(
        InvokeResult_ImpUtils::myDeclval<ARGTYPE>().get().*
        InvokeResult_ImpUtils::myDeclval<FN>())>::type,
    /* ARG1_DERIVES_FROM_CLASS */ false,
    /* ARG1_IS_REFERENCE_WRAPPER */ true,
    FN,
    ARGTYPE> : InvokeResult_ImpUtils {
    // Implementation of 'invoke_result<FN, ARGTYPE>'.  This specialization is
    // instantiated in C++11 and later when 'FN' is a pointer-to-member-object
    // type, 'ARGTYPE' is a specialization of 'bsl::reference_wrapper', and the
    // 'INVOKE(fn, arg)' expression is well-formed given 'fn' is an object of
    // the specified 'FN' type and 'arg' is an object of the specified
    // 'ARGTYPE' type.  The 'INVOKE(fn, arg)' expression in this case is
    // 'arg.get().*fn'.

    // TYPES
    typedef decltype(myDeclval<ARGTYPE>().get().*myDeclval<FN>()) type;
        // For C++11 and later, the type of the 'INVOKE(fn, arg)' expression
        // where 'fn' is an object of the specified 'FN' type, and 'arg' is an
        // object of the specified 'ARGTYPE' type.  The 'INVOKE(fn, arg)'
        // expression in this case is 'arg.get().*fn'.
};

#endif // BSLMF_INVOKERESULT_SUPPORT_CPP17_SEMANTICS

#endif

}  // close package namespace
}  // close enterprise namespace

#endif // End C++11 code

#endif // ! defined(INCLUDED_BSLMF_INVOKERESULT)

// ----------------------------------------------------------------------------
// Copyright 2018 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
