# CN_CA2
computer networking projects Fall 1401 - CA2:    
GOAL: In this project, we want to simulate a wireless network using the 3-ns simulation tool and analyze its various criteria.
___
## INTRO
* ns3.cc:

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
Finally, using ```ApplicationContainer``` and ```OnOffHelper```,  we set source = node(3) or load balancer and send data to other TCP nodes.
Other nodes will recieve packets using ```PacketSinkHelper```.
```cpp
// Create a similar flow from n3 to n1, starting at time 1.3 seconds
   OnOffHelper source ("ns3::TcpSocketFactory",Address(InetSocketAddress (i3i4.GetAddress (1), TCP_port)));

   // Set the amount of data to send in bytes. Zero is unlimited.
   ApplicationContainer sourceApps = source.Install (c.Get (3));
   sourceApps.Start (Seconds (1.3));
   sourceApps.Stop (Seconds (20));


   source.SetAttribute ("Remote",AddressValue(InetSocketAddress (i3i5.GetAddress (1), TCP_port)));

   // Set the amount of data to send in bytes. Zero is unlimited.
   sourceApps = source.Install (c.Get (3));
   sourceApps.Start (Seconds (1.3));
   sourceApps.Stop (Seconds (20));

   source.SetAttribute("Remote",AddressValue(InetSocketAddress (i3i6.GetAddress (1), TCP_port)));

   // Set the amount of data to send in bytes. Zero is unlimited.
   sourceApps = source.Install (c.Get (3));
   sourceApps.Start (Seconds (1.3));
   sourceApps.Stop (Seconds (20));


   // Create a PacketSinkApplication and install it on node 1.
   PacketSinkHelper sink_TCP ("ns3::TcpSocketFactory",
                         Address (InetSocketAddress (Ipv4Address::GetAny (), TCP_port)));
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
```
Using the code below we show the resualt and create output ```.tr``` files.
```cpp
 NS_LOG_INFO ("Run Simulation.");

   std::string fileNameWithNoExtension = "FlowVSThroughput_";
   std::string mainPlotTitle = "Flow vs Throughput";
   std::string graphicsFileName        = fileNameWithNoExtension + ".png";
   std::string plotFileName            = fileNameWithNoExtension + ".plt";
   std::string plotTitle               = mainPlotTitle + ", Error: ";
   std::string dataTitle               = "Throughput";

   // Instantiate the plot and set its title.
   Gnuplot gnuplot (graphicsFileName);
   gnuplot.SetTitle (plotTitle);

   // Make the graphics file, which the plot file will be when it
   // is used with Gnuplot, be a PNG file.
   gnuplot.SetTerminal ("png");

   // Set the labels for each axis.
   gnuplot.SetLegend ("Flow", "Throughput");


   Gnuplot2dDataset dataset;
   dataset.SetTitle (dataTitle);
   dataset.SetStyle (Gnuplot2dDataset::LINES_POINTS);

   // Flow monitor.
   Ptr<FlowMonitor> flowMonitor;
   FlowMonitorHelper flowHelper;
   flowMonitor = flowHelper.InstallAll ();

   
   ThroughputMonitor (&flowHelper, flowMonitor, dataset);


   AsciiTraceHelper ascii;
   p2p.EnableAsciiAll (ascii.CreateFileStream ("out"+file_id+".tr"));
   p2p.EnablePcapAll ("out");

   Simulator::Stop (Seconds (20));
   Simulator::Run ();
   Simulator::Destroy ();

   double e2e_delay = (1000/packet_size_entry)*((10-1.1)+(10-1.2)+(10-1.3)+(20-1.3)+(20-1.3)+(20-1.3));
   std::cout << "..........................................................................." << std::endl;
   std::cout << "e2e_delay = (1/n)*sigma[1, n](stop-start time)*(1000)ms = " << e2e_delay << std::endl;


   double throughput = ((packet_size_entry*8)/((20-1)));
   std::cout << "throughput = recvdSize/(stop-start time)*(8/1000) = " << throughput/1000 << std::endl;
   std::cout << "..........................................................................." << std::endl;

   NS_LOG_INFO ("Done.");
```

* ns3_model.py:

We set  ouput id from [0-4] for error and from [5-7] for bandwidth change.
```python
    throughput_err = [0.0]*5
    throughput_bw = [0]*3

    # error plot elements
    throughput_err[0] = throughput("out0.tr")
    throughput_err[1] = throughput("out1.tr")
    throughput_err[2] = throughput("out2.tr")
    throughput_err[3] = throughput("out3.tr")
    throughput_err[4] = throughput("out4.tr")
    # bandwidth plot elements
    throughput_bw[0] = throughput("out5.tr")
    throughput_bw[1] = throughput("out6.tr")
    throughput_bw[2] = throughput("out7.tr")
```
Then, check for different values of arguments and run ``.cc`` file and plot the output
```python
# xpoints_error = np.array([1, 2, 3, 4, 5])
    xpoints_error = np.array([0.0, 0.001, 0.0001, 0.00001, 0.000001])
    xpoints_bandwidth = np.array([1, 10, 100])

    plt.plot(xpoints_error, throughput_err, color='green', linestyle='dashed', linewidth = 3, marker='o', markerfacecolor='blue', markersize=12)
    plt.xlabel('error rate')
    plt.ylabel('throughput')
    plt.title('vs error')

    # plt.plot(xpoints_bandwidth, throughput_bw, color='green', linestyle='dashed', linewidth = 3, marker='o', markerfacecolor='blue', markersize=12) 
    # plt.xlabel('bandwidth')
    # plt.ylabel('throughput')
    # plt.title('vs bandwidth')
    plt.show()
```
We also used two functions to draw graphs
```python
def throughput(filename):
    trace = open(filename, 'r')
    recvd = 0
    start = 1e6
    stop = 0
    first = 1
    for line in trace:
        words = line.split(' ')
        time = float(words[1])
        pos = words.index("length:")
        packet_size = int(words[pos+1])
        if line.startswith('r'):
            if first:
                start = time
                first = 0
            stop = time
            recvd += packet_size
    throughput_val = recvd / (stop-start)*(8/1000)
    trace.close()
    return throughput_val

def end_to_end_delay(filename):
    trace = open(filename, 'r')
    max_ = 0
    for line in trace:
        words = line.split(' ')
        pos = words.index("id")
        temp_id = int(words[pos+1])
        if temp_id>max_:
            max_ = temp_id
    
    starter = [-1]*(max_+1)
    stoper = [-1]*(max_+1)

    # print("max: ", max_, " size: ", len(starter))
    tracer = open(filename, 'r')
    for line in tracer:
        words = line.split(' ')
        time = float(words[1])
        pos = words.index("id")
        packet_id = int(words[pos+1])
        # print("id: ", packet_id)
        if line.startswith('+') and starter[packet_id] == -1:
            starter[packet_id] = time
        if line.startswith('r'):
            stoper[packet_id] = time
        
    packets_duration = 0
    for i in range(max_):
        start = starter[i]
        stop = stoper[i]
        packets_duration += stop - start
        
    trace.close()
    tracer.close()
    
    return packets_duration/max_
    
    
```
___

## OUTPUT
1.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-13-41.png" >
2.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-19-16.png" >
3.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-19-17.png" >
4.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-19-18.png" >
5.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-19-20.png" >
6.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-19-22.png" >
7.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-19-28.png" >
8.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-19-38.png" >
9.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-21-04.png" >
    
In order to be able to obtain the throughput for different error rates, we run CC for each and produce its outputs, which in the project environment have this status:    

Now we execute the throughput function for all the different trs we obtained, we store the result of each execution that is throughput output in a list of throughput outputs so that we can plot and analyze the output graph for this list.    

Implementation of the throughput algorithm: First, we read the tr output file, which is the CC output and summarizes the network status, and store it in the trace variable. We look at the recvdSize variable to sum up the total number of bytes sent. startTime is a variable that stores the start time of the network operation. At the beginning, it is seen that it has a value of 1e6 to be the maximum value.    
If the network is sending (starting with the character 'r' which means receiving), it passes the condition and sends the packet. So we save the starting time of the whole network.    
  stopTime is a variable that stores the time of the last packet that entered the link and has the minimum input size and was sent to the destination (starting with the character 'r' which means receive).    
We read all the lines of the input tr output file, if this line is a sign of the arrival of a packet (both sending and receiving), then it means that a new packet is transmitted, so we must save the time when this packet is sent (if it was the starting time for the first and We save the last time and if it was the final time of the network, we always update it according to the next lines of tr output).    
According to the tr output structure, the length dimension of the data is the size of the packet. We save the second line data that represents the time. Finally, we have the start and end time of placing the data packet with the input size and we calculate it according to the throughput formula and close the input tr output file and send the calculation result.    

Throughput formula:    
‍`
average throughput = (recvdSize/(stopTime-startTime))*(8/1000)    
recvdSize = Store received packet's size    
stopTime = Simulation stop time    
startTime = Simulation start time    
‍`    
********************* Throughput output *********************    

The plot shown is for the rate of said errors.    
We can see that the smaller the error rate, the greater the throughput.    
    
average end-to-end-delay:     
<img src="./IMG/Image%2010.png" >

First, we open the desired trace file. There is a _max variable whose task is to find the maximum number of transmitted packets, which is implemented in the same way as the previous part of this request. Two arrays of packet transfer start and end times are created, which have a default value of -1, and the length of these two arrays is equal to the number of packets. Then we read all the lines of the file again and while reading the lines, we save the data of each line in words, then we find the value of the sequence number, similar to the previous sections, now according to the sending status (starting with the character 'R' which means receiving) and The first time its time is set (start_time[seq_num-1] == -1) we save the start time of this packet, we always update the final time of receiving the packet.    
We define the variable packet_duration and add the transmission time of all packets in this variable. Finally, this number and the number of _max packets will be the denominator of our average end-to-end delay fraction.
And after division we return the final value.    

10.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-25-45.png" >
11.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-53-58.png" >
12.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-03.png" >
13.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-04.png" >
14.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-06.png" >
15.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-08.png" >
16.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-12.png" >
17.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-14.png" >
18.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-15.png" >
19.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-16.png" >
20.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-18.png" >
21.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-21.png" >
22.
<img src="./IMG/Screenshot%20from%202023-01-13%2013-54-40.png" >
    
If we increase bw, then throughput, average e2e delay will increase too.    
If we increase error rate, then throughput, average e2e delay will decrease.    
