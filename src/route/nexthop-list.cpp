/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  The University of Memphis,
 *                           Regents of the University of California,
 *                           Arizona Board of Regents.
 *
 * This file is part of NLSR (Named-data Link State Routing).
 * See AUTHORS.md for complete list of NLSR authors and contributors.
 *
 * NLSR is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NLSR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NLSR, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "common.hpp"
#include "nexthop-list.hpp"
#include "nexthop.hpp"
#include "logger.hpp"

namespace nlsr {

INIT_LOGGER("NexthopList");

using namespace std;

static bool
nexthopAddCompare(const NextHop& nh1, const NextHop& nh2)
{
  return nh1.getConnectingFaceUri() == nh2.getConnectingFaceUri();
}

static bool
nexthopRemoveCompare(const NextHop& nh1, const NextHop& nh2)
{
  return (nh1.getConnectingFaceUri() == nh2.getConnectingFaceUri() &&
          nh1.getRouteCostAsAdjustedInteger() == nh2.getRouteCostAsAdjustedInteger()) ;
}

bool
operator==(const NexthopList& lhs, const NexthopList& rhs)
{
  if (lhs.getSize() != rhs.getSize()) {
    return false;
  }

  NexthopList slhs = lhs;
  NexthopList srhs = rhs;

  for (struct {std::set<NextHop>::iterator lItr;
    std::set<NextHop>::iterator rItr;} pair = {slhs.begin(), srhs.begin()};
       (pair.lItr != slhs.end() || pair.rItr != srhs.end());
       pair.rItr++, pair.lItr++) {
    if (!((*pair.lItr) == (*pair.rItr))) {
      return false;
    }
  }
  return true;
}

std::ostream&
operator<<(std::ostream& os, const NexthopList& nhl)
{
  NexthopList& ucnhl = const_cast<NexthopList&>(nhl);
  os << "NexthopList(\nNext hops: ";
  for (auto&& nh : ucnhl.getNextHops()) {
    os << nh;
  }
  os << ")";
  return os;
}

void
NexthopList::addNextHop(const NextHop& nh)
{
  std::set<NextHop, NextHopComparator>::iterator it = std::find_if(m_nexthopList.begin(),
                                                 m_nexthopList.end(),
                                                 ndn::bind(&nexthopAddCompare, _1, nh));
  if (it == m_nexthopList.end()) {
    m_nexthopList.insert(nh);
  }
  else if (it->getRouteCost() > nh.getRouteCost()) {
    removeNextHop(*it);
    m_nexthopList.insert(nh);
  }
}

void
NexthopList::removeNextHop(const NextHop& nh)
{
  std::set<NextHop, NextHopComparator>::iterator it = std::find_if(m_nexthopList.begin(),
                                                 m_nexthopList.end(),
                                                 ndn::bind(&nexthopRemoveCompare, _1, nh));
  if (it != m_nexthopList.end()) {
    m_nexthopList.erase(it);
  }
}

void
NexthopList::writeLog()
{
  int i = 1;

  for (std::set<NextHop, NextHopComparator>::iterator it = m_nexthopList.begin();
       it != m_nexthopList.end() ; it++, i++) {
    _LOG_DEBUG("Nexthop " << i << ": " << (*it).getConnectingFaceUri()
               << " Route Cost: " << (*it).getRouteCost());
  }
}

} // namespace nlsr
