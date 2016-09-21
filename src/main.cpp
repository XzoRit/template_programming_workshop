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
#include <numeric>
#include <functional>

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
#include <boost/mpl/transform.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/protect.hpp>
#include <boost/mpl/lambda.hpp>

namespace MplMax
{
namespace v1
{
using namespace boost::mpl;
using namespace boost::mpl::placeholders;

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
using namespace boost::mpl;
using namespace boost::mpl::placeholders;

template<class T1, class T2>
struct MaxSize
{
    typedef typename max< sizeof_<T1>, sizeof_<T2> >::type type;
    static const int value = type::value;
};
}
namespace v3
{
using boost::mpl::transform;
using boost::mpl::sizeof_;
using boost::mpl::accumulate;
using boost::mpl::size_t;
using boost::mpl::protect;
using boost::mpl::lambda;
using boost::mpl::placeholders::_1;
using boost::mpl::placeholders::_2;

namespace MaxSize
{
namespace runtime
{
namespace v1
{
static auto value(const std::vector<std::string>& strings)
{
    std::vector<std::string::size_type> sizes(strings.size());
    std::transform(
        std::begin(strings), std::end(strings),
        std::begin(sizes),
        [](const auto& a)
    {
        return a.size();
    });

    return std::accumulate(
               std::begin(sizes), std::end(sizes),
               std::string::size_type{0},
               [](auto a, auto b)
    {
        return std::max(a, b);
    });
}
}
namespace v2
{
static auto value(const std::vector<std::string>& strings)
{
    return std::accumulate(
               std::begin(strings), std::end(strings),
               std::string::size_type{0},
               [](auto a, auto b)
    {
        return std::max(a, b.size());
    });
}
}
}
struct compiletime
{
    template<class Seq>
    struct v1
    {
        typedef typename transform
        <
        Seq,
        sizeof_< _1 >
        >::type sizes;

        typedef typename accumulate
        <
        sizes,
        size_t< 0 >,
        boost::mpl::max< _1, _2 >
        >::type type;
        static const int value = type::value;
    };

    template<class Seq>
    struct v2
    {
        typedef typename lambda
        <
        sizeof_< _1 >
        >::type sizeof_type;

        typedef typename accumulate
        <
        Seq,
        size_t< 0 >,
        boost::mpl::max< _1, protect< sizeof_type >::type::apply< _2 > >
        >::type type;
        static const int value = type::value;
    };
};
}
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
        using namespace std::string_literals;
        using namespace MplMax::v3;
        using MaxSize::runtime::v1::value;

        CHECK((value({"char"     , "short", "int"  , "long long"})) == "long long"s.size());
        CHECK((value({"long long", "int"  , "short", "char"     })) == "long long"s.size());
        CHECK((value({"float"    , "double"                     })) == "double"s.size());
        CHECK((value({"double"   , "float"     		        })) == "double"s.size());
    }
    {
        using namespace std::string_literals;
        using namespace MplMax::v3;
        using MaxSize::runtime::v2::value;

        CHECK((value({"char"     , "short", "int"  , "long long"})) == "long long"s.size());
        CHECK((value({"long long", "int"  , "short", "char"     })) == "long long"s.size());
        CHECK((value({"float"    , "double"                     })) == "double"s.size());
        CHECK((value({"double"   , "float"     		        })) == "double"s.size());
    }
    {
        using namespace MplMax::v3;

        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v1< vector< char, short, int, long long > >::value),
            ==,
            sizeof(long long));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v1< vector< long long, int, short, char > >::value),
            ==,
            sizeof(long long));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v1< vector< float, double > >::value),
            ==,
            sizeof(double));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v1< vector< double, float > >::value),
            ==,
            sizeof(double));
    }
    {
        using namespace MplMax::v3;

        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v2< vector< char, short, int, long long > >::value),
            ==,
            sizeof(long long));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v2< vector< long long, int, short, char > >::value),
            ==,
            sizeof(long long));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v2< vector< float, double > >::value),
            ==,
            sizeof(double));
        BOOST_MPL_ASSERT_RELATION(
            (MaxSize::compiletime::v2< vector< double, float > >::value),
            ==,
            sizeof(double));
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

#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/add_pointer.hpp>
#include <boost/mpl/transform.hpp>

namespace My
{
namespace v1
{
template<class T>
struct add_pointer
{
    typedef T* type;
};
}
namespace v2
{
template<class T>
struct add_pointer
{
    typedef typename boost::remove_reference<T>::type wo_ref;
    typedef wo_ref* type;
};
}
namespace v3
{
using namespace boost::mpl;

template<class Types>
struct add_pointer : transform< Types, boost::add_pointer<_1> >
{};
}
}

#include <boost/type_traits/is_same.hpp>

TEST_CASE("add_pointer, value types")
{
    static_assert(
        boost::is_same<
        int*,
        My::v1::add_pointer<int>::type
        >::value, "");
    static_assert(
        boost::is_same<
        const int*,
        My::v1::add_pointer<const int>::type
        >::value, "");
}

TEST_CASE("add_pointer, reference types")
{
    static_assert(
        boost::is_same<
        int*,
        My::v2::add_pointer<int&>::type
        >::value, "");
    static_assert(
        boost::is_same<
        const int*,
        My::v2::add_pointer<const int&>::type
        >::value, "");
}

#include <boost/mpl/equal.hpp>

TEST_CASE("add_pointer, sequence of types")
{
    using namespace boost;

    static_assert(
        boost::mpl::equal<
        mpl::vector<char*,short*,int*,long*,float*,double*>,
        My::v3::add_pointer<mpl::vector<char,short,int,long,float,double>>::type
        >::value, "");
    static_assert(
        boost::mpl::equal<
        mpl::vector<char*,short*,int*,long*,float*,double*>,
        My::v3::add_pointer<mpl::vector<char&,short&,int&,long&,float&,double&>>::type
        >::value, "");
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

TEST_CASE("SafePtr with nullptr throws")
{
    SafePtr<std::string> sp(nullptr);
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

class YouAreCopyCloneable : public CopyCloneable<YouAreCopyCloneable>
{
public:
    YouAreCopyCloneable()
        : m_value(234)
    {}
    virtual ~YouAreCopyCloneable() {}
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
    {
        IAmCopyCloneable one;
        Cloneable* clone = one.clone();
        IAmCopyCloneable* iAmClone =
            boost::polymorphic_downcast<IAmCopyCloneable*>(clone);
        CHECK(one.m_value == iAmClone->m_value);
        delete clone;
    }
    {
        YouAreCopyCloneable one;
        Cloneable* clone = one.clone();
        YouAreCopyCloneable* iAmClone =
            boost::polymorphic_downcast<YouAreCopyCloneable*>(clone);
        delete clone;
    }
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
typename boost::enable_if<is_array<Container> >::type
copy_n(
    const Container&
    source, Container& target,
    std::size_t count)
{
    using value_type = decltype(*std::begin(source));
    std::memcpy(target, source, sizeof(value_type) * count);
}
template<class Container>
typename boost::disable_if<is_array<Container> >::type
copy_n(
    const Container& source,
    Container& target,
    std::size_t count)
{
    auto&& b(std::begin(source));
    auto&& e(std::next(b, count));
    for(; b != e; ++b)
    {
        target.push_back(*b);
    }
}
}
namespace v2
{
template<class Container>
void copy_n(
    const Container& source,
    Container& target,
    std::size_t count,
    typename boost::enable_if<is_array<Container> >::type* = nullptr)
{
    using value_type = decltype(*std::begin(source));
    std::memcpy(target, source, sizeof(value_type) * count);
}
template<class Container>
void copy_n(
    const Container& source,
    Container& target,
    std::size_t count,
    typename boost::disable_if<is_array<Container> >::type* = nullptr)
{
    auto&& b(std::begin(source));
    auto&& e(std::next(b, count));
    for(; b != e; ++b)
    {
        target.push_back(*b);
    }
}
}
namespace v3
{
template<class Container>
void copy_n_impl(
    const Container& source,
    Container& target,
    std::size_t count,
    std::true_type)
{
    using value_type = decltype(*std::begin(source));
    std::memcpy(target, source, sizeof(value_type) * count);
}
template<class Container>
void copy_n_impl(
    const Container& source,
    Container& target,
    std::size_t count,
    std::false_type)
{
    auto&& b(std::begin(source));
    auto&& e(std::next(b, count));
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
        CHECK(std::equal(std::begin(a), std::end(a), std::begin(b)));
    }
    SECTION("container")
    {
        std::list<int> a = {1,2,3,4,5};
        std::list<int> b;
        My::v2::copy_n(a, b, 5);
        CHECK(a == b);
    }
    SECTION("array")
    {
        int a[] = {1,2,3,4,5};
        int b[5];
        My::v2::copy_n(a, b, 5);
        CHECK(std::equal(std::begin(a), std::end(a), std::begin(b)));
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
        CHECK(std::equal(std::begin(a), std::end(a), std::begin(b)));
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
    SECTION("SafePtr with nullptr throws")
    {
        My::v2::SafePtr<std::string> sp(
            nullptr,
            My::v2::CheckForNull<std::string>(),
            My::v2::ThrowException<std::string>());
        CHECK_THROWS_AS(void(sp->size() == 6), std::runtime_error);
    }
    SECTION("SafePtr with nullptr returns default constructed type")
    {
        My::v2::SafePtr<std::string> sp(
            nullptr,
            My::v2::CheckForNull<std::string>(),
            My::v2::DefaultConstructed<std::string>());
        CHECK(sp->empty());
        CHECK(sp->size() == 0);
    }
    SECTION("SafePtr with nullptr returns default constructed type")
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
            nullptr,
            My::v2::CheckForNull<std::string>(),
            My::v2::ThrowException<std::string>());
        My::v2::SafePtr<std::string> sp2(
            nullptr,
            My::v2::CheckForNull<std::string>(),
            My::v2::DefaultConstructed<std::string>());
        Tester tester;
        tester(sp1);
        tester(sp2);
        CHECK(tester.counter == 2);
    }
}

namespace geometry
{
namespace lib_one
{
struct Point
{
    int x;
    int y;
};
}
namespace lib_two
{
struct Point
{
    int x() const
    {
        return _x;
    }
    int y() const
    {
        return _y;
    }
    void x(int xx)
    {
        _x = xx;
    }
    void y(int yy)
    {
        _y = yy;
    }
private:
    int _x, _y;
};
}
}

#include <boost/fusion/adapted/struct/adapt_struct.hpp>

BOOST_FUSION_ADAPT_STRUCT(
    geometry::lib_one::Point,
    x,
    y)

#include <boost/fusion/adapted/adt.hpp>

BOOST_FUSION_ADAPT_ADT(
    geometry::lib_two::Point,
    (obj.x(), obj.x(val))
    (obj.y(), obj.y(val)))

#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/zip_view.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/next.hpp>
#include <boost/fusion/include/deref.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/accumulate.hpp>
#include <boost/fusion/include/tuple.hpp>
#include <numeric>

namespace geometry
{
namespace lib
{
template<class P1, class P2>
auto distance(P1 p1, P2 p2)
{
    using namespace boost::fusion;
    static_assert(
        tuple_size<P1>::value
        ==
        tuple_size<P2>::value,
        "points must have same dimension");
    typedef vector<P1&, P2&> Points;
    typedef zip_view<Points> ZippedPoints;
    ZippedPoints zippedPoints{Points{p1, p2}};
    const auto acc = accumulate(
                         zippedPoints,
                         0,
                         [](const auto& acc, const auto& pointAxe)
    {
        const auto diff = deref(begin(pointAxe)) - deref(next(begin(pointAxe)));
        return acc + diff * diff;
    });
    return sqrt(acc);
}
}
}

TEST_CASE("boost.fusion distance of two points")
{
    using namespace geometry;
    lib_one::Point p1{};
    p1.x = 2;
    p1.y = 2;
    lib_two::Point p2{};
    p2.x(2);
    p2.y(6);
    CHECK(4.0 == lib::distance(p1, p2));
}
