
#include <boost/core/enable_if.hpp>
#include <boost/fusion/adapted/adt.hpp>
#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/accumulate.hpp>
#include <boost/fusion/include/begin.hpp>
#include <boost/fusion/include/deref.hpp>
#include <boost/fusion/include/next.hpp>
#include <boost/fusion/include/size.hpp>
#include <boost/fusion/include/tuple.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/zip_view.hpp>

#include <boost/mpl/accumulate.hpp>
#include <boost/mpl/assert.hpp>
#include <boost/mpl/deref.hpp>
#include <boost/mpl/equal.hpp>
#include <boost/mpl/if.hpp>
#include <boost/mpl/int.hpp>
#include <boost/mpl/lambda.hpp>
#include <boost/mpl/max_element.hpp>
#include <boost/mpl/min_max.hpp>
#include <boost/mpl/placeholders.hpp>
#include <boost/mpl/protect.hpp>
#include <boost/mpl/size_t.hpp>
#include <boost/mpl/sizeof.hpp>
#include <boost/mpl/transform.hpp>
#include <boost/mpl/transform_view.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/vector_c.hpp>

#include <boost/polymorphic_cast.hpp>

#include <boost/type_traits/add_pointer.hpp>
#include <boost/type_traits/is_same.hpp>
#include <boost/type_traits/remove_reference.hpp>

#include <algorithm>
#include <cassert>
#include <deque>
#include <exception>
#include <forward_list>
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <set>
#include <string>
#include <type_traits>
#include <vector>

using namespace std::string_literals;

using namespace boost;
using namespace boost::mpl;
using namespace boost::mpl::placeholders;

///////////////////////////////////////////
// snippet 1
///////////////////////////////////////////

class NamedParameter
{
public:
    explicit NamedParameter(const string& value)
        : m_value{value}
        {}
    operator string() const
        { return m_value; }
private:
    string m_value;
};

class Location
{
public:
    Location(const Building&
             , const PointOfCare&
             , const Floor&
             , const Bed&)
        {}
};

const Location loc(Building("Central")
                   , PointOfCare("Intensive Care")
                   , Floor(3)
                   , Bed(12)
    );

///////////////////////////////////////////
// snippet 2
///////////////////////////////////////////

template<class T1, class T2>
struct MaxSize
{
    static const int value = ...
};

assert(MaxSize<int, char>::value == sizeof(int));
assert(MaxSize<int, long long>::value == sizeof(long long));

///////////////////////////////////////////
// snippet 3
///////////////////////////////////////////

namespace v1
{
using namespace boost::mpl;
using namespace boost::mpl::placeholders;

template<class T1, class T2>
struct MaxSize
{
    typedef typename if_c < ...
};
}

///////////////////////////////////////////
// snippet 4
///////////////////////////////////////////

namespace v2
{
using namespace boost::mpl;
using namespace boost::mpl::placeholders;

template<class T1, class T2>
struct MaxSize
{
    typedef typename max< ...
};
}

///////////////////////////////////////////
// snippet 5
///////////////////////////////////////////

namespace My
{
template<int val, int exp>
struct pow
{
    static const int value = val ...
};

template<...>
struct pow<...>
{
    static const int value ...
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

///////////////////////////////////////////
// snippet 6
///////////////////////////////////////////

namespace v3
{
using boost::mpl::accumulate;
using boost::mpl::deref;
using boost::mpl::lambda;
using boost::mpl::max_element;
using boost::mpl::protect;
using boost::mpl::size_t;
using boost::mpl::sizeof_;
using boost::mpl::transform;
using boost::mpl::transform_view;

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
        [](const auto & a)
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
namespace v3
{
static auto value(const std::vector<std::string>& strings)
{
    return std::max_element(
               std::begin(strings), std::end(strings),
               [](auto a, auto b)
    {
        return a.size() < b.size();
    })->size();
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
            ...

        typedef typename accumulate
        <
            ...

        static const int value ...
    };

    template<class Seq>
    struct v2
    {
        typedef typename lambda
        <
            ...

        typedef typename accumulate
        <
            ...

        static const int value = ...
    };


    template<class Seq>
    struct v3
    {
        typedef typename max_element
        <
            transform_view
            <
                ...

        static const int value = ...
    };
};
}
}
}
}

///////////////////////////////////////////
// snippet 7
///////////////////////////////////////////

namespace My
{
namespace v1
{
template<class T>
struct add_pointer
{
    typedef ...
};
}
namespace v2
{
template<class T>
struct add_pointer
{
    typedef ...
};
}
namespace v3
{
using namespace boost::mpl;

template<class Types>
struct add_pointer : transform< ...
{};
}
}

///////////////////////////////////////////
// snippet 8
///////////////////////////////////////////

namespace My
{
namespace v1
{
template<class IterType>
void advance(IterType& it, int amount)
{
    ...
}
}
// namespace v2
// {
// template<class IterType>
// void advanceImpl(IterType& it, int amount, ...
// {
// }

// template<class IterType>
// void advanceImpl(IterType& it, int amount, ...
// {
// }

// template<class IterType>
// void advanceImpl(IterType& it, int amount, ...
// {
// }

// template<class IterType>
// void advance(IterType& it, int amount)
// {
//     advanceImpl(it, amount, ...
// }
// }
}

const int a[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
const std::size_t sizeOfA = sizeof(a) / sizeof(a[0]);

{
    std::vector<int> v{0,1,2,3,4,5,6,7,8,9};
    auto it = std::cbegin(v);
    advance(it, 0);
    assert((*it) == 0);
    advance(it, 3);
    assert((*it) == 3);
    advance(it, -2);
    assert((*it) == 1);
}

// {
//     std::list<int> v(std::cbegin(a), std::cend(a));
//     auto it = std::cbegin(v);
//     advance(it, 0);
//     CHECK((*it) == 0);
//     advance(it, 3);
//     CHECK((*it) == 3);
//     advance(it, -2);
//     CHECK((*it) == 1);
// }

// {
//     std::forward_list<int> v(std::cbegin(a), std::cend(a));
//     auto it = std::cbegin(v);
//     advance(it, 0);
//     CHECK((*it) == 0);
//     advance(it, 3);
//     CHECK((*it) == 3);
// }

///////////////////////////////////////////
// snippet 9
///////////////////////////////////////////
template<class T>
class CheckForNull
{
public:
    static bool isValid(T const* ptr)
    {
        ...
    }
};

template<class T>
class ThrowException
{
public:
    static void apply(T*&)
    {
        ...
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
        delete m_ptr;
        m_ptr = nullptr;
    }

    T& operator*()
    {
        return *getSafePtr(m_ptr);
    }

    T* operator->()
    {
        return getSafePtr(m_ptr);
    }
private:
    static T* getSafePtr(T*& ptr)
    {
        if(!CheckingPolicy::isValid(ptr))
        {
            FallbackPolicy::apply(ptr);
        }
        return ptr;
    }

    T* m_ptr;
};

{
    SafePtr<int> sp(new int(179));
    assert((*sp) == 179);
}

{
    SafePtr<std::string> sp(new std::string("string"));
    assert(sp->size() == 6);
}

{
    SafePtr<std::string> sp(nullptr);
    try
    {
        static_cast<void>(sp->size());
        assert(false);
    }
    catch(const std::runtime_error& e)
    {
        assert(std::string{e.what()} == "invalid ptr!"s);
    }
}

///////////////////////////////////////////
// snippet 10
///////////////////////////////////////////

class Cloneable
{
public:
    virtual ~Cloneable() {};
    virtual Cloneable* clone() const = 0;
};

template <typename Derived>
class CopyCloneable : public Cloneable
{
public:
    virtual Cloneable* clone() const
    {
        return ...
    }
};

class IAmCopyCloneable : ...
{
public:
    IAmCopyCloneable()
        : m_value(234)
    {}
    virtual ~IAmCopyCloneable() {}
    int m_value;
};

class YouAreCopyCloneable : ...
{
public:
    YouAreCopyCloneable()
        : m_value(234)
    {}
    virtual ~YouAreCopyCloneable() {}
    int m_value;
};

{
    using namespace crtp;
    {
        IAmCopyCloneable one;
        Cloneable* clone = one.clone();
        IAmCopyCloneable* iAmClone =
            boost::polymorphic_downcast<IAmCopyCloneable*>(clone);
        assert(one.m_value == iAmClone->m_value);
        delete clone;
    }
    {
        YouAreCopyCloneable one;
        Cloneable* clone = one.clone();
        YouAreCopyCloneable* iAmClone =
            boost::polymorphic_downcast<YouAreCopyCloneable*>(clone);
        assert(one.m_value == iAmClone->m_value);
        delete clone;
    }
}

///////////////////////////////////////////
// snippet 11
///////////////////////////////////////////

namespace My
{
namespace v1
{
template<class Container>
typename enable_if< ...
copy_n(
    const Container&
    source, Container& target,
    std::size_t count)
{
    using value_type = decltype(*std::begin(source));
    std::memcpy(target, source, sizeof(value_type) * count);
}
template<class Container>
typename enable_if<! ...
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
// namespace v2
// {
// template<class Container>
// void copy_n(
//     const Container& source,
//     Container& target,
//     std::size_t count,
//     typename enable_if< ...
// {
//     using value_type = decltype(*std::begin(source));
//     std::memcpy(target, source, sizeof(value_type) * count);
// }
// template<class Container>
// void copy_n(
//     const Container& source,
//     Container& target,
//     std::size_t count,
//     typename disable_if<! ...
// {
//     auto&& b(std::begin(source));
//     auto&& e(std::next(b, count));
//     for(; b != e; ++b)
//     {
//         target.push_back(*b);
//     }
// }
// }
// namespace v3
// {
// template<class Container>
// void copy_n_impl(
//     const Container& source,
//     Container& target,
//     std::size_t count,
//     std::true_type)
// {
//     using value_type = decltype(*std::begin(source));
//     std::memcpy(target, source, sizeof(value_type) * count);
// }
// template<class Container>
// void copy_n_impl(
//     const Container& source,
//     Container& target,
//     std::size_t count,
//     std::false_type)
// {
//     auto&& b(std::begin(source));
//     auto&& e(std::next(b, count));
//     for(; b != e; ++b)
//     {
//         target.push_back(*b);
//     }
// }
// template<class Container>
// void copy_n(const Container& source, Container& target, std::size_t count)
// {
//     typedef typename ... maybe_array;
//     copy_n_impl(source, target, count, maybe_array());
// }
// }
}

{
    {
        std::list<int> a = {1, 2, 3, 4, 5};
        std::list<int> b;
        My::v1::copy_n(a, b, 5);
        assert(a == b);
    }
    {
        int a[] = {1, 2, 3, 4, 5};
        int b[5];
        My::v1::copy_n(a, b, 5);
        assert(std::equal(std::begin(a), std::end(a), std::begin(b)));
    }
    // {
    //     std::list<int> a = {1, 2, 3, 4, 5};
    //     std::list<int> b;
    //     My::v2::copy_n(a, b, 5);
    //     assert(a == b);
    // }
    // {
    //     int a[] = {1, 2, 3, 4, 5};
    //     int b[5];
    //     My::v2::copy_n(a, b, 5);
    //     assert(std::equal(std::begin(a), std::end(a), std::begin(b)));
    // }
    // {
    //     std::list<int> a = {1, 2, 3, 4, 5};
    //     std::list<int> b;
    //     My::v3::copy_n(a, b, 5);
    //     assert(a == b);
    // }
    // {
    //     int a[] = {1, 2, 3, 4, 5};
    //     int b[5];
    //     My::v3::copy_n(a, b, 5);
    //     assert(std::equal(std::begin(a), std::end(a), std::begin(b)));
    // }
}

///////////////////////////////////////////
// snippet 12
///////////////////////////////////////////
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

    T& operator*()
    {
        return *getSafePtr(m_ptr);
    }

    T* operator->()
    {
        return getSafePtr(m_ptr);
    }
private:
    class Check
    {
    public:
        virtual ~Check() {}
        virtual bool isValid(T const* ptr) const = 0;
    };

    class Fallback
    {
    public:
        virtual ~Fallback() {}
        virtual void fallback(T*& ptr) = 0;
    };

    template<class CheckType>
    class CheckHolder : public Check
    {
        ...
    };

    template<class FallbackType>
    class FallbackHolder : public Fallback
    {
        ...
    };

    T* getSafePtr(T*& ptr)
    {
        if(!m_checker->isValid(ptr))
        {
            m_fallback->fallback(ptr);
        }
        return ptr;
    }

    T* m_ptr;
    const Check* m_checker;
    Fallback* m_fallback;
};

template<class T>
class CheckForNull
{
public:
    bool operator()(T const* ptr) const
    {
        ...
    }
};
template<class T>
class ThrowException
{
public:
    void operator()(T*&) const
    {
        ...
    }
};
template<class T>
class DefaultConstructed
{
public:
    void operator()(T*& ptr)
    {
        ...
    }
};
}
}

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

///////////////////////////////////////////
// snippet 13
///////////////////////////////////////////

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

BOOST_FUSION_ADAPT_STRUCT(
    geometry::lib_one::Point,
    x,
    y)

BOOST_FUSION_ADAPT_ADT(
    geometry::lib_two::Point,
    (obj.x(), obj.x(val))
    (obj.y(), obj.y(val)))

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

{
    using namespace geometry;
    lib_one::Point p1{};
    p1.x = 2;
    p1.y = 2;
    lib_two::Point p2{};
    p2.x(2);
    p2.y(6);
    assert(4.0 == lib::distance(p1, p2));
}
