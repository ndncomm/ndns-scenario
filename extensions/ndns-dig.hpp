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

#include "ndns-label.hpp"
#include "logger.hpp"
#include "clients/response.hpp"
#include "clients/query.hpp"
#include "clients/iterative-query-controller.hpp"
#include "validator.hpp"
#include "util/util.hpp"

#include <ndn-cxx/security/key-chain.hpp>
#include <ndn-cxx/face.hpp>
#include <boost/program_options.hpp>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>
#include <boost/noncopyable.hpp>

#include <memory>
#include <string>


namespace ndn {
namespace ndns {

class NdnsDig
{

public:
  NdnsDig()
    : m_validator(m_face)
  {
    m_interestLifetime = DEFAULT_INTEREST_LIFETIME;
    m_hasError = false;
    if (m_shouldValidateIntermediate)
      m_ctr = std::unique_ptr<IterativeQueryController>
        (new IterativeQueryController(m_dstLabel, m_rrType, m_interestLifetime,
                                      bind(&NdnsDig::onSucceed, this, _1, _2),
                                      bind(&NdnsDig::onFail, this, _1, _2),
                                      m_face, &m_validator));
    else
      m_ctr = std::unique_ptr<IterativeQueryController>
        (new IterativeQueryController(m_dstLabel, m_rrType, m_interestLifetime,
                                      bind(&NdnsDig::onSucceed, this, _1, _2),
                                      bind(&NdnsDig::onFail, this, _1, _2),
                                      m_face, nullptr));
  }

  void
  setDstLabel(Name dstLabel)
  {
    m_dstLabel = dstLabel;
  }

  void
  setInterestLifetime(const time::milliseconds& lifetime)
  {
    m_interestLifetime = lifetime;
  }

  const bool
  hasError() const
  {
    return m_hasError;
  }

  void
  setDstFile(const std::string& dstFile)
  {
    m_dstFile = dstFile;
  }

  void
  setRrType(name::Component rrType)
  {
    m_rrType = rrType;
  }

  void
  setHint(Name hint)
  {
    m_hint = hint;
  }

  void
  setShouldValidateIntermediate(bool shouldValidateIntermediate)
  {
    m_shouldValidateIntermediate = shouldValidateIntermediate;
  }

  void
  run()
  {
    NDNS_LOG_INFO(" =================================== "
                  << "start to dig label = " << this->m_dstLabel
                  << " for type = " << this->m_rrType
                  << " =================================== ");

    try {
      m_ctr->start(); // non-block, may throw exception
    }
    catch (std::exception& e) {
      std::cerr << "Error: " << e.what();
      m_hasError = true;
    }
  }

  void
  stop()
  {
    std::cout << "application stops. \n";
  }

  void
  setStartZone(const Name& start)
  {
    m_ctr->setStartComponentIndex(start.size());
  }

private:

  void
  onSucceed(const Data& data, const Response& response)
  {
    std::cout << "Dig get following Response (need verification): \n";
    Name name = Name().append(response.getZone()).append(response.getRrLabel());
    if (name == m_dstLabel && m_rrType == response.getRrType()) {
      std::cout << "This is the final response returned by zone=" << response.getZone()
                    << " and NdnsType=" << response.getNdnsType()
                    << ". It contains " << response.getRrs().size() << " RR(s) \n";

      std::string msg;
      size_t i = 0;
      for (const auto& rr : response.getRrs()) {
        try {
          msg = std::string(reinterpret_cast<const char*>(rr.value()), rr.value_size());
          std::cout << "succeed to get the info from RR[" << i << "]"
            "type=" << rr.type() << " content=" << msg << "\n";
        }
        catch (std::exception& e) {
          std::cout << "error to get the info from RR[" << i << "]"
            "type=" << rr.type() << "\n";
        }
        ++i;
      }
    }
    else {
      std::cout << "[* !! *] This is not final response.The target Label: "
                << m_dstLabel << " may not exist \n";
    }

    // if (m_dstFile.empty()) {
    //   ;
    // }
    // else if (m_dstFile == "-") {
    //   output(data, std::cout, true);
    // }
    // else {
    //   NDNS_LOG_INFO("output Data packet to " << m_dstFile << " with BASE64 encoding format");
    //   std::filebuf fb;
    //   fb.open(m_dstFile, std::ios::out);
    //   std::ostream os(&fb);
    //   output(data, os, false);
    // }

    std::cout << response << "\n";

    std::cout << "to verify the response" << "\n";
    m_validator.validate(data,
                         bind(&NdnsDig::onDataValidated, this, _1),
                         bind(&NdnsDig::onDataValidationFailed, this, _1, _2)
                         );
  }


  void
  onFail(uint32_t errCode, const std::string& errMsg)
  {
    std::cout << "fail to get response: errCode=" << errCode << " msg=" << errMsg << "\n";
    m_hasError = true;
    this->stop();
  }

  void
  onDataValidated(const shared_ptr<const Data>& data)
  {
    std::cout << "final data pass verification \n";
    this->stop();
  }

  void
  onDataValidationFailed(const shared_ptr<const Data>& data, const std::string& str)
  {
    std::cout << "final data does not pass verification \n";
    m_hasError = true;
    this->stop();
  }

private:
  Name m_dstLabel;
  name::Component m_rrType;

  Name m_hint;
  Name m_certName;
  time::milliseconds m_interestLifetime;

  Face m_face;

  Validator m_validator;
  bool m_shouldValidateIntermediate;
  std::unique_ptr<QueryController> m_ctr;

  bool m_hasError;
  std::string m_dstFile;
};

} // namespace ndns
} // namespace ndn
