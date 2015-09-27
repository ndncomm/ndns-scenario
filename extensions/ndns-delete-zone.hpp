/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014, Regents of the University of California.
 *
 * This file is part of NDNS (Named Data Networking Domain Name Service).
 * See AUTHORS.md for complete list of NDNS authors and contributors.
 *
 * NDNS is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NDNS is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NDNS, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mgmt/management-tool.hpp"
#include "ndns-label.hpp"

#include <boost/filesystem.hpp>

#include <string>

namespace ndn {
namespace ndns {

class NdnsDeleteZone
{
public:
  NdnsDeleteZone(const string& dbPath,
                 const Name& zone)
    : m_keyChain(ns3::ndn::StackHelper::getKeyChain())
    , m_dbPath(dbPath)
    , m_zone(zone)
  {
  }

  void
  start()
  {
    ndn::ndns::ManagementTool tool(m_dbPath, m_keyChain);
    tool.deleteZone(m_zone);
  }

  void
  stop()
  {
  }

protected:
  KeyChain& m_keyChain;
  string m_dbPath;
  Name m_zone;
};

} // namespace ndns
} // namespace ndn
