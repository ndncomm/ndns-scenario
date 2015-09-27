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

#include "ns3/ndnsim-module.h"

#include "clients/response.hpp"
#include "clients/query.hpp"

#include "daemon/db-mgr.hpp"

#include "ndns-label.hpp"
#include "ndns-enum.hpp"
#include "ndns-tlv.hpp"

#include "util/util.hpp"

#include "logger.hpp"
#include "validator.hpp"

#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/data.hpp>
#include <ndn-cxx/util/io.hpp>
#include <ndn-cxx/encoding/block.hpp>
#include <ndn-cxx/encoding/block-helpers.hpp>
#include <boost/noncopyable.hpp>

//#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

#include <string>
#include <tuple>

namespace ndn {
namespace ndns {

class NdnsUpdate : noncopyable
{
public:

  NdnsUpdate(Face& face,
             const Name& hint,
             const Name& zone,
             const Name& rrLabel,
             const std::vector<string>& contents,
             const Name& certName = Name(),
             string rrType = "TXT",
             string ndnsTypeStr = "resp")
    : m_hint(hint)
    , m_zone(zone)
    , m_interestLifetime(DEFAULT_INTEREST_LIFETIME)
    , m_face(face)
    , m_validator(face)
    , m_hasError(false)
  {
    configure(certName, rrLabel, contents, rrType, ndnsTypeStr);
  }

  void
  start()
  {
    std::cout << " ================ "
              << "start to update RR at Zone = " << this->m_zone
              << " new RR is: " << m_update->getName()
              <<" =================== " << std::endl;

    std::cout << "new RR is signed by: "
              << m_update->getSignature().getKeyLocator().getName()
              << std::endl;

    Interest interest = this->makeUpdateInterest();
    std::cout << "[* <- *] send Update: " << m_update->getName().toUri() << std::endl;
    m_face.expressInterest(interest,
                           bind(&NdnsUpdate::onData, this, _1, _2),
                           bind(&NdnsUpdate::onTimeout, this, _1) //dynamic binding
                           );
  }

  void
  stop()
  {
    // m_face.getIoService().stop();
  }

private:
  bool
  configure(Name certName,
            const Name& rrLabel,
            const std::vector<string>& contents,
            const string& rrType,
            const string& ndnsTypeStr)
  {
    NdnsType ndnsType = toNdnsType(ndnsTypeStr);
    auto& keyChain = ns3::ndn::StackHelper::getKeyChain();

    if (certName.empty()) {
      const Name name = Name().append(m_zone).append(rrLabel);
      // choosing the longest match of the identity who also have default certificate
      for (size_t i = name.size() + 1; i > 0; --i) { // i >= 0 will present warning
        const Name prefix(name.getPrefix(i - 1));

        if (keyChain.doesIdentityExist(prefix)) {
          try {
            certName = keyChain.getDefaultCertificateNameForIdentity(prefix);
            break;
          }
          catch (std::exception&) {
            // If it cannot get a default certificate from one identity,
            // just ignore this one try next identity.
            ;
          }
        }
      } // for

      if (certName.empty()) {
        std::cerr << "cannot figure out the certificate automatically. "
                  << "please set it with -c CERT_NAME" << std::endl;
        return false;
      }
    } else {
      if (!keyChain.doesCertificateExist(certName)) {
        std::cerr << "certificate: " << certName << " does not exist" << std::endl;
        return false;
      }
    }

    if (ndnsType == ndns::NDNS_UNKNOWN) {
      std::cerr << "unknown NdnsType: " << ndnsTypeStr << std::endl;
      return false;
    }

    Response re;
    re.setZone(m_zone);
    re.setRrLabel(rrLabel);
    name::Component qType = (rrType == "ID-CERT" ?
                             ndns::label::NDNS_CERT_QUERY : ndns::label::NDNS_ITERATIVE_QUERY);

    re.setQueryType(qType);
    re.setRrType(name::Component(rrType));
    re.setNdnsType(ndnsType);

    for (const auto& content : contents) {
      re.addRr(ndn::dataBlock(ndn::ndns::tlv::RrData,
                              content.c_str(),
                              content.size()));
    }

    m_update = re.toData();
    keyChain.sign(*m_update, certName);

    return true;
  }

private:
  void
  onData(const Interest& interest, const Data& data)
  {
    std::cout << "get response of Update" << std::endl;
    int ret = -1;
    std::string msg;
    std::tie(ret, msg) = this->parseResponse(data);
    std::cout << "Return Code: " << ret << ", and Update "
              << (ret == UPDATE_OK ? "succeeds" : "fails") << std::endl;
    if (ret != UPDATE_OK)
      m_hasError = true;

    if (!msg.empty()) {
      std::cout << "Return Msg: " << msg << std::endl;;
    }

    std::cout << "to verify the response" << std::endl;
    m_validator.validate(data,
                         bind(&NdnsUpdate::onDataValidated, this, _1),
                         bind(&NdnsUpdate::onDataValidationFailed, this, _1, _2));
  }

  std::tuple<int, std::string>
  parseResponse(const Data& data)
  {
    int ret = -1;
    std::string msg;
    Block blk = data.getContent();
    blk.parse();
    Block block = blk.blockFromValue();
    block.parse();
    Block::element_const_iterator val = block.elements_begin();
    for (; val != block.elements_end(); ++val) {
      if (val->type() == ndns::tlv::UpdateReturnCode) { // the first must be return code
        ret = readNonNegativeInteger(*val);
      }
      else if (val->type() == ndns::tlv::UpdateReturnMsg) {
        msg =  std::string(reinterpret_cast<const char*>(val->value()), val->value_size());
      }
    }

    return std::make_tuple(ret, msg);
  }

  /**
   * @brief construct a query (interest) which contains the update information
   */
  Interest
  makeUpdateInterest()
  {
    Query q(m_hint, m_zone, label::NDNS_ITERATIVE_QUERY);
    q.setRrLabel(Name().append(m_update->wireEncode()));
    q.setRrType(label::NDNS_UPDATE_LABEL);
    q.setInterestLifetime(m_interestLifetime);

    return q.toInterest();
  }

private:
  void
  onTimeout(const ndn::Interest& interest)
  {
    std::cout << "Update timeouts" << std::endl;
    m_hasError = true;
    this->stop();
  }

  void
  onDataValidated(const shared_ptr<const Data>& data)
  {
    std::cout << "data pass verification" << std::endl;
    this->stop();
  }

  void
  onDataValidationFailed(const shared_ptr<const Data>& data, const std::string& str)
  {
    std::cout << "data does not pass verification" << std::endl;
    m_hasError = true;
    this->stop();
  }

public:

  void
  setInterestLifetime(const time::milliseconds& interestLifetime)
  {
    m_interestLifetime = interestLifetime;
  }

  const bool
  hasError() const
  {
    return m_hasError;
  }

private:
  Name m_hint;
  Name m_zone;

  time::milliseconds m_interestLifetime;

  Face& m_face;
  Validator m_validator;
  KeyChain m_keyChain;

  shared_ptr<Data> m_update;
  bool m_hasError;
};

} // namespace ndns
} // namespace ndn
