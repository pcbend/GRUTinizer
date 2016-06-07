#ifndef __ENUM_TO_STRING_H__
#define __ENUM_TO_STRING_H__

//see: http://stackoverflow.com/questions/28828957/enum-to-string-in-modern-c-and-future-c17

//use: 
//
//defined as,
//   ENUM(Color, char,Red,Green,Blue) 
//   constexpr Color color = Color::_from_string("Green");
//   
//called as,
//   std::cout << color._to_string() << std::endl;
//   switch (color) {
//     case Color::Red:   return 0;
//     case Color::Green: return 1;
//     case Color::Blue:  return 2;
//   };
//


#include <cstddef>
#include <cstring>
#include <stdexcept>


#define MAP(macro, ...) \
    IDENTITY( \
        APPLY(CHOOSE_MAP_START, COUNT(__VA_ARGS__)) \
            (macro, __VA_ARGS__))

#define CHOOSE_MAP_START(count) MAP ## count

#define APPLY(macro, ...) IDENTITY(macro(__VA_ARGS__))

#define IDENTITY(x) x

#define MAP1(m, x)      m(x)
#define MAP2(m, x, ...) m(x) IDENTITY(MAP1(m, __VA_ARGS__))
#define MAP3(m, x, ...) m(x) IDENTITY(MAP2(m, __VA_ARGS__))
#define MAP4(m, x, ...) m(x) IDENTITY(MAP3(m, __VA_ARGS__))
#define MAP5(m, x, ...) m(x) IDENTITY(MAP4(m, __VA_ARGS__))
#define MAP6(m, x, ...) m(x) IDENTITY(MAP5(m, __VA_ARGS__))
#define MAP7(m, x, ...) m(x) IDENTITY(MAP6(m, __VA_ARGS__))
#define MAP8(m, x, ...) m(x) IDENTITY(MAP7(m, __VA_ARGS__))

#define EVALUATE_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, count, ...) \
    count

#define COUNT(...) \
    IDENTITY(EVALUATE_COUNT(__VA_ARGS__, 8, 7, 6, 5, 4, 3, 2, 1))



template <typename U>
struct ignore_assign {
    constexpr explicit ignore_assign(U value) : _value(value) { }
    constexpr operator U() const { return _value; }

    constexpr const ignore_assign& operator =(int dummy) const
        { return *this; }

    U   _value;
};



#define IGNORE_ASSIGN_SINGLE(e) (ignore_assign<_underlying>)e,
#define IGNORE_ASSIGN(...) \
    IDENTITY(MAP(IGNORE_ASSIGN_SINGLE, __VA_ARGS__))

#define STRINGIZE_SINGLE(e) #e,
#define STRINGIZE(...) IDENTITY(MAP(STRINGIZE_SINGLE, __VA_ARGS__))

constexpr const char    terminators[] = " =\t\r\n";

constexpr bool is_terminator(char c, size_t index = 0)
{
    return
        index >= sizeof(terminators) ? false :
        c == terminators[index] ? true :
        is_terminator(c, index + 1);
}

constexpr bool matches_untrimmed(const char *untrimmed, const char *s,
                                 size_t index = 0)
{
    return
        is_terminator(untrimmed[index]) ? s[index] == '\0' :
        s[index] != untrimmed[index] ? false :
        matches_untrimmed(untrimmed, s, index + 1);
}



#define ENUM(EnumName, Underlying, ...)                               \
namespace data_ ## EnumName {                                         \
    using _underlying = Underlying;                                   \
    enum { __VA_ARGS__ };                                             \
                                                                      \
    constexpr const size_t           _size =                          \
        IDENTITY(COUNT(__VA_ARGS__));                                 \
                                                                      \
    constexpr const _underlying      _values[] =                      \
        { IDENTITY(IGNORE_ASSIGN(__VA_ARGS__)) };                     \
                                                                      \
    constexpr const char * const     _raw_names[] =                   \
        { IDENTITY(STRINGIZE(__VA_ARGS__)) };                         \
}                                                                     \
                                                                      \
struct EnumName {                                                     \
    using _underlying = Underlying;                                   \
    enum _enum : _underlying { __VA_ARGS__ };                         \
                                                                      \
    constexpr EnumName(const char* s)				      \
      : _value(_underlying_from_string(s)) { } 			      \
                                                                      \
    const char * _to_string() const                                   \
    {                                                                 \
        for (size_t index = 0; index < data_ ## EnumName::_size;      \
             ++index) {                                               \
                                                                      \
            if (data_ ## EnumName::_values[index] == _value)          \
                return _trimmed_names()[index];                       \
        }                                                             \
                                                                      \
        throw std::runtime_error("invalid value");                    \
    }                                                                 \
                                                                      \
    constexpr static EnumName _from_string(const char *s)	      \
    {                                                                 \
      return EnumName(_enum(_underlying_from_string(s)));	      \
    }                                                                 \
                                                                      \
    EnumName() = delete;                                              \
    constexpr EnumName(_enum value) : _value(value) { }               \
    constexpr operator _enum() const { return (_enum)_value; }        \
                                                                      \
  private:                                                            \
    _underlying     _value;                                           \
								      \
    constexpr static _underlying _underlying_from_string(const char* s,	\
							 size_t index = 0) \
  {									\
    return								\
      index >= data_ ## EnumName::_size ?				\
      throw std::runtime_error("invalid identifier") :			\
      matches_untrimmed(data_ ## EnumName::_raw_names[index], s) ?	\
      data_ ## EnumName::_values[index] :				\
      _underlying_from_string(s, index + 1);				\
									\
  }									\
									\
    static const char * const * _trimmed_names()			\
    {                                                                 \
        static char     *the_names[data_ ## EnumName::_size];         \
        static bool     initialized = false;                          \
                                                                      \
        if (!initialized) {                                           \
            for (size_t index = 0; index < data_ ## EnumName::_size;  \
                 ++index) {                                           \
                                                                      \
                size_t  length =                                      \
                    std::strcspn(data_ ## EnumName::_raw_names[index],\
                                 terminators);                        \
                                                                      \
                the_names[index] = new char[length + 1];              \
                                                                      \
                std::strncpy(the_names[index],                        \
                             data_ ## EnumName::_raw_names[index],    \
                             length);                                 \
                the_names[index][length] = '\0';                      \
            }                                                         \
        }                                                             \
                                                                      \
        return the_names;                                             \
    }                                                                 \
};




#endif


