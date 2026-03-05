#!/usr/bin/env python3
import re, sys

# kernel-network regex (TCP/IP + skb + NAPI + qdisc + mlx5 +
# netfilter/conntrack + neighbor/ARP + backlog + loopback).
NET = re.compile(
    r"(net_rx_action|net_tx_action|__napi_poll|napi_|gro_|gso_|"
    r"tcp_|udp_|ip_|ipv4_|ipv6_|inet_|inet6_|sock_|skb_|"
    r"\bsk_|"                              # word-boundary avoids task_, mask_, etc.
    r"__dev_queue_xmit|dev_queue_xmit|dev_hard_start_xmit|"
    r"netif_|neigh_|"                      # RX dispatch + ARP/neighbor layer
    r"nf_|resolve_normal_ct|"             # netfilter + conntrack lookup
    r"qdisc_|sch_|"
    r"_backlog|kfree_skb|consume_skb|"    # backlog RX path + skb lifecycle
    r"cubictcp_|bbr_|westwood_|"          # TCP congestion-control algorithms
    r"loopback_xmit|"                     # loopback device transmit
    r"mlx5)"
)

total_period = 0
net_period = 0

cur_period = None
cur_is_net = False
in_sample = False

def flush():
    global total_period, net_period, cur_period, cur_is_net, in_sample
    if not in_sample or cur_period is None:
        return
    total_period += cur_period
    if cur_is_net:
        net_period += cur_period

for line in sys.stdin:
    if line.startswith("\t"): # callchain frame line
        if NET.search(line):
            cur_is_net = True
        continue

    line = line.strip()
    if not line:
        continue

    # New sample header line
    flush()
    in_sample = True
    cur_is_net = False
    cur_period = None

    # With -F comm,period,event,ip,sym,dso, the 2nd token is period
    parts = line.split()
    if len(parts) >= 2:
        try:
            cur_period = int(parts[1])
        except ValueError:
            cur_period = None

    # Also allow header-line match (shouldn't be the case, but harmless)
    if NET.search(line):
        cur_is_net = True

flush()

print(f"total_period={total_period}")
print(f"net_period={net_period}")
print(f"net_fraction={(net_period/total_period) if total_period else 0.0:.7f}")