#define KBUILD_MODNAME "foo"
#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

int xdp_prog(struct xdp_md *ctx) {
  bpf_trace_printk("Hello, World!\n");
  return XDP_PASS;
}
