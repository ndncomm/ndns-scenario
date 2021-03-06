// ndn-simple.cpp

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ndnSIM-module.h"

#include "../extensions/ndns-daemon-helper.hpp"

namespace ns3 {

int
main(int argc, char* argv[])
{
  // setting default parameters for PointToPoint links and channels
  Config::SetDefault("ns3::PointToPointNetDevice::DataRate", StringValue("1Mbps"));
  Config::SetDefault("ns3::PointToPointChannel::Delay", StringValue("10ms"));
  Config::SetDefault("ns3::DropTailQueue::MaxPackets", StringValue("20"));

  // Read optional command-line parameters (e.g., enable visualizer with ./waf --run=<> --visualize
  CommandLine cmd;
  cmd.Parse(argc, argv);

  // Creating nodes
  NodeContainer nodes;
  nodes.Create(3);

  // Connecting nodes using two links
  PointToPointHelper p2p;
  p2p.Install(nodes.Get(0), nodes.Get(1));
  p2p.Install(nodes.Get(1), nodes.Get(2));

  // Install NDN stack on all nodes
  ndn::StackHelper ndnHelper;
  ndnHelper.SetDefaultRoutes(true);
  ndnHelper.InstallAll();

  // Choosing forwarding strategy
  ndn::StrategyChoiceHelper::InstallAll("/prefix", "/localhost/nfd/strategy/multicast");

  // Installing applications

  // Consumer
  ndn::AppHelper consumerHelper("NdnsDigApp");
  // Consumer will request /prefix/0, /prefix/1, ...
  //consumerHelper.SetPrefix("/prefix");
  //consumerHelper.SetAttribute("Frequency", StringValue("10")); // 10 interests a second
  consumerHelper.Install(nodes.Get(0)).Start(Seconds(3));                        // first node

  ::ndn::ndns::ManagementTool tool("test.db", ns3::ndn::StackHelper::getKeyChain());
  ::ndn::ndns::DaemonHelper daemonHelper("test.db", tool);

  string rootZone = "/";
  string rootParent = "/";
  string rootKsk = "/a";
  string rootDsk = "/b";
  daemonHelper.createZone(rootZone, rootParent, 100, 200, rootKsk, rootDsk);

  // string zone = "/ndn";
  // string parent = "/";
  // string ksk = "/ndn/a";
  // string dsk = "/ndn/b";
  // daemonHelper.createZone(zone, parent, 100, 200, ksk, dsk);

  // Producer
  ndn::AppHelper producerHelper("NdnsServerApp");
  // Producer will reply to all requests starting with /prefix
  //producerHelper.SetPrefix("/prefix");
  producerHelper.SetAttribute("ConfigFile", StringValue("ndns.conf.sample"));
  producerHelper.Install(nodes.Get(2)).Start(Seconds(2)); // last node

  
  Simulator::Stop(Seconds(20.0));

  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

} // namespace ns3

int
main(int argc, char* argv[])
{
  return ns3::main(argc, argv);
}
