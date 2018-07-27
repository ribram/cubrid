/*
 * Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution.
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

//
// monitor_multidim.hpp - interface for collecting multi-dimensional statistics
//
//    multi-dimensional statistics is just a wrapper structure over specialized statistics collectors like the ones
//    found in monitor_statistics.hpp, monitor_transaction.hpp and monitor_collect.hpp
//

#if !defined _MONITOR_MULTIDIM_HPP_
#define _MONITOR_MULTIDIM_HPP_

#include "monitor_definition.hpp"
#include "monitor_registration.hpp"

#include <cassert>

namespace cubmonitor
{
  //  array_statistics
  //
  //  description:
  //
  //      array of statistics collectors. operator[] allows accessing a statistic collector by index
  //
  //  templates:
  //
  //      S - value type; must be a statistic
  //
  //      N - array size
  //
  //  how to use:
  //
  //      const std::size_t ARRAY_SIZE = 2;
  //      array_statistics<amount_accumulator_statistic, ARRAY_SIZE> my_stat_array;
  //
  //      my_stat_array[0].collect (1);     // first statistic collector will add 1
  //      my_stat_array[1].collect (2);     // second statistic collector will add 2
  //
  //      // NOTE: you can create multi-dimensional statistics by nesting two or more arrays
  //      using first_dim = array_statistics<amount_accumulator_statistic, ARRAY_SIZE_1>;
  //      using second_dim = array_statistic<first_dim, ARRAY_SIZE_2>;
  //
  //      second_dim twodim_stat;
  //      twodim_stat[0][0].collect (1);    // voila
  //
  template <class S, std::size_t N>
  class array_statistics
  {
    public:
      using value_type = S;
      static const std::size_t SIZE = N;

      value_type &operator[] (std::size_t index);

      // fetch/registration interface
      inline void fetch (statistic_value *destination, fetch_mode mode = FETCH_GLOBAL) const;
      std::size_t get_statistics_count (void) const;

    private:
      value_type m_array[SIZE];
  };

  //  enumerated_array_statistics
  //
  //  description:
  //
  //      wrapper over a regular statistics array. operator[] allows accessing a statistic collector by enumeration
  //
  //  templates:
  //
  //      S - value type; must be a statistic
  //
  //      IE - indexable enumeration concept, that must cover next public properties:
  //
  //          enum_type = type of enumerator used by operator[]
  //          SIZE = static const std::size_t for array size
  //          static std::size_t to_index (enum_type e) = function to convert enumeration value to index
  //
  //  how to use:
  //
  //      // to use we need an enumerator and an indexable enumerator class
  //      const std::size_t ARRAY_SIZE = 2;
  //      enum my_enum
  //      {
  //        FIRST_VALUE = 10, SECOND_VALUE = 11
  //      }
  //
  //      class my_indexable_enumerator
  //      {
  //      public:
  //        using enum_type = my_enum;
  //        static const std::size_t SIZE = ARRAY_SIZE;
  //
  //        static std::size_t to_index (my_enum e)
  //        {
  //          switch (e):
  //          case FIRST_VALUE:
  //            return 0;
  //          case SECOND_VALUE:
  //            return 1;
  //        }
  //      }
  //
  //      // define my enumerated_array_statistics
  //      using my_enum_array_stat = enumerated_array_statistics<amount_accumulator_statistic,
  //                                                             my_indexable_enumerator>;
  //
  //      my_enum_array_stat enum_array_stat_instance;
  //
  //      enum_array_stat_instance[FIRST_VALUE].collect (1);
  //      enum_array_stat_instance[SECOND_VALUE].collect (2);
  //
  //      // NOTE: enumerated array statistics can be nested similarly to array statistics.
  //
  template <class S, typename IE>
  class enumerated_array_statistics : public array_statistics<S, IE::SIZE>
  {
    public:
      value_type &operator[] (typename IE::enum_type e)
      {
	return this->array_statistics<S, IE::SIZE>::operator[] (IE::to_index (e));
      }
  };

  //////////////////////////////////////////////////////////////////////////
  // Template/inline implementation
  //////////////////////////////////////////////////////////////////////////

  template <class S, std::size_t N>
  typename array_statistics<S, N>::value_type &
  array_statistics<S, N>::operator[] (std::size_t index)
  {
    assert (index < SIZE);
    return m_array[index];
  }

  template <class S, std::size_t N>
  std::size_t
  array_statistics<S, N>::get_statistics_count (void) const
  {
    std::size_t total_count = 0;
    for (std::size_t i = 0; i < SIZE; i++)
      {
	total_count += m_array[i].get_statistics_count ();
      }
    return total_count;
  }

  template <class S, std::size_t N>
  void
  array_statistics<S, N>::fetch (statistic_value *destination, fetch_mode mode /* = FETCH_GLOBAL */) const
  {
    statistic_value *statsp = destination;
    for (std::size_t i = 0; i < SIZE; i++)
      {
	m_array[index].fetch (statsp, mode);
	statsp += m_array[index].get_statistics_count ();
      }
    assert (statsp - destination == get_statistics_count ());
  }

} // namespace cubmonitor

#endif // _MONITOR_MULTIDIM_HPP_
