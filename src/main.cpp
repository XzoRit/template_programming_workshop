#define CATCH_CONFIG_COLOUR_NONE
#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <map>
#include <forward_list>
#include <iterator>
#include <exception>
#include <algorithm>

template<class T, class Tag>
class NamedParameter
{
public:
    explicit NamedParameter(const T& value)
        : m_value(value)
    {}

    operator T() const
    {
        return m_value;
    }
private:
    T m_value;
};

#define NAMED_PARAMETER_TYPE(Name, WrappedType) \
typedef NamedParameter<WrappedType, struct Name##Tag> Name

NAMED_PARAMETER_TYPE(Building, std::string);
NAMED_PARAMETER_TYPE(PointOfCare, std::string);
NAMED_PARAMETER_TYPE(Floor, int);
NAMED_PARAMETER_TYPE(Bed, int);

class Location
{
public:
    Location(
        Building building,
        PointOfCare pointOfCare,
        Floor floor,
        Bed bed)
        : m_building(building)
        , m_pointOfCare(pointOfCare)
        , m_floor(floor)
        , m_bed(bed)
    {}
private:
    std::string m_building;
    std::string m_pointOfCare;
    int m_floor;
    int m_bed;
};

TEST_CASE("ctor of Location")
{
    const Location loc(
        Building("Central"),
        PointOfCare("Intensive Care"),
        Floor(3),
        Bed(12));
}

namespace My
{
template<class T1, class T2>
struct MaxSize
{
    static const int value = sizeof(T1) > sizeof(T2) ? sizeof(T1) : sizeof(T2);
};
}
typedef My::MaxSize<int, char> MaxIntChar;
typedef My::MaxSize<int, long long> MaxIntLongLong;

struct Max1
{
    static const int value = MaxIntChar::value;
};

struct Max2
{
    static const int value = MaxIntLongLong::value;
};

template <bool>
struct StaticAssert;

template<>
struct StaticAssert<true>
{};

#define STATIC_ASSERT(Check, Msg)		\
     StaticAssert<(bool)(Check)> Msg

TEST_CASE("MaxSize at compile-time")
{
    STATIC_ASSERT(Max1::value == sizeof(int), SizeOfIntShallBeMax);
    STATIC_ASSERT(Max2::value == sizeof(long long), SizeOfLongLongShallBeMax);
}

#include <boost/mpl/if.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/int.hpp>

namespace MplMax
{
using namespace boost::mpl;
using namespace boost::mpl::placeholders;

namespace v1
{
template<class T1, class T2>
struct MaxSize
{
    typedef typename if_c<
    (sizeof(T1) > sizeof(T2)),
    T1,
    T2 >::type type;
    static const int value = sizeof(type);
};
}
namespace v2
{
template<class T1, class T2>
struct MaxSize
{
    typedef typename max< sizeof_<T1>, sizeof_<T2> >::type type;
    static const int value = type::value;
};
}
namespace v3
{
template<class Seq>
struct MaxSize
{
    typedef typename accumulate
    <
    Seq,
    int_< 0 >,
    max
    <
    sizeof_< _1 >,
    sizeof_< _2 >
    >
    >::type type;
    static const int value = type::value;
};
}
}

#include <boost/type_traits/is_same.hpp>

TEST_CASE("MaxSize with boost.mpl at compile-time")
{
    using namespace boost;
    using namespace boost::mpl;
    using namespace boost::mpl::placeholders;

    {
        using namespace MplMax::v1;
        BOOST_MPL_ASSERT_MSG(
            (is_same<MaxSize<char, int>::type, int>::value),
            IntIsBiggerThanChar,
            (char, int));
        BOOST_MPL_ASSERT_MSG(
            (bool_<MaxSize<char, int>::value == sizeof(int)>::value),
            SizeofIntIsBiggerThanSizeofChar,
            (char, int));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize<char, int>::value), ==, sizeof(int));
    }
    {
        using namespace MplMax::v2;
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize<char, int>::value), ==, sizeof(int));
    }
    {
        using namespace MplMax::v3;
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize< vector< char, int, long long > >::value), ==, sizeof(long long));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize< vector< char, int, double, long > >::value), ==, sizeof(double));
    }
}

namespace My
{
template<int fac>
struct factorial
{
    static const int value = fac * factorial<fac - 1>::value;
};

template<>
struct factorial<0>
{
    static const int value = 1;
};
}

TEST_CASE("compile-time factorial")
{
    BOOST_MPL_ASSERT_RELATION(
        My::factorial<6>::value, ==, 720);
}

namespace My
{
template<int val, int exp>
struct pow
{
    static const int value = val * pow<val, exp - 1>::value;
};

template<int val>
struct pow<val, 0>
{
    static const int value = 1;
};
namespace const_expr_11
{
constexpr int pow(int val, int exp)
{
    return exp ? val * pow(val, exp - 1) : 1;
}
}
namespace const_expr_14
{
constexpr int pow(int val, int exp)
{
    int result = 1;
    for(; exp; --exp) result *= val;
    return result;
}
}
}

TEST_CASE("compile-time pow")
{
    SECTION("c++03")
    {
        BOOST_MPL_ASSERT_RELATION(
            (My::pow<2, 0>::value), ==, 1);
        BOOST_MPL_ASSERT_RELATION(
            (My::pow<2, 1>::value), ==, 2);
        BOOST_MPL_ASSERT_RELATION(
            (My::pow<2, 3>::value), ==, 8);
        BOOST_MPL_ASSERT_RELATION(
            (My::pow<2, 10>::value), ==, 1024);
    }
    SECTION("c++11")
    {
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_11::pow(2, 0)), ==, 1);
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_11::pow(2, 1)), ==, 2);
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_11::pow(2, 3)), ==, 8);
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_11::pow(2, 10)), ==, 1024);
        CHECK(
            (My::const_expr_11::pow(2, 0)) == 1);
        CHECK(
            (My::const_expr_11::pow(2, 1)) == 2);
        CHECK(
            (My::const_expr_11::pow(2, 3)) == 8);
        CHECK(
            (My::const_expr_11::pow(2, 10)) == 1024);
    }
    SECTION("c++14")
    {
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_14::pow(2, 0)), ==, 1);
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_14::pow(2, 1)), ==, 2);
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_14::pow(2, 3)), ==, 8);
        BOOST_MPL_ASSERT_RELATION(
            (My::const_expr_14::pow(2, 10)), ==, 1024);
        CHECK(
            (My::const_expr_14::pow(2, 0)) == 1);
        CHECK(
            (My::const_expr_14::pow(2, 1)) == 2);
        CHECK(
            (My::const_expr_14::pow(2, 3)) == 8);
        CHECK(
            (My::const_expr_14::pow(2, 10)) == 1024);
    }
}

namespace My
{
namespace v1
{
template<class IterType>
void advance(IterType& it, int amount)
{
    if(amount > 0) for(; amount; --amount) ++it;
    // else for(; amount; ++amount) --it;
}
}
namespace v2
{
template<class IterType>
void advanceImpl(IterType& it, int amount, std::random_access_iterator_tag)
{
    it += amount;
}

template<class IterType>
void advanceImpl(IterType& it, int amount, std::bidirectional_iterator_tag)
{
    if(amount > 0) for(; amount; --amount) ++it;
    else for(; amount; ++amount) --it;
}

template<class IterType>
void advanceImpl(IterType& it, int amount, std::input_iterator_tag)
{
    for(; amount; --amount) ++it;
}

template<class IterType>
void advance(IterType& it, int amount)
{
    advanceImpl(it, amount, typename std::iterator_traits<IterType>::iterator_category());
}
}
}

const int a[] = {0,1,2,3,4,5,6,7,8,9};
const std::size_t sizeOfA = sizeof(a) / sizeof(a[0]);

TEST_CASE("advance for vector")
{
    using My::v2::advance;
    std::vector<int> v(std::cbegin(a), std::cend(a));
    auto it = std::cbegin(v);
    advance(it, 0);
    CHECK((*it) == 0);
    advance(it, 3);
    CHECK((*it) == 3);
    advance(it, -2);
    CHECK((*it) == 1);
}

TEST_CASE("advance for list")
{
    using My::v2::advance;
    std::list<int> v(std::cbegin(a), std::cend(a));
    auto it = std::cbegin(v);
    advance(it, 0);
    CHECK((*it) == 0);
    advance(it, 3);
    CHECK((*it) == 3);
    advance(it, -2);
    CHECK((*it) == 1);
}

TEST_CASE("advance for forward_list")
{
    using My::v2::advance;
    std::forward_list<int> v(std::cbegin(a), std::cend(a));
    auto it = std::cbegin(v);
    advance(it, 0);
    CHECK((*it) == 0);
    advance(it, 3);
    CHECK((*it) == 3);
}

template<class T>
class CheckForNull
{
public:
    static bool isValid(T* ptr)
    {
        return ptr;
    }
};

template<class T>
class ThrowException
{
public:
    static void apply(T* const &)
    {
        throw std::runtime_error("invalid ptr!");
    }
};

template<class T,
         class CheckingPolicy = CheckForNull<T>,
         class FallbackPolicy = ThrowException<T> >
class SafePtr
{
public:
    SafePtr(T* ptr)
        : m_ptr(ptr)
    {}

    ~SafePtr()
    {
        if(m_ptr) delete m_ptr;
    }

    T& operator*() const
    {
        return *getSafePtr(m_ptr);
    }

    T* operator->() const
    {
        return getSafePtr(m_ptr);
    }
private:
    static T* getSafePtr(T* const & ptr)
    {
        if(!CheckingPolicy::isValid(ptr))
        {
            FallbackPolicy::apply(ptr);
        }
        return ptr;
    }

    T* m_ptr;
};

TEST_CASE("SafePtr operator* forwarded to ptr")
{
    SafePtr<int> sp(new int(179));
    CHECK((*sp) == 179);
}

TEST_CASE("SafePtr operator-> forwarded to ptr")
{
    SafePtr<std::string> sp(new std::string("string"));
    CHECK(sp->size() == 6);
}

TEST_CASE("SafePtr with NULL throws")
{
    SafePtr<std::string> sp(NULL);
    CHECK_THROWS_AS(void(sp->size() == 6), std::runtime_error);
}

namespace crtp
{
template <class T>
class Base
{
public:
    void interface()
    {
        // ...
        static_cast<T*>(this)->implementation();
        // ...
    }

    static void staticFunc()
    {
        // ...
        T::staticSubFunc();
        // ...
    }
};

class Derived : public Base<Derived>
{
public:
    Derived()
        : m_memberImplCalled(false)
    {}
    bool m_memberImplCalled;
    static bool s_staticImplCalled;

    void implementation()
    {
        m_memberImplCalled = true;
    }
    static void staticSubFunc()
    {
        s_staticImplCalled = true;
    }
};
bool Derived::s_staticImplCalled = false;

class Cloneable
{
public:
    virtual ~Cloneable() {};
    virtual Cloneable *clone() const = 0;
};

template <typename Derived>
class CopyCloneable : public Cloneable
{
public:
    virtual Cloneable* clone() const
    {
        return new Derived(static_cast<Derived const&>(*this));
    }
};

class IAmCopyCloneable : public CopyCloneable<IAmCopyCloneable>
{
public:
    IAmCopyCloneable()
        : m_value(234)
    {}
    virtual ~IAmCopyCloneable() {}
    int m_value;
};

template<class CountMe>
class ObjectCounter
{
public:
    static int s_alive;
    static int s_created;
    static int alive()
    {
        return s_alive;
    }
    static int created()
    {
        return s_created;
    }
    ObjectCounter()
    {
        ++s_alive;
        ++s_created;
    }
    ObjectCounter(const ObjectCounter&)
    {
        ++s_alive;
        ++s_created;
    }
    ~ObjectCounter()
    {
        --s_alive;
    }
};
template<class CountMe>
int ObjectCounter<CountMe>::s_alive = 0;
template<class CountMe>
int ObjectCounter<CountMe>::s_created = 0;

class CountMe : public ObjectCounter<CountMe>
{
};
}

TEST_CASE("crtp")
{
    using namespace crtp;
    Derived derived;
    derived.interface();
    CHECK(derived.m_memberImplCalled);
    Derived::staticFunc();
    CHECK(Derived::s_staticImplCalled);
}

TEST_CASE("crtp-object-counter")
{
    using namespace crtp;
    CHECK(CountMe::created() == 0);
    CHECK(CountMe::alive() == 0);
    {
        CountMe countMe1;
        CHECK(CountMe::created() == 1);
        CHECK(CountMe::alive() == 1);
        {
            CountMe countMe2 = countMe1;
            CHECK(CountMe::created() == 2);
            CHECK(CountMe::alive() == 2);
        }
        CHECK(CountMe::created() == 2);
        CHECK(CountMe::alive() == 1);
    }
    CHECK(CountMe::created() == 2);
    CHECK(CountMe::alive() == 0);
}

#include <boost/polymorphic_cast.hpp>

TEST_CASE("cloneable")
{
    using namespace crtp;
    IAmCopyCloneable one;
    Cloneable* clone = one.clone();
    IAmCopyCloneable* iAmClone =
        boost::polymorphic_downcast<IAmCopyCloneable*>(clone);
    CHECK(one.m_value == iAmClone->m_value);
    delete clone;
}

namespace My
{
template<class T>
struct is_array
    : std::false_type
{};

template<class T>
struct is_array<T[]>
    : std::true_type
{};

template<class T, std::size_t N>
struct is_array<T[N]>
    : std::true_type
{};
}

TEST_CASE("is_array")
{
    class A {};
    BOOST_MPL_ASSERT_NOT((My::is_array<A>));
    BOOST_MPL_ASSERT((My::is_array<A[]>));
    BOOST_MPL_ASSERT((My::is_array<A[3]>));
    BOOST_MPL_ASSERT_NOT((My::is_array<float>));
    BOOST_MPL_ASSERT_NOT((My::is_array<int>));
    BOOST_MPL_ASSERT((My::is_array<int[]>));
    BOOST_MPL_ASSERT((My::is_array<int[3]>));
}

#include <boost/core/enable_if.hpp>

namespace My
{
namespace v1
{
template<class Container>
typename boost::enable_if_c<is_array<Container>::value >::type
copy_n(
    const Container&
    source, Container& target,
    std::size_t count)
{
    std::memcpy(target, source, 5);
}
template<class Container>
typename boost::enable_if_c<!is_array<Container>::value >::type
copy_n(
    const Container& source,
    Container& target,
    std::size_t count)
{
    auto&& b(std::begin(source));
    auto&& e(std::next(b, 5));
    for(; b != e; ++b)
    {
        target.push_back(*b);
    }
}
}
namespace v2
{
template<class Container>
void copy_n_impl(
    const Container& source,
    Container& target,
    std::size_t count,
    std::true_type)
{
    std::memcpy(target, source, 5);
}
template<class Container>
void copy_n_impl(
    const Container& source,
    Container& target,
    std::size_t count,
    std::false_type)
{
    auto&& b(std::begin(source));
    auto&& e(std::next(b, 5));
    for(; b != e; ++b)
    {
        target.push_back(*b);
    }
}
template<class Container>
void copy_n(const Container& source, Container& target, std::size_t count)
{
    typedef typename is_array<Container>::type maybe_array;
    copy_n_impl(source, target, count, maybe_array());
}
}
namespace v3
{
template<class Container>
void copy_n(
    const Container& source,
    Container& target,
    std::size_t count,
    typename boost::enable_if_c<is_array<Container>::value >::type* = NULL)
{
    std::memcpy(target, source, 5);
}
template<class Container>
void copy_n(
    const Container& source,
    Container& target,
    std::size_t count,
    typename boost::disable_if_c<is_array<Container>::value >::type* = NULL)
{
    auto&& b(std::begin(source));
    auto&& e(std::next(b, 5));
    for(; b != e; ++b)
    {
        target.push_back(*b);
    }
}
}
}

TEST_CASE("copy_n for containers")
{
    SECTION("container")
    {
        std::list<int> a = {1,2,3,4,5};
        std::list<int> b;
        My::v1::copy_n(a, b, 5);
        CHECK(a == b);
    }
    SECTION("array")
    {
        int a[] = {1,2,3,4,5};
        int b[5];
        My::v1::copy_n(a, b, 5);
        CHECK(*a == *b);
    }
    SECTION("container")
    {
        std::list<int> a = {1,2,3,4,5};
        std::list<int> b;
        My::v3::copy_n(a, b, 5);
        CHECK(a == b);
    }
    SECTION("array")
    {
        int a[] = {1,2,3,4,5};
        int b[5];
        My::v3::copy_n(a, b, 5);
        CHECK(*a == *b);
    }
}

namespace My
{
namespace v2
{
template<class T>
class SafePtr
{
public:
    template<class CheckingPolicy, class FallbackPolicy>
    SafePtr(T* ptr, const CheckingPolicy& c, const FallbackPolicy& f)
        : m_ptr(ptr)
        , m_checker(new CheckHolder<CheckingPolicy>(c))
        , m_fallback(new FallbackHolder<FallbackPolicy>(f))
    {}

    ~SafePtr()
    {
        delete m_ptr;
        delete m_checker;
        delete m_fallback;
    }

    T& operator*() const
    {
        return *getSafePtr(m_ptr);
    }

    T* operator->() const
    {
        return getSafePtr(m_ptr);
    }
private:
    class Check
    {
    public:
        virtual ~Check() {}
        virtual bool isValid(T* ptr) const = 0;
    };

    class Fallback
    {
    public:
        virtual ~Fallback() {}
        virtual void fallback(T* const & ptr) const = 0;
    };

    template<class CheckType>
    class CheckHolder : public Check
    {
    public:
        CheckHolder(const CheckType& value)
            : m_held(value)
        {
        }

        virtual bool isValid(T* ptr) const
        {
            return m_held(ptr);
        }
    private:
        CheckType m_held;
    };

    template<class FallbackType>
    class FallbackHolder : public Fallback
    {
    public:
        FallbackHolder(const FallbackType& value)
            : m_held(value)
        {
        }

        virtual void fallback(T* const & ptr) const
        {
            m_held(ptr);
        }
    private:
        FallbackType m_held;
    };

    T* getSafePtr(T* const & ptr) const
    {
        if(!m_checker->isValid(ptr))
        {
            m_fallback->fallback(ptr);
        }
        return ptr;
    }

    T* m_ptr;
    const Check* m_checker;
    const Fallback* m_fallback;
};

template<class T>
class CheckForNull
{
public:
    bool operator()(T* ptr) const
    {
        return ptr;
    }
};
template<class T>
class ThrowException
{
public:
    void operator()(T* const &) const
    {
        throw std::runtime_error("invalid ptr!");
    }
};
template<class T>
class DefaultConstructed
{
public:
    void operator()(T* const & ptr) const
    {
        delete ptr;
        const_cast<T*&>(ptr) = new T();
    }
};
}
}

TEST_CASE("type erased SafePtr")
{
    SECTION("SafePtr operator* forwarded to ptr")
    {
        My::v2::SafePtr<int> sp(
            new int(179),
            My::v2::CheckForNull<int>(),
            My::v2::ThrowException<int>());
        CHECK((*sp) == 179);
    }
    SECTION("SafePtr operator-> forwarded to ptr")
    {
        My::v2::SafePtr<std::string> sp(
            new std::string("string"),
            My::v2::CheckForNull<std::string>(),
            My::v2::ThrowException<std::string>());
        CHECK(sp->size() == 6);
    }
    SECTION("SafePtr with NULL throws")
    {
        My::v2::SafePtr<std::string> sp(
            NULL,
            My::v2::CheckForNull<std::string>(),
            My::v2::ThrowException<std::string>());
        CHECK_THROWS_AS(void(sp->size() == 6), std::runtime_error);
    }
    SECTION("SafePtr with NULL returns default constructed type")
    {
        My::v2::SafePtr<std::string> sp(
            NULL,
            My::v2::CheckForNull<std::string>(),
            My::v2::DefaultConstructed<std::string>());
        CHECK(sp->empty());
        CHECK(sp->size() == 0);
    }
    SECTION("SafePtr with NULL returns default constructed type")
    {
        struct Tester
        {
            int counter = 0;
            void operator()(const My::v2::SafePtr<std::string>&)
            {
                ++counter;
            }
        };
        My::v2::SafePtr<std::string> sp1(
            NULL,
            My::v2::CheckForNull<std::string>(),
            My::v2::ThrowException<std::string>());
        My::v2::SafePtr<std::string> sp2(
            NULL,
            My::v2::CheckForNull<std::string>(),
            My::v2::DefaultConstructed<std::string>());
        Tester tester;
        tester(sp1);
        tester(sp2);
        CHECK(tester.counter == 2);
    }
}
