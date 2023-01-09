/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// Network topology
//
//  n0
//     \ 5 Mb/s, 2ms
//      \          1.5Mb/s, 10ms
//       n2 -------------------------n3
//      /
//     / 5 Mb/s, 2ms
//   n1
//
// - all links are point-to-point links with indicated one-way BW/delay
// - CBR/UDP flows from n0 to n3, and from n3 to n1
// - FTP/TCP flow from n0 to n3, starting at time 1.2 to time 1.35 sec.
// - UDP packet size of 210 bytes, with per-packet interval 0.00375 sec.
//   (i.e., DataRate of 448,000 bps)
// - DropTail queues 
// - Tracing of queues and packet receptions to file 
//   "simple-error-model.tr"

#include <fstream>
#include <vector>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-global-routing-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE  ("SimpleErrorModelExample");

std::vector<int> random_packet_id(int len)
{
   int random_num = rand() % 3;
   std::vector<int> res(len);

   while (true)
   {
      if (len == 0)
         break;

      switch (random_num)
      {
      case 0:
         res[len] = 0;
         len --;
         break;
      case 1:
         res[len] = 1;
         len --;
         break;
      case 2:
         res[len] = 2;
         len --;
         break;
      }

      random_num = rand() % 3;
   }
   return res;   
}

int main (int argc, char *argv[])
{
   // Users may find it convenient to turn on explicit debugging
   // for selected modules; the below lines suggest how to do this
#if 0 
   LogComponentEnable ("SimplePointToPointExample", LOG_LEVEL_INFO);
#endif

	// default value for cmd line 
	double error_rate_entry = 0.0;
	double bandwidth_entry = 1;
	int udp_node_count = 3;
	int tcp_node_count = 3;
	int packet_size_entry = 210;
	std::string data_rate_entry = "448kb/s";
	std::string delay_entry = "2ms";

	// get values from cmd 
	ns3::CommandLine cmd;
	cmd.AddValue("er", "error rate value", error_rate_entry);
	cmd.AddValue("bw", "bandwidth value", bandwidth_entry);
	cmd.AddValue("uc", "udp node count value", udp_node_count);
	cmd.AddValue("tc", "tcp node count value", tcp_node_count);
	cmd.AddValue("ps", "packet size value", packet_size_entry);
	cmd.AddValue("dr", "data rate value", data_rate_entry);
	cmd.AddValue("delay", "p2p delay value", delay_entry);


   // Set a few attributes
   Config::SetDefault ("ns3::RateErrorModel::ErrorRate", DoubleValue (error_rate_entry));
   Config::SetDefault ("ns3::RateErrorModel::ErrorUnit", StringValue ("ERROR_UNIT_PACKET"));

   Config::SetDefault ("ns3::BurstErrorModel::ErrorRate", DoubleValue (error_rate_entry*10));
   Config::SetDefault ("ns3::BurstErrorModel::BurstSize", StringValue ("ns3::UniformRandomVariable[Min=1|Max=3]"));

   Config::SetDefault ("ns3::OnOffApplication::PacketSize", UintegerValue (packet_size_entry));
   Config::SetDefault ("ns3::OnOffApplication::DataRate", DataRateValue (DataRate (data_rate_entry)));

   std::string errorModelType = "ns3::RateErrorModel";

   // Allow the user to override any of the defaults and the above
   // Bind()s at run-time, via command-line arguments
   cmd.AddValue("errorModelType", "TypeId of the error model to use", errorModelType);
   cmd.Parse (argc, argv);

   // Here, we will explicitly create four nodes.  In more sophisticated
   // topologies, we could configure a node factory.
   NS_LOG_INFO ("Create nodes.");
   NodeContainer c;
   c.Create (udp_node_count + tcp_node_count + 1);
   NodeContainer n0n3 = NodeContainer (c.Get (0), c.Get (3));
   NodeContainer n1n3 = NodeContainer (c.Get (1), c.Get (3));
   NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));

   NodeContainer n3n4 = NodeContainer (c.Get (3), c.Get (4));
   NodeContainer n3n5 = NodeContainer (c.Get (3), c.Get (5));
   NodeContainer n3n6 = NodeContainer (c.Get (3), c.Get (6));

   InternetStackHelper internet;
   internet.Install (c);

   // We create the channels first without any IP addressing information
   NS_LOG_INFO ("Create channels.");
   PointToPointHelper p2p;
   p2p.SetDeviceAttribute ("DataRate", DataRateValue (data_rate_entry));
   p2p.SetChannelAttribute ("Delay", StringValue (delay_entry));

   NetDeviceContainer d0d3 = p2p.Install (n0n3);
   NetDeviceContainer d1d3 = p2p.Install (n1n3);
   NetDeviceContainer d2d3 = p2p.Install (n2n3);

   NetDeviceContainer d3d4 = p2p.Install (n3n4);
   NetDeviceContainer d3d5 = p2p.Install (n3n5);
   NetDeviceContainer d3d6 = p2p.Install (n3n6);

   // Create error model on receiver.
   Ptr<RateErrorModel> em = CreateObject<RateErrorModel> ();
   em->SetAttribute ("ErrorRate", DoubleValue (error_rate_entry));
   d2d3.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (em));

   // Later, we add IP addresses.
   NS_LOG_INFO ("Assign IP Addresses.");
   Ipv4AddressHelper ipv4;
   ipv4.SetBase ("10.1.1.0", "255.255.255.0");
   Ipv4InterfaceContainer i0i3 = ipv4.Assign (d0d3);

   ipv4.SetBase ("10.1.2.0", "255.255.255.0");
   Ipv4InterfaceContainer i1i3 = ipv4.Assign (d1d3);

   ipv4.SetBase ("10.1.3.0", "255.255.255.0");
   Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

   ipv4.SetBase ("10.1.4.0", "255.255.255.0");
   Ipv4InterfaceContainer i3i4 = ipv4.Assign (d3d4);

   ipv4.SetBase ("10.1.5.0", "255.255.255.0");
   Ipv4InterfaceContainer i3i5 = ipv4.Assign (d3d5);

   ipv4.SetBase ("10.1.6.0", "255.255.255.0");
   Ipv4InterfaceContainer i3i6 = ipv4.Assign (d3d6);


   NS_LOG_INFO ("Use global routing.");
   Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

   // Create the OnOff application to send UDP datagrams of size
   // 210 bytes at a rate of 448 Kb/s
   NS_LOG_INFO ("Create Applications.");
   uint16_t port = 9;   // Discard port (RFC 863)

   OnOffHelper onoff ("ns3::UdpSocketFactory",
                      Address (InetSocketAddress (i0i3.GetAddress (1), port)));
   onoff.SetConstantRate (DataRate (data_rate_entry));
   ApplicationContainer apps = onoff.Install (c.Get (0));
   apps.Start (Seconds (1.0));
   apps.Stop (Seconds (10.0));

   // Create an optional packet sink to receive these packets
   PacketSinkHelper sink ("ns3::UdpSocketFactory",
                          Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
   // apps = sink.Install (c.Get (3));
   // apps.Start (Seconds (1.0));
   // apps.Stop (Seconds (10.0));

   // Create a similar flow from n3 to n1, starting at time 1.1 seconds
   onoff.SetAttribute ("Remote", 
                       AddressValue (InetSocketAddress (i1i3.GetAddress (1), port)));
   apps = onoff.Install (c.Get (1));
   apps.Start (Seconds (1.1));
   apps.Stop (Seconds (10.0));

   // Create a packet sink to receive these packets
   // sink.SetAttribute ("Local", 
   //                    AddressValue (InetSocketAddress (Ipv4Address::GetAny (), port)));
   // apps = sink.Install (c.Get (3));
   // apps.Start (Seconds (1.1));
   // apps.Stop (Seconds (10.0));

   // Create a similar flow from n3 to n1, starting at time 1.1 seconds
   onoff.SetAttribute ("Remote", 
                       AddressValue (InetSocketAddress (i2i3.GetAddress (1), port)));
   apps = onoff.Install (c.Get (2));
   apps.Start (Seconds (1.2));
   apps.Stop (Seconds (10.0));

   // Create a packet sink to receive these packets
   sink.SetAttribute ("Local", 
                      AddressValue (InetSocketAddress (Ipv4Address::GetAny (), port)));
   apps = sink.Install (c.Get (3));
   apps.Start (Seconds (1.2));
   apps.Stop (Seconds (10.0));

   //TCP
   //
   // Error model
   //
   // Create an ErrorModel based on the implementation (constructor)
   // specified by the default TypeId
   uint16_t TCP_port = 10;   // Discard port (RFC 863)

   //randomize 
   // std::vector<int> rand = random_packet_id(packet_size_entry);
   int random_num = rand() % 3;
   int len = packet_size_entry;
   int res[len] = {0};

   while (true)
   {
      if (len == 0)
         break;

      switch (random_num)
      {
      case 0:
         res[len] = 0;
         len --;
         break;
      case 1:
         res[len] = 1;
         len --;
         break;
      case 2:
         res[len] = 2;
         len --;
         break;
      }

      random_num = rand() % 3;
   }
   std::list<uint32_t> rand4;
   std::list<uint32_t> rand5;
   std::list<uint32_t> rand6;

   for (int i = 0; i < packet_size_entry; i++)
   {
      switch (res[i])
      {
      case 0:
         rand5.push_back(i);
         rand6.push_back(i);
         break;
      case 1:
         rand4.push_back(i);
         rand6.push_back(i);
         break;
      case 2:
         rand4.push_back(i);
         rand5.push_back(i);
         break;
      }
      // std::cout << "i, rand[i] " << i << " " << rand[i] << std::endl;   
   }
   
   ObjectFactory factory;
   factory.SetTypeId (errorModelType);
   Ptr<ErrorModel> em_ = factory.Create<ErrorModel> ();
   d3d4.Get (0)->SetAttribute ("ReceiveErrorModel", PointerValue (em_));

   // Now, let's use the ListErrorModel and explicitly force a loss
   // of the packets with pkt-uids = 11 and 17 on node 2, device 0
   // This time, we'll explicitly create the error model we want
   Ptr<ListErrorModel> pem4 = CreateObject<ListErrorModel> ();
   pem4->SetList (rand4);
   d3d4.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (pem4));
   Ptr<ListErrorModel> pem5 = CreateObject<ListErrorModel> ();
   pem5->SetList (rand5);
   d3d5.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (pem5));
   Ptr<ListErrorModel> pem6 = CreateObject<ListErrorModel> ();
   pem6->SetList (rand6);
   d3d6.Get (1)->SetAttribute ("ReceiveErrorModel", PointerValue (pem6));

   // Create a similar flow from n3 to n1, starting at time 1.3 seconds
   BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (i2i3.GetAddress (1), TCP_port));

   // Set the amount of data to send in bytes. Zero is unlimited.
   ApplicationContainer sourceApps = source.Install (c.Get (3));
   sourceApps.Start (Seconds (1.3));
   sourceApps.Stop (Seconds (20));

   // Create a PacketSinkApplication and install it on node 1.
   PacketSinkHelper sink_TCP ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), TCP_port));
   ApplicationContainer sinkApps = sink_TCP.Install (c.Get (4));
   sinkApps.Start (Seconds (1.3));
   sinkApps.Stop (Seconds (20));

   sink_TCP.SetAttribute ("Local", 
                      AddressValue (InetSocketAddress (Ipv4Address::GetAny (), TCP_port)));
   sinkApps = sink_TCP.Install (c.Get (5));
   sinkApps.Start (Seconds (1.3));
   sinkApps.Stop (Seconds (20));

   sink_TCP.SetAttribute ("Local", 
                      AddressValue (InetSocketAddress (Ipv4Address::GetAny (), TCP_port)));
   sinkApps = sink_TCP.Install (c.Get (6));
   sinkApps.Start (Seconds (1.3));
   sinkApps.Stop (Seconds (20));

   // std::cout << "here " << std::endl;

   AsciiTraceHelper ascii;
   p2p.EnableAsciiAll (ascii.CreateFileStream ("ns3-model.tr"));
   p2p.EnablePcapAll ("ns3-model");

   NS_LOG_INFO ("Run Simulation.");
   Simulator::Run ();
   Simulator::Destroy ();
   NS_LOG_INFO ("Done.");

   return 0;
}