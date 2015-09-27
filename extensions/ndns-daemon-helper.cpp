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

#include "ndns-daemon-helper.hpp"

namespace ndn {
namespace ndns {

DaemonHelper::DaemonHelper(std::string db, ndn::ndns::ManagementTool& tool)
  : m_db(db)
  , m_tool(tool)
{
}

DaemonHelper::~DaemonHelper()
{
  std::system(("rm " + m_db).c_str());
}


void
DaemonHelper::createZone(std::string& zoneStr, std::string& parentStr, int cacheTtlInt, int certTtlInt,
                         std::string& kskStr, std::string& dskStr)
{
  m_tool.createZone(Name(zoneStr), Name(parentStr), time::seconds(cacheTtlInt),
                    time::seconds(certTtlInt), Name(kskStr), Name(dskStr));
}

void
DaemonHelper::createRR(std::string& zoneStr, std::string& rrLabelStr, std::string& ndnsTypeStr,
                       std::string& rrTypeStr, int version, std::vector<std::string>& content,
                       std::string& dsk, int ttl)
{
  m_tool.addRrSet(Name(zoneStr), Name(rrLabelStr), ndn::Name::Component(rrTypeStr),
                  ndns::toNdnsType(ndnsTypeStr), static_cast<uint64_t>(version),
                  content, Name(dsk), time::seconds(ttl));
}

void
DaemonHelper::setDb(std::string& dbStr)
{
  m_db = dbStr;
}

} // ndns namespace
} // ndn namespace
