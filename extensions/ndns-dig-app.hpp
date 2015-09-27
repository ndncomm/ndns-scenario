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

#include "ns3/ndnSIM-module.h"
#include "ns3/nstime.h"
#include "ns3/string.h"
#include "ns3/boolean.h"

#include "ndns-dig.hpp"

namespace ns3 {
namespace ndn {

class NdnsDigApp : public Application{
public:
  static TypeId
  GetTypeId()
  {
    static TypeId tid = TypeId("NdnsDigApp")
      .SetParent<Application>()
      .AddConstructor<NdnsDigApp>()
      .AddAttribute("Timeout", "Query Timeout",
                    TimeValue(Seconds(4)),
                    MakeTimeAccessor(&NdnsDigApp::m_timeout),
                    MakeTimeChecker())
      .AddAttribute("Lifetime", "Query Interest lifetime (milliseconds)",
                    StringValue("2s"),
                    MakeTimeAccessor(&NdnsDigApp::m_interestLifeTime),
                    MakeTimeChecker())
      .AddAttribute("RrType", "Request RR type",
                    StringValue("TXT"),
                    MakeStringAccessor(&NdnsDigApp::m_rrType),
                    MakeStringChecker())
      .AddAttribute("Start", "Set the first zone to query",
                    StringValue("/ndn"),
                    MakeNameAccessor(&NdnsDigApp::m_start),
                    MakeNameChecker())
      .AddAttribute("Validate", "Flag to validate intermediate results",
                    BooleanValue(false),
                    MakeBooleanAccessor(&NdnsDigApp::m_validate),
                    MakeBooleanChecker())
      .AddAttribute("Name", "Name to be resolved",
                    StringValue("/ndn"),
                    MakeNameAccessor(&NdnsDigApp::m_name),
                    MakeNameChecker())
      .AddAttribute("Hint", "Name hint",
                    StringValue("/ndn"),
                    MakeNameAccessor(&NdnsDigApp::m_hint),
                    MakeNameChecker())
      ;

    return tid;
  }

protected:

  virtual void
  StartApplication()
  {
    const time::milliseconds interestLifeTime(m_interestLifeTime.GetMilliSeconds());
    const Name::Component rrType(m_rrType);

    m_instance.reset(new ::ndn::ndns::NdnsDig());

    m_instance->setDstLabel(m_name);
    m_instance->setInterestLifetime(interestLifeTime);
    m_instance->setRrType(rrType);
    m_instance->setHint(m_hint);
    m_instance->setShouldValidateIntermediate(m_validate);

    m_instance->run();
  }

  virtual void
  StopApplication()
  {
    m_instance.reset();
  }

protected:
  Time m_timeout;
  string m_rrType;
  Name m_start;
  bool m_validate;
  Name m_name;
  Name m_hint;
  Time m_interestLifeTime;

private:
  unique_ptr<::ndn::ndns::NdnsDig> m_instance;
};

} // namespace ndn
} // namespace ns3


// int
// main(int argc, char* argv[])
// {
//   using std::string;
//   using namespace ndn;

//   Name dstLabel;
//   int ttl = 4;
//   string rrType = "TXT";
//   string dstFile;
//   bool shouldValidateIntermediate = true;
//   Name start("/ndn");

//   try {
//     namespace po = boost::program_options;
//     po::variables_map vm;

//     po::options_description generic("Generic Options");
//     generic.add_options()("help,h", "print help message");

//     po::options_description config("Configuration");
//     config.add_options()
//       ("timeout,T", po::value<int>(&ttl), "query timeout. default: 4 sec")
//       ("rrtype,t", po::value<std::string>(&rrType), "set request RR Type. default: TXT")
//       ("dstFile,d", po::value<std::string>(&dstFile), "set output file of the received Data. "
//        "if omitted, not print; if set to be -, print to stdout; else print to file")
//       ("start,s", po::value<Name>(&start)->default_value("/ndn"), "set first zone to query")
//       ("not-validate,n", "trigger not validate intermediate results")
//       ;

//     po::options_description hidden("Hidden Options");
//     hidden.add_options()
//       ("name", po::value<Name>(&dstLabel), "name to be resolved")
//       ;
//     po::positional_options_description postion;
//     postion.add("name", 1);

//     po::options_description cmdline_options;
//     cmdline_options.add(generic).add(config).add(hidden);

//     po::options_description config_file_options;
//     config_file_options.add(config).add(hidden);

//     po::options_description visible("Usage: ndns-dig /name/to/be/resolved [-t rrType] [-T ttl]"
//                                     "[-d dstFile] [-s startZone] [-n]\n"
//                                     "Allowed options");

//     visible.add(generic).add(config);

//     po::parsed_options parsed =
//       po::command_line_parser(argc, argv).options(cmdline_options).positional(postion).run();

//     po::store(parsed, vm);
//     po::notify(vm);

//     if (vm.count("help")) {
//       std::cout << visible << std::endl;
//       return 0;
//     }

//     if (!vm.count("name")) {
//       std::cerr << "must contain a target label parameter." << std::endl;
//       std::cerr << visible << std::endl;
//       return 1;
//     }

//     if (!start.isPrefixOf(dstLabel)) {
//       std::cerr << "Error: start zone " << start << " is not prefix of the target label "
//                 << dstLabel << std::endl;
//       return 1;
//     }

//     if (vm.count("not-validate")) {
//       shouldValidateIntermediate = false;
//     }

//     if (ttl < 0) {
//       std::cerr << "Error: ttl parameter cannot be negative" << std::endl;
//       return 1;
//     }
//   }
//   catch (const std::exception& ex) {
//     std::cerr << "Parameter Error: " << ex.what() << std::endl;
//     return 1;
//   }

//   try {
//     ndn::ndns::NdnsDig dig("", dstLabel, ndn::name::Component(rrType), shouldValidateIntermediate);
//     dig.setInterestLifetime(ndn::time::seconds(ttl));
//     dig.setDstFile(dstFile);

//     // Due to ndn testbed does not contain the root zone
//     // dig here starts from the TLD (Top-level Domain)
//     // precondition is that TLD : 1) only contains one component in its name; 2) its name is routable
//     dig.setStartZone(start);

//     dig.run();

//     if (dig.hasError())
//       return 1;
//     else
//       return 0;
//   }
//   catch (const ndn::ValidatorConfig::Error& e) {
//     std::cerr << "Fail to create the validator: " << e.what() << std::endl;
//     return 1;
//   }
//   catch (const std::exception& e) {
//     std::cerr << "Error: " << e.what() << std::endl;
//     return 1;
//   }

// }
