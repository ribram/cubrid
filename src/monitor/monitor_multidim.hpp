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

#include <string>
#include <vector>

namespace cubmonitor
{
  using dimension = std::vector<std::string>;

  template <typename S, dimension ... Ds>
  class multidim_statistic
  {
      // todo
  };


  //template <typename S, dimension D>
  //using array_statistic = multidim_statistic<S, D>;

  //using sample_array = array_statistic<amount_accumulator_statistic, SAMPLE_DIM>;


} // namespace cubmonitor

#endif // _MONITOR_MULTIDIM_HPP_
