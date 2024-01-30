#!/bin/python3
import os
import shlex
import subprocess
import socket

KEYBOARD = "sparrow60c"
KEYMAP = "74th"

current = socket.gethostname()

host = ""
if "SSH_CLIENT" in os.environ:
    ssh_client = os.environ["SSH_CLIENT"].split(" ")
    host = ssh_client[0]
    vm = current
    if host in ["192.168.1.186"]:
        code_path = "/usr/bin/code"
    else:
        code_path = "/usr/local/bin/code"

cmd = ["qmk", "compile", f"-kb={KEYBOARD}", f"-km={KEYMAP}"]

print("$", shlex.join(cmd))
subprocess.run(cmd, check=True, text=True)

if host:
    cmd = [
        "scp",
        f".build/{KEYBOARD}_{KEYMAP}.uf2",
        f"nnyn@{host}:/Users/nnyn/Desktop/",
    ]

    print("$", shlex.join(cmd))
    subprocess.run(cmd, check=True, text=True)
