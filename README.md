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
Then we implement wireless on all the nodes.
```cpp
   InternetStackHelper internet;
   internet.Install (c);
```
پپ
___
## OUTPUT
