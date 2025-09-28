#!/usr/bin/env python3
import json
import subprocess
import re

def get_wireless_interfaces():
    try:
        output = subprocess.check_output(['iwconfig'], text=True, stderr=subprocess.DEVNULL)
        # parse all interfaces that have 'IEEE 802.11' (wireless)
        interfaces = []
        for line in output.split('\n'):
            if 'IEEE 802.11' in line:
                iface = line.split()[0]
                interfaces.append(iface)
        return interfaces
    except Exception:
        return []

def parse_iwlist(interface):
    try:
        output = subprocess.check_output(['iwlist', interface, 'scan'], text=True, stderr=subprocess.DEVNULL)
    except Exception as e:
        return []

    networks = []
    cells = output.split("Cell ")
    for cell in cells[1:]:
        mac = re.search(r"Address: ([\w:]+)", cell)
        ssid = re.search(r'ESSID:"([^"]+)"', cell)
        chan = re.search(r"Channel:(\d+)", cell)
        sig = re.search(r"Signal level=(-?\d+) dBm", cell)
        enc = "Open"
        if "Encryption key:on" in cell:
            if "WPA2" in cell:
                enc = "WPA2"
            elif "WPA" in cell:
                enc = "WPA"
            elif "WEP" in cell:
                enc = "WEP"
            else:
                enc = "Unknown"
        networks.append({
            "ssid": ssid.group(1) if ssid else "",
            "bssid": mac.group(1) if mac else "",
            "channel": int(chan.group(1)) if chan else 0,
            "signal": int(sig.group(1)) if sig else 0,
            "security": enc
        })
    return networks

if __name__ == "__main__":
    interfaces = get_wireless_interfaces()
    if not interfaces:
        print(json.dumps([]))
    else:
        # Use first wireless device found
        nets = parse_iwlist(interfaces[0])
        print(json.dumps(nets))
