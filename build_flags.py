import json
import sys
import os

config_error_msg = "You have to generate a data.json file by calling the conf_init.py script"

if "data.json" not in os.listdir("."):
    print(config_error_msg)
    sys.exit(1)

conf = ""
out_str = ""
with open("data.json", "r") as fin:
    conf = json.load(fin)

for key, value in conf.items():
    if key == "PORT":
        out_str += f"-D {key}={value} "
    else:
        out_str += f"\"-D {key}={value}\" "

print(out_str)
