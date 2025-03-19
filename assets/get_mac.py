from scapy.all import ARP, send, sr
import argparse
import logging
import uuid
from getmac import get_mac_address as gma

logging.basicConfig(format='[%(asctime)s] %(message)s', level=logging.INFO)

parser = argparse.ArgumentParser(description="Cleanup the network configuration")
parser.add_argument("--target_ip", type=str, help="Target IP address", default="")
args = parser.parse_args()

TARGET_IP = args.target_ip

if "__main__" == __name__:
    if TARGET_IP == "OWN":
        own_mac = gma()
        logging.info(f"MAC address of own device is |{own_mac}|")
        exit(0)
    # send a ping request and return the mac adress of the target
    ans, _ = sr(ARP(op=1, pdst=TARGET_IP), timeout=2, verbose=False)
    for _, rcv in ans:
        logging.info(f"MAC address of {TARGET_IP} is |{rcv.hwsrc}|")
        break
    else:
        logging.info(f"MAC address of {TARGET_IP} was not found ||")
    exit(0)