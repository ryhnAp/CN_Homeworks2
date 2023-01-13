# CN_CA2
computer networking projects Fall 1401 - CA2:    
GOAL: In this project, we want to simulate a wireless network using the 3-ns simulation tool and analyze its various criteria.
___
## INTRO
In the first section of the code, we set the default value for variables we want to use in the code, plus the cmd.AddValue command which gets values from the user.
The change of values(band width and error rate), will affect Throughput and Average end-to-end delay.

Throughput: It is equal to the ratio of the total packets received by the receiving nodes to the time interval between receiving the first packet and the last packet.

Average end-to-end delay: It is equal to the average arrival times of packages from origin to destination.
```cpp
// default value for cmd line 
	double error_rate_entry = 0.0;
	double bandwidth_entry = 1;
	int udp_node_count = 3;
	int tcp_node_count = 3;
	int packet_size_entry = 210;
	std::string data_rate_entry = "448kb/s";
	std::string delay_entry = "2ms";
   std::string file_id = "";

	// get values from cmd 
	ns3::CommandLine cmd;
	cmd.AddValue("er", "error rate value", error_rate_entry);
	cmd.AddValue("bw", "bandwidth value", bandwidth_entry);
	cmd.AddValue("uc", "udp node count value", udp_node_count);
	cmd.AddValue("tc", "tcp node count value", tcp_node_count);
	cmd.AddValue("ps", "packet size value", packet_size_entry);
	cmd.AddValue("dr", "data rate value", data_rate_entry);
	cmd.AddValue("delay", "p2p delay value", delay_entry);
	cmd.AddValue("o", "file output name id", file_id);

	std::cout << "er" << "error rate value" << error_rate_entry << std::endl;
	std::cout << "bw" << "bandwidth value" << bandwidth_entry << std::endl;
	std::cout << "uc" << "udp node count value" << udp_node_count << std::endl;
	std::cout << "tc" << "tcp node count value" << tcp_node_count << std::endl;
	std::cout << "ps" << "packet size value" << packet_size_entry << std::endl;
	std::cout << "dr" << "data rate value" << data_rate_entry << std::endl;
	std::cout << "delay" << "p2p delay value" << delay_entry << std::endl;
	std::cout << "o" << "file output name id" << file_id << std::endl;

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
```

Then we create nodes, in three groups of TCP nodes and UDP nodes, and a load balancer node, using ```NodeContainer ```. UDP nodes use best effort to send packets to load balancer and load balancer will send packets using ```rand()``` to TCP nodes and receive ```ACK```.
Direction of connctions would be identified using ```NodeContainer(source,dest)```.
```cpp
  NS_LOG_INFO ("Create nodes.");
   NodeContainer c;
   c.Create (udp_node_count + tcp_node_count + 1);
   NodeContainer n0n3 = NodeContainer (c.Get (0), c.Get (3));
   NodeContainer n1n3 = NodeContainer (c.Get (1), c.Get (3));
   NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));

   NodeContainer n3n4 = NodeContainer (c.Get (3), c.Get (4));
   NodeContainer n3n5 = NodeContainer (c.Get (3), c.Get (5));
   NodeContainer n3n6 = NodeContainer (c.Get (3), c.Get (6));
```
Then we implement wireless on all the nodes, and set up channel between nodes(wireless links between nodes).
```cpp
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

```
After that we assign IP to each channel and we use ```IPv4``` in this implementation. plus, for load balancer we make arror model to see lost packets here on load balancer.
```cpp
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
```
Then, Create the OnOff application in two parts one part from UDP nodes to the load balancer and apart from the load balancer to TCP nodes.
Using ```OnOffHelper``` and a specific port we send UDP packets to load balancer. ```sink()``` will help to recieve packets.
```cpp
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
```
The second part of the application as mentioned above is TCP nodes. Here, we use  ```rand()``` to find a random number and between the id range between [0-2], we assign packets to nodes number from [4-6]. Here using three vectors:
```   
   std::list<uint32_t> rand4;
   std::list<uint32_t> rand5;
   std::list<uint32_t> rand6;
 ```
 we set value of packets should be lost in the recievers. for example in the below code and case 1, we set node 5 receive packet and two other nodes (4 & 6), should lost the packet.
```cpp
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
   
```
In the next few lines, we set error receiver in load balancer. which means, nodes 3 (load balancer) will send packet with ```lost packets error model``` to the receivers.

```cpp
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
```
___

## OUTPUT
