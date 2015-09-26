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

#include "daemon/name-server.hpp"
#include "logger.hpp"
#include "config.hpp"
#include "daemon/config-file.hpp"
#include "ndn-cxx/security/key-chain.hpp"
#include <boost/program_options.hpp>

namespace ndn {
namespace ndns {

class NdnsDaemon
{
public:
  DEFINE_ERROR(Error, std::runtime_error);

  NdnsDaemon(const std::string& configFile)
  {
    ConfigFile config;

    config.addSectionHandler("zones",
                               bind(&NdnsDaemon::processZonesSection, this, _1, _3));
    // config.addSectionHandler("hints",
    //                            bind(&NdnsDaemon::processHintsSection, this, _1, _3));

    config.parse(configFile, false);
  }

  void
  processZonesSection(const ndn::ndns::ConfigSection& section, const std::string& filename)
  {
    using namespace boost::filesystem;
    using namespace ndn::ndns;
    using ndn::ndns::ConfigSection;

    if (section.begin() == section.end()) {
      throw Error("zones section is empty");
    }

    std::string dbFile = " "; // DEFAULT_DATABASE_PATH "/" "ndns.db";
    ConfigSection::const_assoc_iterator item = section.find("dbFile");
    if (item != section.not_found()) {
      dbFile = item->second.get_value<std::string>();
    }
    NDNS_LOG_INFO("DbFile = " << dbFile);
    m_dbMgr = unique_ptr<DbMgr>(new DbMgr(dbFile));

    std::string validatorConfigFile = " "; // DEFAULT_CONFIG_PATH "/" "validator.conf";
    item = section.find("validatorConfigFile");
    if (item != section.not_found()) {
      validatorConfigFile = item->second.get_value<std::string>();
    }
    NDNS_LOG_INFO("ValidatorConfigFile = " << validatorConfigFile);
    m_validator = unique_ptr<Validator>(new Validator(m_validatorFace, validatorConfigFile));

    for (const auto& option : section) {
      Name name;
      Name cert;
      if (option.first == "zone") {
        try {
          name = option.second.get<Name>("name"); // exception leads to exit
        }
        catch (const std::exception& e) {
          NDNS_LOG_ERROR("Required `name' attribute missing in `zone' section");
          throw Error("Required `name' attribute missing in `zone' section");
        }
        try {
          cert = option.second.get<Name>("cert");
        }
        catch (std::exception&) {
          ;
        }


        if (!m_keyChain.doesIdentityExist(name)) {
          NDNS_LOG_FATAL("Identity: " << name << " does not exist in the KeyChain");
          throw Error("Identity does not exist in the KeyChain");
        }

        if (cert.empty()) {
          try {
            cert = m_keyChain.getDefaultCertificateNameForIdentity(name);
          }
          catch (std::exception& e) {
            NDNS_LOG_FATAL("Identity: " << name << " does not have default certificate. "
                           << e.what());
            throw Error("identity does not have default certificate");
          }
        }
        else {
          if (!m_keyChain.doesCertificateExist(cert)) {
            throw Error("Certificate `" + cert.toUri() + "` does not exist in the KeyChain");
          }
        }
        NDNS_LOG_TRACE("name = " << name << " cert = " << cert);
        m_servers.push_back(make_shared<NameServer>(name, cert, m_face, *m_dbMgr,
                                                    m_keyChain, *m_validator));
      }
    } // for
  }

private:
  Face m_face;
  Face m_validatorFace;
  unique_ptr<DbMgr> m_dbMgr;
  unique_ptr<Validator> m_validator;
  std::vector<shared_ptr<NameServer>> m_servers;
  KeyChain& m_keyChain = ns3::ndn::StackHelper::getKeyChain();
};

} // ndns namespace
} // ndn namespace
