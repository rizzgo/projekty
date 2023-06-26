import subprocess
import sys
from os import listdir, path, mkdir, rmdir

def run(cmd):
    return subprocess.run([cmd], shell=True, capture_output=True)

def parseArgs():
    return sys.argv[1] if len(sys.argv) >= 2 else "tests"

def run_tests():
    if not path.exists(test_outputs):
        mkdir(test_outputs)
    for file in listdir(tests_path):
        if file.endswith(".in"):
            file_name = path.splitext(file)[0]
            file_path = path.join(tests_path, file)
            command = f"{program_path} < {file_path} > {test_outputs}/{file_name}.out"
            print(command)
            run(command).stdout.decode("utf-8")

def diff():
    for file in listdir(test_outputs):
        if file.endswith(".out"):
            ref_file_path = path.join(tests_path, file)
            file_path = path.join(test_outputs, file)
            command = f"diff {ref_file_path} {file_path}"
            print(command, "passed" if run(command).returncode == 0 else "failed")

tests_path = parseArgs()
test_outputs = "test-outputs"
print("compiling...")
run("swipl -q -g main -o flp22-log -c flp22-log.pl").stdout.decode("utf-8")
program_path = "./flp22-log"

run_tests()
diff()
run(f"rm -R {test_outputs}")
