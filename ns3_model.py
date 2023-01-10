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
        pos = words.index("id")
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
    print("throughput is: ", throughput("out.tr"))
    print("average end to end delay is: ", end_to_end_delay("out.tr"))

if __name__ == '__main__':
    main()