#define KBUILD_MODNAME "foo"
#include <uapi/linux/bpf.h>
#include <linux/in.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <linux/ipv6.h>

static inline int parse_ipv4(void *data, u64 nh_off, void *data_end) {
    struct iphdr *iph = data + nh_off;
    if ((void*)&iph[1] > data_end)
        return 0;
    return iph->protocol;
}

static inline int parse_ipv6(void *data, u64 nh_off, void *data_end) {
    struct ipv6hdr *ip6h = data + nh_off;
    if ((void*)&ip6h[1] > data_end)
        return 0;
    return ip6h->nexthdr;
}

static inline int get_tcp_dest_port(void *data, u64 nh_off, void *data_end) {
  struct tcphdr *tcph = data + nh_off;
  if (data + nh_off + sizeof(struct tcphdr) > data_end)
    return 0;
  return tcph->dest;
}

static inline int get_tcp_src_port(void *data, u64 nh_off, void *data_end) {
  struct tcphdr *tcph = data + nh_off;
  if (data + nh_off + sizeof(struct tcphdr) > data_end)
    return 0;
  return tcph->source;
}

int xdp_prog(struct xdp_md *ctx) {
    void* data_end = (void*)(long)ctx->data_end;
    void* data = (void*)(long)ctx->data;
    struct ethhdr *eth = data;
    // drop packets
    uint16_t h_proto;
    uint64_t nh_off = 0;
    uint16_t dest_port;
    uint16_t src_port;
    nh_off = sizeof(*eth);
    if (data + nh_off  > data_end){
      return XDP_PASS;
    }
    h_proto = eth->h_proto;

    if (h_proto == htons(ETH_P_IP)){
      h_proto = parse_ipv4(data, nh_off, data_end);
    }else if (h_proto == htons(ETH_P_IPV6)){
      h_proto = parse_ipv6(data, nh_off, data_end);
    }else{
      return XDP_PASS;
    }

    if(h_proto == IPPROTO_TCP){
      dest_port = ntohs(get_tcp_dest_port(data, nh_off, data_end));
       src_port = ntohs(get_tcp_src_port(data, nh_off, data_end));
      if(dest_port == 80 || src_port == 80){
         return XDP_DROP;
      }
    }

    return XDP_DROP;
}
