import numpy as np
import matplotlib.pyplot as plt

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
    
def main():
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

    # print(throughput_err)
    # print(throughput_bw)

    # print("throughput is: ", throughput("out.tr"))
    # print("average end to end delay is: ", end_to_end_delay("out.tr"))

if __name__ == '__main__':
    main()