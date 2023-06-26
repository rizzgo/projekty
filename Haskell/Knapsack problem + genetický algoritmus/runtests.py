import subprocess
import sys
from os import listdir, path

def run(cmd):
    completed = subprocess.run(cmd, shell=True, capture_output=True)
    return completed

def parseArgs():
    if len(sys.argv) >= 2:
        return sys.argv[1]
    else:
        print("Error: Missing mode specification (-i, -b, -o).")
        print("Help: python3 runtests.py <mode>")
        exit()

def run_tests():
    for file in listdir(tests_path):
        file_path = path.join(tests_path, file)
        print(f"{program_path} {mode} {file_path}")
        print(run(f"{program_path} {mode} {file_path}").stdout.decode("utf-8"))

mode = parseArgs()
print(run("make").stdout.decode("utf-8"))
tests_path = "test"
program_path = "./flp22-fun"

run_tests()
