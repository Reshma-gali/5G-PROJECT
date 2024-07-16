#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RandomAccessSimulation");

void SendMsg1 (Ptr<LteEnbNetDevice> enbDevice)
{
  NS_LOG_INFO ("Sending MSG1 (RACH Preamble)");
  Simulator::Schedule (Seconds (0.1), &SendMsg1, enbDevice);
}

void ReceiveMsg2 (Ptr<LteEnbNetDevice> enbDevice)
{
  NS_LOG_INFO ("Receiving MSG2 (RAR)");
  Simulator::Schedule (Seconds (0.2), &ReceiveMsg2, enbDevice);
}

int main (int argc, char *argv[])
{
  LogComponentEnable ("RandomAccessSimulation", LOG_LEVEL_INFO);

  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  Ptr<Node> enbNode = CreateObject<Node> ();
  Ptr<Node> ueNode = CreateObject<Node> ();

  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Add (enbNode);
  ueNodes.Add (ueNode);

  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);
  mobility.Install (ueNodes);

  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  Ptr<LteEnbNetDevice> enbDevice = enbDevs.Get (0)->GetObject<LteEnbNetDevice> ();

  // Schedule MSG1 and MSG2 processes
  Simulator::Schedule (Seconds (0.1), &SendMsg1, enbDevice);
  Simulator::Schedule (Seconds (0.2), &ReceiveMsg2, enbDevice);

  Simulator::Stop (Seconds (1.0));
  Simulator::Run ();
  Simulator::Destroy ();
  return 0;
}
