import subprocess
import json
import sys
import os


missing_certs_msg = "Before calling this script, put the chain.pem (CA) and the cert.pem (server) "\
    "files inside the certs/ directory"

openssl_needed_msg = "You need to have openssl installed"

config_create_msg = "You will be prompted to insert some data to create a configuration file"


def generate_certs_h():
    first = True
    certs_in = ""
    certs_data = ""
    begin = "BEGIN CERTIFICATE"
    end = "END CERTIFICATE"
    
    c_cert1_start = "const char cert1[] PROGMEM = R\"KEY(\n"
    c_cert2_start = "const char cert2[] PROGMEM = R\"KEY(\n"
    c_cert_end = ")KEY\";\n"

    with open("../certs/chain.pem") as fin:
        certs_in = fin.read()
    
    certs_data += "#include <Arduino.h>\n\n"

    for line in certs_in.split("\n"):
        if begin in line:
            if first:
                certs_data += c_cert1_start
                first = False
            else:
                certs_data += c_cert2_start

        certs_data += line + "\n"

        if end in line:
            certs_data += c_cert_end
            certs_data += "\n"
    
    with open("../certs/certs.h", "w") as fout:
        fout.write(certs_data)


def generate_fingerprint():
    output = subprocess.run(["openssl", "x509", "--in", "../certs/cert.pem", "--noout", "--fingerprint", "-sha1"], capture_output=True)
    str_output = output.stdout.strip().decode()
    fingerprint = str_output[str_output.index("=")+1:]
    return fingerprint


def input_conf():
    print("You will be prompted to insert some data to create a configuration file")
    ssid = input("SSID: ")
    key = input("key: ")
    domain = input("domain: ")
    port = input("port: ")
    username = input("username: ")
    password = input("password: ")
    clientid = input("MQTT Client Id: ")

    config = {
        "WIFI_SSID": ssid,
        "KEY": key,
        "DOMAIN_NAME": domain,
        "PORT": int(port),
        "USERNAME": username,
        "PASSWORD": password,
        "CLIENTID": clientid,
        "FINGERPRINT": generate_fingerprint()
    }

    with open("../data.json", "w") as fout:
        json.dump(config, fout, indent=4)


if __name__ == "__main__":
    certs = {"cert.pem", "chain.pem"}
    if not certs.issubset(set(os.listdir("../certs"))):
        print(missing_certs_msg)
        sys.exit(1)

    generate_certs_h()
    input_conf()

