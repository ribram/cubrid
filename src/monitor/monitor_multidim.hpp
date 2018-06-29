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

namespace cubmonitor
{
  template <class S, std::size_t N>
  class array_statistics
  {
    public:
      using subtype = S;
      static const std::size_t SIZE = N;

      subtype &operator[] (std::size_t index);

      // fetch/registration interface
      inline void fetch (statistic_value *destination, fetch_mode mode = FETCH_GLOBAL) const;
      std::size_t get_statistics_count (void) const;

    private:
      subtype m_array[SIZE];
  };

  //////////////////////////////////////////////////////////////////////////
  // Template/inline implementation
  //////////////////////////////////////////////////////////////////////////

  template <class S, std::size_t N>
  typename array_statistics<S, N>::subtype &
  array_statistics<S, N>::operator[] (std::size_t index)
  {
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
