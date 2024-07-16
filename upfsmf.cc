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
 
class UpfApplication : public Application {
public:
    static TypeId GetTypeId(void) {
        static TypeId tid = TypeId("ns3::UpfApplication")
            .SetParent<Application>()
            .SetGroupName("Applications")
            .AddConstructor<UpfApplication>();
        return tid;
    }
 
    UpfApplication() : m_socket(0) {}
    virtual ~UpfApplication() { m_socket = 0; }
 
protected:
    virtual void StartApplication() {
        NS_LOG_INFO("UPF Application Started");
        if (!m_socket) {
            m_socket = Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));
            m_socket->Bind();
            m_socket->SetRecvCallback(MakeCallback(&UpfApplication::HandleRead, this));
        }
    }
 
    virtual void StopApplication() {
        NS_LOG_INFO("UPF Application Stopped");
        if (m_socket) {
            m_socket->Close();
            m_socket = 0;
        }
    }
 
private:
    void HandleRead(Ptr<Socket> socket) {
        Ptr<Packet> packet;
        Address from;
        while ((packet = socket->RecvFrom(from))) {
            NS_LOG_INFO("UPF received packet from " << InetSocketAddress::ConvertFrom(from).GetIpv4());
            // Example: Echo the packet back
            socket->SendTo(packet, 0, from);
        }
    }
 
    Ptr<Socket> m_socket;
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
 
    SmfApplication() : m_socket(0) {}
    virtual ~SmfApplication() { m_socket = 0; }
 
protected:
    virtual void StartApplication() {
        NS_LOG_INFO("SMF Application Started");
        if (!m_socket) {
            m_socket = Socket::CreateSocket(GetNode(), TypeId::LookupByName("ns3::UdpSocketFactory"));
            InetSocketAddress local = InetSocketAddress(Ipv4Address::GetAny(), 12345);
            m_socket->Bind(local);
            Simulator::Schedule(Seconds(2.0), &SmfApplication::SendToUpf, this);
        }
    }
 
    virtual void StopApplication() {
        NS_LOG_INFO("SMF Application Stopped");
        if (m_socket) {
            m_socket->Close();
            m_socket = 0;
        }
    }
 
private:
    void SendToUpf() {
        NS_LOG_INFO("SMF sending packet to UPF");
        Ptr<Packet> packet = Create<Packet>(100); // 100-byte packet
        InetSocketAddress remote = InetSocketAddress(Ipv4Address("10.1.2.2"), 12345);
        m_socket->SendTo(packet, 0, remote);
    }
 
    Ptr<Socket> m_socket;
};
 
int main(int argc, char *argv[]) {
    LogComponentEnable("Simple5GCore", LOG_LEVEL_INFO);
 
    CommandLine cmd;
    cmd.Parse(argc, argv);
 
    // Create nodes
    NodeContainer nodes;
    nodes.Create(3); // One UE, one gNB, one 5G Core Network (SMF + UPF)
 
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
    Ipv4InterfaceContainer interfaces = address.Assign(devices.Get(0));
 
    address.SetBase("10.1.2.0", "255.255.255.0");
    interfaces.Add(address.Assign(devices.Get(1)));
 
    // Install UPF and SMF applications on Core Network node
    Ptr<UpfApplication> upfApp = CreateObject<UpfApplication>();
    nodes.Get(2)->AddApplication(upfApp);
    upfApp->SetStartTime(Seconds(1.0));
    upfApp->SetStopTime(Seconds(10.0));
 
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
