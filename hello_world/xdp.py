from bcc import BPF
import pyroute2
import time
import sys

device = sys.argv[1]
mode = BPF.XDP
flags = 2 # XDP_FLAGS_SKB_MODE

# load BPF program
b = BPF(src_file="./xdp_func.c", cflags=["-w"])
fn = b.load_func("xdp_prog", mode)

b.attach_xdp(device, fn, flags)


print("CTRL+C to stop")
while 1:
    try:
        time.sleep(1)
        continue
    except KeyboardInterrupt:
        print("Removing filter from device")
        b.remove_xdp(device, flags)
        break;

