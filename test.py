from os import listdir, mkdir
from time import time
import subprocess

BIN_FOLDER   = "bin"
TESTS_FOLDER = "test"
OUT_FOLDER   = "out"
APP_NAME     = "siguel"

APP_PATH     = "{0}/{1}".format(BIN_FOLDER, APP_NAME)

RUN_COMMAND  = "{0} -f {3} -e {1} -o {2}"

if __name__ == "__main__":

    try:
        mkdir(OUT_FOLDER)
    except FileExistsError:
        pass

    tests = listdir(TESTS_FOLDER)
    tests.sort()

    for filename in tests:
        if not filename.endswith(".geo"):
            continue

        print("Rodando \"{0}\".".format(filename))

        before_time = time()
        output = subprocess.Popen(
            RUN_COMMAND.format(
                APP_PATH, TESTS_FOLDER, OUT_FOLDER, filename
            ),
            shell=True,
            stdout=subprocess.PIPE            
        ).stdout.read()
        output = str(output, 'utf-8')
        print(output)

        after_time = time()

        print("Rodou em {} seg.\n".format(
            round(after_time - before_time, 2)
        ))