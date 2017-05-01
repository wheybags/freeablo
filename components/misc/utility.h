#ifndef UTILITY_H
#define UTILITY_H
// for general, uncategorized utility functions which may later be located to appropriate files

namespace detail
{
  template<typename T, typename FUNC_T>
  class func_binder_t
  {
    T m_pointer;
    FUNC_T m_func;
  public:
    func_binder_t (T ptr, FUNC_T func): m_pointer (ptr), m_func (func) {}

    template<typename... Args>
    auto operator() (Args &&... vals) const -> decltype ((m_pointer->*m_func) (std::forward<Args> (vals)...))
    { return (m_pointer->*m_func) (std::forward<Args> (vals)...); }
  };
}

// function to create function object from pointer and member function without specifying anything about arguments
template<typename T, typename FUNC_T>
detail::func_binder_t<T, FUNC_T> bind_this (T ptr, FUNC_T func) { return {ptr, func}; }
// macro to quickly apply it to actual this and while mentioning only member function name (will not work for overloaded function)
#define BIND_THIS(FUNC_NAME) bind_this(this, &std::remove_pointer<decltype (this)>::type::FUNC_NAME)

#endif // UTILITY_H
