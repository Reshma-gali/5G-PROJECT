#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/lte-module.h"
#include "ns3/mobility-module.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("SimpleHarqSimulation");

void PrintHarqStats (uint32_t numBlks, uint32_t txFailed)
{
  std::cout << "HARQ statistics after " << numBlks << " blocks:" << std::endl;
  std::cout << "HARQ TX failed: " << txFailed << std::endl;
}

int main (int argc, char *argv[])
{
  // Simulation parameters
  uint32_t numTransportBlocks = 10;
  double simTime = 0.1 * numTransportBlocks;

  // Create LTE helper
  Ptr<LteHelper> lteHelper = CreateObject<LteHelper> ();

  // Create nodes for eNB and UE
  NodeContainer enbNodes;
  NodeContainer ueNodes;
  enbNodes.Create (1);
  ueNodes.Create (1);

  // Configure mobility
  MobilityHelper mobility;
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (enbNodes);
  mobility.Install (ueNodes);

  // Install LTE devices
  NetDeviceContainer enbDevs = lteHelper->InstallEnbDevice (enbNodes);
  NetDeviceContainer ueDevs = lteHelper->InstallUeDevice (ueNodes);

  // Attach UE to eNB
  lteHelper->Attach (ueDevs, enbDevs.Get (0));

  // Initialize HARQ simulation variables
  uint32_t harqTxFailed = 0;

  // Simulate HARQ process
  for (uint32_t i = 0; i < numTransportBlocks; ++i)
  {
    // Simulate a simple HARQ process where each transmission has a 20% chance to fail
    bool success = (rand() % 100) >= 20; // 80% chance of success
    if (!success)
    {
      harqTxFailed++;
    }

    // Schedule HARQ process statistics printing
    Simulator::Schedule (Seconds (0.1 * i), &PrintHarqStats, i + 1, harqTxFailed);
  }

  Simulator::Stop (Seconds (simTime + 1));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
