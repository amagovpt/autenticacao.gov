import os
import sys
import re
import subprocess

# Portuguese CC Application Identifiers (AIDs)
PT_CC_AIDS = {
    "A000000077010800070000": "IAS (eID) Application",
    "A000000077010800070000FE00000100": "IAS (eID) Application (Full)",
    "A0000002471001": "ICAO (Travel) Application",
    "A000000063504B43532D3135": "PKCS#15 Application",
    "A00000024300130000000101": "Biometric (Match-on-Card)",
}

# Key Elementary Files (EFs) for Portuguese CC
PT_CC_FILES = {
    "3F00": "Master File (MF)",
    "5F00": "Dedicated File (DF.eID)",
    "EF02": "EF.ID (Identity Data)",
    "EF03": "EF.Photo (Face Image)",
    "EF04": "EF.Notes (Personal Notes)",
    "EF05": "EF.Addr (Address)",
    "EF06": "EF.SOD (Security Object)",
    "EF0C": "EF.Cert (Certificates)",
    "4401": "EF.AOD (PIN Information)",
}

def decode_pin(payload_hex):
    """Attempts to decode the PIN data from the VERIFY APDU payload."""
    # Common format: ASCII digits padded with FF or 00
    try:
        # Convert hex to bytes
        data = bytes.fromhex(payload_hex)
        # Filter only printable digits/characters
        decoded = "".join([chr(b) for b in data if 32 <= b <= 126])
        return f"HEX:[{payload_hex}] STRING:\"{decoded.strip()}\""
    except:
        return f"HEX:[{payload_hex}]"

def parse_apdu_cmd(data_hex):
    """Parses ISO 7816 APDU commands."""
    if len(data_hex) < 8:
        return f"Raw Data: {data_hex}"
    
    cla = data_hex[0:2].upper()
    ins = data_hex[2:4].upper()
    p1 = data_hex[4:6].upsaper()
    p2 = data_hex[6:8].upper()
    
    ins_map = {
        "A4": "SELECT",
        "B0": "READ BINARY",
        "20": "VERIFY PIN",
        "C0": "GET RESPONSE",
        "B2": "READ RECORD",
        "2A": "PERFORM SECURITY OP",
        "84": "GET CHALLENGE",
        "88": "INTERNAL AUTH",
    }
    
    name = ins_map.get(ins, f"INS_{ins}")
    details = ""

    if ins == "A4": # SELECT
        mode = "File" if p1 == "00" else "Application"
        payload = data_hex[10:].upper()
        if mode == "Application":
            details = f"[{PT_CC_AIDS.get(payload, 'Unknown App')}] AID: {payload}"
        else:
            file_id = payload[0:4]
            details = f"[{PT_CC_FILES.get(file_id, 'File ' + file_id)}]"
            
    elif ins == "B0": # READ BINARY
        offset = int(p1 + p2, 16) & 0x7FFF
        details = f"Offset: {offset}"
        
    elif ins == "20": # VERIFY PIN
        pin_type = {"81":"Auth PIN", "82":"Sign PIN", "83":"Addr PIN"}.get(p2, p2)
        pin_data = decode_pin(data_hex[10:])
        details = f"Verify {pin_type} | CAPTURED PIN: {pin_data}"

    return f"{name:<18} | {details}"

def parse_apdu_resp(inf_hex):
    """Parses Smart Card responses."""
    if len(inf_hex) < 4:
        return f"Data: {inf_hex}"
    
    sw = inf_hex[-4:].upper()
    data = inf_hex[:-4]
    
    status = "SUCCESS" if sw == "9000" else (f"ERROR {sw}" if sw.startswith("6") else f"SW {sw}")
    
    if data:
        preview = data[:32] + ("..." if len(data) > 32 else "")
        return f"RESP {status:<12} | Data: {preview}"
    return f"RESP {status}"

def parse_t1(hex_data, is_resp=False):
    """Decodes T=1 Protocol blocks."""
    if len(hex_data) < 8:
        return f"Short T=1 Payload: {hex_data}"
    
    nad = hex_data[0:2]
    pcb = int(hex_data[2:4], 16)
    length = int(hex_data[4:6], 16)
    inf = hex_data[6:6 + length*2]
    edc = hex_data[6 + length*2:8 + length*2]

    # Decode PCB
    if (pcb & 0x80) == 0: # I-Block
        more = (pcb >> 5) & 1
        chaining = " [Chained]" if more else ""
        if length == 0: return "I-BLOCK ACK"
        
        parsed_inf = parse_apdu_resp(inf) if is_resp else parse_apdu_cmd(inf)
        return f"{parsed_inf}{chaining}"
    
    elif (pcb & 0xC0) == 0x80: # R-Block
        seq = (pcb >> 4) & 1
        return f"R-BLOCK (Next Seq {seq})"
    
    else: # S-Block
        stype = pcb & 0x3F
        return f"S-BLOCK (Type {stype:02x}) | INF: {inf}"

def main():
    try:
        output = subprocess.check_output(["lsusb"], text=True)
        bus, dev = None, None
        for line in output.splitlines():
            if any(kw in line for kw in ["Smart Card", "Realtek", "Reader"]):
                match = re.search(r"Bus (\d+) Device (\d+)", line)
                if match: bus, dev = int(match.group(1)), int(match.group(2))
        
        if not bus:
            print("[!] Reader not found.")
            sys.exit(1)

        path = f"/sys/kernel/debug/usb/usbmon/{bus}u"
        device_pattern = re.compile(f":{bus}:({dev}|{dev:03d}):")
        
        print(f"[*] [SUPER GEMINI] MONITORING PORTUGUESE CITIZEN CARD...")
        print(f"[*] WARNING: PINs will be captured in plaintext if sent over USB.")
        
        with open(path, "r") as f:
            for line in f:
                if device_pattern and not device_pattern.search(line): continue
                if "=" not in line: continue
                
                hex_part = line.split("=")[1].strip()
                clean_hex = "".join(hex_part.split())
                
                if len(clean_hex) < 20: continue
                
                msg_type = clean_hex[0:2].lower()
                is_callback = " C " in line
                
                len_part = clean_hex[2:10]
                data_len = int("".join([len_part[i:i+2] for i in range(0, 8, 2)][::-1]), 16)
                payload = clean_hex[20:20 + data_len*2]
                
                if not payload: continue

                if msg_type == "6f": # Command
                    print(f"\033[93m>>> {parse_t1(payload, is_resp=False)}\033[0m")
                elif msg_type == "80": # Response
                    print(f"\033[94m<<< {parse_t1(payload, is_resp=True)}\033[0m")
                
                sys.stdout.flush()

    except PermissionError: print("[!] Run with 'sudo'.")
    except KeyboardInterrupt: print("\n[*] Stopped.")

if __name__ == "__main__":
    main()
