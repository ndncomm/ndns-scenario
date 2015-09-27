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

#include "ndns-add-rr.hpp"
#include "ndns-create-zone.hpp"
#include "ndns-daemon.hpp"
#include "ndns-delete-zone.hpp"
#include "ndns-dig.hpp"
#include "ndns-get-rr.hpp"
#include "ndns-remove-rr.hpp"
#include "ndns-update.hpp"

namespace ndn {
namespace ndns {

class DaemonHelper
{
public:
  DaemonHelper(std::string db, ndn::ndns::ManagementTool& tool);

  void
  createZone(std::string& zoneStr, std::string& parentStr, int cacheTtlInt, int certTtlInt,
             std::string& kskStr, std::string& dskStr);

  void
  createRR(std::string& zoneStr, std::string& rrLabelStr, std::string& ndnsTypeStr,
           std::string& rrTypeStr, int version, std::vector<std::string>& content,
           std::string& dsk, int ttl);

  void
  setDb(std::string& dbStr);

private:
  std::string m_db;
  ndn::ndns::ManagementTool& m_tool;
};

} // ndns namespace
} // ndn namespace
