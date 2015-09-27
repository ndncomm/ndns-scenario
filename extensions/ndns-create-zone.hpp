/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2015, Regents of the University of California.
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

#include "ns3/ndnSIM-module.h"

#include "mgmt/management-tool.hpp"
#include "ndns-label.hpp"
#include "logger.hpp"
#include "config.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <string>

namespace ndn {
namespace ndns {

class NdnsCreateZone {
public:
  NdnsCreateZone(std::string db, ndn::ndns::ManagementTool& tool)
    : m_db(db)
    , m_tool(tool)
  {
  }

  void
  createZone() {
    ndn::ndns::ManagementTool tool(m_db, m_keyChain);
    //m_tool = tool;
    m_tool.createZone(m_zone, m_parent, m_cacheTtl, m_certTtl, m_ksk, m_dsk);
  }

  time::seconds
  getCacheTtl()
  {
    return m_cacheTtl;
  }

  void
  setCacheTtl(int cacheTtlInt)
  {
    if (cacheTtlInt == -1)
      m_cacheTtl = ndns::DEFAULT_CACHE_TTL;
    else
      m_cacheTtl = time::seconds(cacheTtlInt);
  }

  time::seconds
  getCertTtl()
  {
    return m_certTtl;
  }

  void
  setCertTtl(int certTtlInt)
  {
    if (certTtlInt == -1)
      m_certTtl = ndns::DEFAULT_CERT_TTL;
    else
      m_certTtl = time::seconds(certTtlInt);
  }

  Name
  getZone()
  {
    return m_zone;
  }

  void
  setZone(std::string zoneStr)
  {
    m_zone = Name(zoneStr);
  }

  Name
  getParent()
  {
    return m_parent;
  }

  void
  setParent(std::string parentStr)
  {
    m_parent = Name(parentStr);
  }

  Name
  getDsk()
  {
    return m_dsk;
  }

  void
  setDsk(std::string dskStr)
  {
    m_dsk = Name(dskStr);
  }

  Name
  getKsk()
  {
    return m_ksk;
  }

  void
  setKsk(std::string kskStr)
  {
    m_ksk = Name(kskStr);
  }

  Name
  getDb()
  {
    return m_db;
  }

  void
  setDb(std::string dbStr)
  {
    m_db = dbStr;
  }

private:
  time::seconds m_cacheTtl;
  time::seconds m_certTtl;
  Name m_zone;
  Name m_parent;
  Name m_dsk;
  Name m_ksk;
  string m_db;
  ndn::KeyChain& m_keyChain = ns3::ndn::StackHelper::getKeyChain();
  ndn::ndns::ManagementTool& m_tool;
};

} // ndns namespace
} // ndd namespace
