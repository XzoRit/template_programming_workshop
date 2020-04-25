#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <algorithm>
#include <deque>
#include <exception>
#include <forward_list>
#include <functional>
#include <iostream>
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <vector>
class string_view
{
  public:
    constexpr string_view() = default;
    template <class TStr>
    constexpr /*explicit(false)*/ string_view(const TStr& str)
        : data_{str.c_str()}
        , size_{str.size()}
    {
    }
    /*constexpr*/ /*explicit(false)*/ string_view(const char* const data)
        : data_{data}
        , size_{[data] {
            decltype(sizeof("")) size{};
            for (size = {}; data[size] != '\0'; ++size)
                ;
            return size;
        }()}
    {
    }
    constexpr string_view(const char* const data, decltype(sizeof("")) size)
        : data_{data}
        , size_{size}
    {
    }
    template <size_t N>
    constexpr string_view(const char (&data)[N])
        : data_{data}
        , size_{N - 1}
    {
    }
    template <class T>
    [[nodiscard]] constexpr operator T() const
    {
        return T{data_, size_};
    }
    template <class TOs>
    friend auto operator<<(TOs& os, const string_view& sv) -> TOs&
    {
        os.write(sv.data_, long(sv.size_));
        return os;
    }
    [[nodiscard]] friend auto operator==(const string_view& a, const string_view& b) -> bool
    {
        return strncmp(a.data_, b.data_, (std::min(a.size_, b.size_))) == 0;
    }
  private:
    const char* data_{};
    decltype(sizeof("")) size_{};
};
template <class T>
constexpr auto type_name() -> string_view
{
#if defined(_MSC_VER) and not defined(__clang__)
    return {&__FUNCSIG__[36], sizeof(__FUNCSIG__) - 44};
#elif defined(__clang__)
    return {&__PRETTY_FUNCTION__[29], sizeof(__PRETTY_FUNCTION__) - 2};
#elif defined(__GNUC__)
    return {&__PRETTY_FUNCTION__[103], sizeof(__PRETTY_FUNCTION__) - 105};
#endif
}
/*

*Template Programming Workshop



* 4 different types of computation

- runtime

- type - level computations

- heterogeneous computations

- constexpr

- file:///C:/develop/mgit_projects/thirdparty/boost/libs/hana/doc/html/index.html#tutorial-introduction-quadrants



* generic programming

- algorithms on unknown types



* problems

- safety

- performance

- memory

- ...

* safety

an error at ... is better than

- compile time

- link time

- object construction

- object use



** example

*/
namespace unsafe
{
class Location
{
  public:
    Location(std::string building, std::string pointOfCare, int floor, int bed);
};
} // namespace unsafe
// Location loc{"Main", "Intensive", 125, 3};
namespace safe
{
class Building;
class PointOfCare;
class Floor;
class Bed;
class Location
{
  public:
    Location(Building building, PointOfCare pointOfCare, Floor floor, Bed bed);
};
} // namespace safe
// Location loc{Building{"Main"}, PointOfCare{"Intensive"}, Floor{125}, Bed{3}};
/*

** named parameter

*/
namespace safe
{
template <class T, class Tag>
class NamedParameter
{
  public:
    explicit NamedParameter(const T& value)
        : m_value(value)
    {
    }
    operator T() const
    {
        return m_value;
    }
  private:
    T m_value;
};
} // namespace safe
#define NAMED_PARAMETER_TYPE(Name, WrappedType)                                                                        \
    class Name : public safe::NamedParameter<WrappedType, struct Name##Tag>                                            \
    {                                                                                                                  \
      private:                                                                                                         \
        using base = safe::NamedParameter<WrappedType, struct Name##Tag>;                                              \
      public:                                                                                                          \
        using base::base;                                                                                              \
    };
namespace LocationLib
{
NAMED_PARAMETER_TYPE(Building, std::string);
NAMED_PARAMETER_TYPE(PointOfCare, std::string);
NAMED_PARAMETER_TYPE(Floor, int);
NAMED_PARAMETER_TYPE(Bed, int);
} // namespace LocationLib
/*

- tags for parameter which wrap the same type

- examples:

   - https://en.cppreference.com/w/cpp/chrono/duration

   - file:///C:/develop/mgit_projects/binaries/DFL/doxygen/doc/html/class_d_f_l_1_1_strong_typedef.html#details

*/
namespace LocationLib
{
class Location
{
  public:
    Location(Building building, PointOfCare pointOfCare, Floor floor, Bed bed)
        : m_building(building)
        , m_pointOfCare(pointOfCare)
        , m_floor(floor)
        , m_bed(bed)
    {
    }
  private:
    std::string m_building;
    std::string m_pointOfCare;
    int m_floor;
    int m_bed;
};
} // namespace LocationLib
TEST_CASE("std::chrono::duration")
{
    using namespace std::chrono;
    using namespace std::chrono_literals;
    CHECK(1s == 1000ms);
    const milliseconds a{1s + 111ms};
    CHECK(a == 1111ms);
    const seconds b{duration_cast<seconds>(a)};
    CHECK(b == 1s);
}
TEST_CASE("ctor of Location")
{
    using namespace LocationLib;
    const Location loc{Building{"Central"}, PointOfCare{"Intensive Care"}, Floor{3}, Bed{125}};
}
/*

*deduction

*/
TEST_CASE("int to string")
{
    std::map<int, std::string> a{};
    a.insert(std::pair<int, std::string>{0, "null"});
    // template argument deduction
    // only works for functions
    a.insert(std::make_pair(1, "one"));
    // but has been extended to class template parameters
    // in c++17 -> CTAD (Class Template Argument Deduction
    a.insert(std::pair(2, "two"));
}
/*

**generator pattern

*/
namespace generator
{
template <class T>
class TemplateClass
{
    TemplateClass(T t);
};
template <class T>
TemplateClass<T> make_t(T t)
{
    return TemplateClass<T>(t);
}
} // namespace generator
/*

**examples

- http://en.cppreference.com/w/cpp/memory/unique_ptr/make_unique

- http://en.cppreference.com/w/cpp/utility/functional/bind



* template specialization

*/
template <bool x>
struct StaticAssert;
template <>
struct StaticAssert<true>
{
    static const int value = 1;
};
#define STATIC_ASSERT(Check, Msg) StaticAssert<(bool)(Check)> Msg
/*

* compute



** recursive templates



compile-time factorieal



*/
namespace compute
{
    template <int fac>
    struct factorial
    {
        static const int value = fac * factorial<fac - 1>::value;
    };
    template <>
    struct factorial<0>
    {
        static const int value = 1;
    };
}
TEST_CASE("compile-time factorial")
{
    STATIC_ASSERT(compute::factorial<6>::value == 720, Factorial6Is720);
}
/*

** compile-time pow

*/
namespace compute
{
template <int val, int exp>
struct pow
{
    static const int value = val * pow<val, exp - 1>::value;
};
template <int val>
struct pow<val, 0>
{
    static const int value = 1;
};
} // namespace compute
TEST_CASE("compile-time pow")
{
    using compute::pow;
    static_assert(pow<2, 0>::value == 1, "2 exp 0 shall be 1");
    static_assert(pow<2, 3>::value == 8, "2 exp 3 shall be 8");
    static_assert(pow<2, 10>::value == 1024, " 2 exp 10 shall be 1024");
}
/*

** constexpr

http://en.cppreference.com/w/cpp/language/constexpr

unifies compiletime functions and runtime functions

*/
namespace const_expr_11
{
constexpr int pow(int val, int exp)
{
    return exp ? val * pow(val, exp - 1) : 1;
}
} // namespace const_expr_11
namespace const_expr_14
{
constexpr int pow(int val, int exp)
{
    int result = 1;
    for (; exp; --exp)
        result *= val;
    return result;
}
} // namespace const_expr_14
TEST_CASE("constexpr pow")
{
    {
        using const_expr_11::pow;
        static_assert(pow(2, 0) == 1, "2 exp 0 shall be 1");
        static_assert(pow(2, 3) == 8, "2 exp 3 shall be 8");
        static_assert(pow(2, 10) == 1024, " 2 exp 10 shall be 1024");
    }
    {
        using const_expr_14::pow;
        static_assert(pow(2, 0) == 1, "2 exp 0 shall be 1");
        static_assert(pow(2, 3) == 8, "2 exp 3 shall be 8");
        static_assert(pow(2, 10) == 1024, " 2 exp 10 shall be 1024");
    }
}
/*

Max size only value no type

*/
namespace My
{
template <class T1, class T2>
struct MaxSize
{
    static const int value = sizeof(T1) > sizeof(T2) ? sizeof(T1) : sizeof(T2);
};
} // namespace My
#include <boost/mp11/algorithm.hpp>
#include <boost/mp11/function.hpp>
#include <boost/mp11/integral.hpp>
#include <boost/mp11/utility.hpp>
namespace Mp11
{
template <class T>
struct sizeof_t
{
    using type = T;
    constexpr static auto value{sizeof(T)};
};
namespace v1
{
using boost::mp11::mp_if_c;
template <class T1, class T2>
struct MaxSize
{
    using type = mp_if_c<(sizeof(T1) > sizeof(T2)), T1, T2>;
    constexpr static auto value{sizeof(type)};
};
} // namespace v1
namespace v2
{
using boost::mp11::mp_max;
template <class T1, class T2>
struct MaxSize
{
  private:
    using result = mp_max<sizeof_t<T1>, sizeof_t<T2>>;
  public:
    using type = typename result::type;
    constexpr static auto value{result::value};
};
} // namespace v2
namespace v3
{
using boost::mp11::mp_fold;
using boost::mp11::mp_list;
using boost::mp11::mp_max;
using boost::mp11::mp_size_t;
using boost::mp11::mp_transform;
template <class T, class... Ts>
struct MaxSize
{
  private:
    using list = mp_list<T, Ts...>;
    using sizes = mp_transform<sizeof_t, list>;
    using result = mp_fold<sizes, mp_size_t<0>, mp_max>;
  public:
    using type = typename result::type;
    constexpr static auto value{result::value};
};
} // namespace v3
namespace v4
{
using boost::mp11::mp_fold_q;
using boost::mp11::mp_list;
using boost::mp11::mp_max;
using boost::mp11::mp_size_t;
template <class T, class... Ts>
struct MaxSize
{
  private:
    struct max_sizeof
    {
        template <class T1, class T2>
        using fn = mp_max<T1, sizeof_t<T2>>;
    };
    using list = mp_list<T, Ts...>;
    using result = mp_fold_q<list, mp_size_t<0>, max_sizeof>;
  public:
    using type = typename result::type;
    constexpr static auto value{result::value};
};
} // namespace v4
namespace v5
{
using boost::mp11::mp_less;
using boost::mp11::mp_list;
using boost::mp11::mp_max_element;
using boost::mp11::mp_transform;
template <class T, class... Ts>
struct MaxSize
{
  private:
    using result = mp_max_element<mp_transform<sizeof_t, mp_list<T, Ts...>>, mp_less>;

  public:
    using type = typename result::type;
    constexpr static auto value{result::value};
};
} // namespace v5
namespace v6
{
using boost::mp11::mp_max;
template <class T, class... Ts>
struct MaxSize
{
  private:
    using result = mp_max<sizeof_t<T>, sizeof_t<Ts>...>;

  public:
    using type = typename result::type;
    constexpr static auto value{sizeof(type)};
};
} // namespace v6
} // namespace Mp11
TEST_CASE("MaxSize<T1, T2>")
{
    {
        using Mp11::v1::MaxSize;
        CHECK(type_name<MaxSize<char, int>::type>() == string_view("int"));
        static_assert(std::is_same<MaxSize<char, int>::type, int>::value, "");
        static_assert(MaxSize<char, int>::value == sizeof(int), "");
    }
    {
        using Mp11::v2::MaxSize;
        CHECK(type_name<MaxSize<char, int>::type>() == string_view("int"));
        static_assert(std::is_same<MaxSize<char, int>::type, int>::value, "");
        static_assert(MaxSize<char, int>::value == sizeof(int), "");
    }
    {
        using Mp11::v3::MaxSize;
        CHECK(type_name<MaxSize<char, int>::type>() == string_view("int"));
        static_assert(std::is_same<MaxSize<char, int>::type, int>::value, "");
        static_assert(MaxSize<char, int>::value == sizeof(int), "");
    }
    {
        using Mp11::v4::MaxSize;
        CHECK(type_name<MaxSize<char, int, double>::type>() == string_view("double"));
        static_assert(std::is_same<MaxSize<char, int, double>::type, double>::value, "");
        static_assert(MaxSize<char, int, double>::value == sizeof(double), "");
    }
    {
        using Mp11::v5::MaxSize;
        CHECK(type_name<MaxSize<char, int, double>::type>() == string_view("double"));
        static_assert(std::is_same<MaxSize<char, int, double>::type, double>::value, "");
        static_assert(MaxSize<char, int, double>::value == sizeof(double), "");
    }
    {
        using Mp11::v6::MaxSize;
        CHECK(type_name<MaxSize<char, int, double>::type>() == string_view("double"));
        static_assert(std::is_same<MaxSize<char, int, double>::type, double>::value, "");
        static_assert(MaxSize<char, int, double>::value == sizeof(double), "");
    }
}
