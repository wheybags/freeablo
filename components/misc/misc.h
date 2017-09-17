#ifndef FA_MISC_H
#define FA_MISC_H

#include <utility>
#include <stdint.h>
#include <stddef.h>

namespace Misc
{
    int32_t getVecDir(const std::pair<float, float>& vector);

    template <typename T>
    std::pair<float, float> getVec(const std::pair<T,T>& from, const std::pair<T,T>& to)
    {
        return std::make_pair(((float)to.first)-((float)from.first), ((float)to.second)-((float)from.second));
    }
    std::pair<int32_t, int32_t> getNextPosByDir(std::pair<int32_t, int32_t> pos, int32_t dir);

    // Point in pixels
    struct Point
    {
      int32_t x;
      int32_t y;
      Point operator+ (const Point &v) const { return {x + v.x, y + v.y}; }
      Point operator- (const Point &v) const { return {x - v.x, y - v.y}; }
      Point operator* (double c) const { return {static_cast<int32_t> (x * c), static_cast<int32_t> (y * c)}; }
      Point operator/ (int c) const { return {x / c, y / c}; }
    };

    namespace detail
    {
      template <typename RetType, typename... Args> class overload_class;

      template <typename RetType, typename Arg>
      class overload_class<RetType, Arg> : Arg
      {
        using self = overload_class<RetType, Arg>;
      public:
        using Arg::operator ();
        template <class ActualArg, typename std::enable_if<!std::is_same<typename std::decay<ActualArg>::type, self>::value, int>::type = 0>
        overload_class(const ActualArg &arg) : Arg(arg) {}
      };

      template <typename RetType, typename Arg0, typename... Args>
      class overload_class<RetType, Arg0, Args...> : Arg0, overload_class<RetType, Args...>
      {
        using self = overload_class<RetType, Arg0, Args...>;
        using parent_t = overload_class<RetType, Args...>;

      public:
        using result_type = RetType;

      public:
        using Arg0::operator();
        using parent_t::operator ();

        template <class ActualArg0, class... ActualArgs, typename std::enable_if<!std::is_same<typename std::decay<ActualArg0>::type, self>::value, int>::type = 0>
        overload_class(const ActualArg0 &arg0, const ActualArgs &... args) : Arg0(arg0), parent_t(args...) {}
      };

    template <typename T>
    struct wrapper_for
    {
      using type = T;
    };

    template <typename T> struct func_ptr_wrapper;
    template <typename Ret, typename... Args>
    struct func_ptr_wrapper<Ret(Args...)>
    {
      using func_ptr_t = Ret(*)(Args...);
      func_ptr_t m_ptr;
      func_ptr_wrapper(func_ptr_t ptr) : m_ptr(ptr) {}
      template <typename... ActualArgs>
      auto operator()(ActualArgs&&... args) -> decltype (this->m_ptr(std::forward<ActualArgs>(args)...)) { return this->m_ptr(std::forward<ActualArgs>(args)...); }
    };

    template <typename Ret, typename ...Args>
    struct wrapper_for<Ret(*) (Args...)>
    {
      using type = func_ptr_wrapper<Ret(Args...)>;
    };

    template <typename T>
    using wrapper_for_t = typename wrapper_for<T>::type;
    }  // namespace detail

    // generate class overloading operator() of several function objects to ease the usage of variant classes
    template <typename RetType = void, typename... Args>
    auto overload(Args&&... args) -> detail::overload_class<RetType, detail::wrapper_for_t<typename std::decay<Args>::type>...> { return { std::forward<Args>(args)... }; }
}

#define UNUSED_PARAM(x) (void)(x)

#endif
