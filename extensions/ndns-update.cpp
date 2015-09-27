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



#include <boost/program_options.hpp>
#include "ndns-update.hpp"

int
main(int argc, char* argv[])
{
  // ndn::ndns::log::init();
  // using std::string;
  // using namespace ndn;
  // using namespace ndn::ndns;

  // try {
  //   namespace po = boost::program_options;
  //   po::variables_map vm;

  //   po::options_description generic("Generic Options");
  //   generic.add_options()("help,h", "print help message");

  //   po::options_description config("Configuration");
  //   // config.add_options()
  //   //   ("hint,H", po::value<Name>(&hint), "forwarding hint")
  //   //   ("ttl,T", po::value<int>(&ttl), "TTL of query. default: 4 sec")
  //   //   ("rrtype,t", po::value<string>(&rrType), "set request RR Type. default: TXT")
  //   //   ("ndnsType,n", po::value<string>(&ndnsTypeStr), "Set the ndnsType of the resource record. "
  //   //    "Potential values are [resp|nack|auth|raw]. Default: resp")
  //   //   ("cert,c", po::value<Name>(&certName), "set the name of certificate to sign the update")
  //   //   ("content,o", po::value<std::vector<string>>(&contents)->multitoken(),
  //   //    "set the content of the RR")
  //   //   ("contentFile,f", po::value<string>(&contentFile), "set the path of file which contain"
  //   //    " Response packet in base64 format")
  //   //   ;

  //   po::options_description hidden("Hidden Options");
  //   // hidden.add_options()
  //   //   ("zone,z", po::value<Name>(&zone), "zone the record is delegated")
  //   //   ("rrlabel,l", po::value<Name>(&rrLabel), "set request RR Label")
  //   //   ;
  //   po::positional_options_description postion;
  //   postion.add("zone", 1);
  //   postion.add("rrlabel", 1);

  //   po::options_description cmdline_options;
  //   cmdline_options.add(generic).add(config).add(hidden);

  //   po::options_description config_file_options;
  //   config_file_options.add(config).add(hidden);

  //   po::options_description visible("Usage: ndns-update zone rrLabel [-t rrType] [-T TTL] "
  //                                   "[-H hint] [-n NdnsType] [-c cert] "
  //                                   "[-f contentFile]|[-o content]\n"
  //                                   "Allowed options");

  //   visible.add(generic).add(config);

  //   po::parsed_options parsed =
  //     po::command_line_parser(argc, argv).options(cmdline_options).positional(postion).run();

  //   po::store(parsed, vm);
  //   po::notify(vm);

  //   if (vm.count("help")) {
  //     std::cout << visible << std::endl;
  //     return 0;
  //   }


  //   if (vm.count("content") && vm.count("contentFile")) {
  //     std::cerr << "both -o content and -f contentFile are set. Only one is allowed" << std::endl;
  //     return 1;
  //   }

  //   if (!vm.count("contentFile")) {
  //     NDNS_LOG_TRACE("content option is set. try to figure out the certificate");
  //     if (!vm.count("zone") || !vm.count("rrlabel")) {
  //       std::cerr << "-o option must be set together with -z zone and -r rrLabel" << std::endl;
  //       return 1;
  //     }
  //   }
  //   else {

  //       if (vm.count("rrlabel") && rrLabel != re.getRrLabel()) {
  //         std::cerr << "The loaded Data packet is supposed to have rrLabel: " << re.getRrLabel()
  //                   << " instead of label: " << rrLabel << std::endl;
  //         return 1;
  //       }

  //       if (vm.count("rrtype") && name::Component(rrType) != re.getRrType()) {
  //         std::cerr << "The loaded Data packet is supposed to have rrType: " << re.getRrType()
  //                   << " instead of label: " << rrType << std::endl;
  //         return 1;
  //       }
  //     }
  //     catch (const std::exception& e) {
  //       std::cerr << "Error: the loaded Data packet cannot parse to a Response stored at zone: "
  //                 << zone << std::endl;
  //       return 1;
  //     }
  // }
  // catch (const std::exception& ex) {
  //   std::cerr << "Parameter Error: " << ex.what() << std::endl;
  //   return 1;
  // }


}
