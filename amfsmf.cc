#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-routing-table-entry.h"
#include "ns3/log.h"
 
using namespace ns3;
 
NS_LOG_COMPONENT_DEFINE("Simple5GCore");
 
class AmfApplication : public Application {
public:
    static TypeId GetTypeId(void) {
        static TypeId tid = TypeId("ns3::AmfApplication")
            .SetParent<Application>()
            .SetGroupName("Applications")
            .AddConstructor<AmfApplication>();
        return tid;
    }
 
    AmfApplication() {}
    virtual ~AmfApplication() {}
 
protected:
    virtual void StartApplication() {
        NS_LOG_INFO("AMF Application Started");
        // Example: Handle registration request (not implemented)
    }
 
    virtual void StopApplication() {
        NS_LOG_INFO("AMF Application Stopped");
    }
};
 
class SmfApplication : public Application {
public:
    static TypeId GetTypeId(void) {
        static TypeId tid = TypeId("ns3::SmfApplication")
            .SetParent<Application>()
            .SetGroupName("Applications")
            .AddConstructor<SmfApplication>();
        return tid;
    }
 
    SmfApplication() {}
    virtual ~SmfApplication() {}
 
protected:
    virtual void StartApplication() {
        NS_LOG_INFO("SMF Application Started");
        // Example: Handle session management (not implemented)
    }
 
    virtual void StopApplication() {
        NS_LOG_INFO("SMF Application Stopped");
    }
};
 
int main(int argc, char *argv[]) {
    LogComponentEnable("Simple5GCore", LOG_LEVEL_INFO);
 
    CommandLine cmd;
    cmd.Parse(argc, argv);
 
    // Create nodes
    NodeContainer nodes;
    nodes.Create(3); // One UE, one gNB, one 5G Core Network (AMF + SMF)
 
    // Create point-to-point links
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute("DataRate", StringValue("10Mbps"));
    pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));
 
    NetDeviceContainer devices;
    devices = pointToPoint.Install(nodes.Get(0), nodes.Get(1)); // UE to gNB
    devices.Add(pointToPoint.Install(nodes.Get(1), nodes.Get(2))); // gNB to Core Network
 
    // Install internet stack
    InternetStackHelper stack;
    stack.Install(nodes);
 
    // Assign IP addresses
    Ipv4AddressHelper address;
    address.SetBase("10.1.1.0", "255.255.255.0");
    Ipv4InterfaceContainer interfaces = address.Assign(devices);
 
    // Install AMF and SMF applications on Core Network node
    Ptr<AmfApplication> amfApp = CreateObject<AmfApplication>();
    nodes.Get(2)->AddApplication(amfApp);
    amfApp->SetStartTime(Seconds(1.0));
    amfApp->SetStopTime(Seconds(10.0));
 
    Ptr<SmfApplication> smfApp = CreateObject<SmfApplication>();
    nodes.Get(2)->AddApplication(smfApp);
    smfApp->SetStartTime(Seconds(1.0));
    smfApp->SetStopTime(Seconds(10.0));
 
    // Set up routing
    Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> ueStaticRouting = ipv4RoutingHelper.GetStaticRouting(nodes.Get(0)->GetObject<Ipv4>());
    ueStaticRouting->AddNetworkRouteTo(Ipv4Address("10.1.2.0"), Ipv4Mask("255.255.255.0"), Ipv4Address("10.1.1.2"), 1);
 
    Ptr<Ipv4StaticRouting> gnbStaticRouting = ipv4RoutingHelper.GetStaticRouting(nodes.Get(1)->GetObject<Ipv4>());
    gnbStaticRouting->AddNetworkRouteTo(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"), Ipv4Address("10.1.2.1"), 2);
 
    Ptr<Ipv4StaticRouting> coreStaticRouting = ipv4RoutingHelper.GetStaticRouting(nodes.Get(2)->GetObject<Ipv4>());
    coreStaticRouting->AddNetworkRouteTo(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"), Ipv4Address("10.1.2.2"), 1);
 
    Simulator::Run();
    Simulator::Destroy();
 
    return 0;
}
