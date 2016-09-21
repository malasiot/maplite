#ifndef __TUPLE_HASH_HPP__
#define __TUPLE_HASH_HPP__

namespace std{
  namespace
  {
    template <class t>
    inline void hash_combine(std::size_t& seed, t const& v)
    {
      seed ^= hash<t>()(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    template <class tuple,
             size_t index = std::tuple_size<tuple>::value - 1>
    struct hashvalueimpl
    {
      static void apply(size_t& seed, tuple const& t)
      {
        hashvalueimpl<tuple, index-1>::apply(seed, t);
        hash_combine(seed, get<index>(t));
      }
    };

    template <class tuple>
    struct hashvalueimpl<tuple,0>
    {
      static void apply(size_t& seed, tuple const& t)
      {
        hash_combine(seed, get<0>(t));
      }
    };
  }

  template <typename ... tt>
  struct hash<std::tuple<tt...>>
  {
    size_t
      operator()(std::tuple<tt...> const& t) const
      {
        size_t seed = 0;
        hashvalueimpl<std::tuple<tt...> >::apply(seed, t);
        return seed;
      }

  };
}

#endif
