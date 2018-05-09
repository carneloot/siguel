from os import listdir, mkdir
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

    for filename in listdir(TESTS_FOLDER):
        if not filename.endswith(".geo"):
            continue

        print("Rodando \"{0}\".".format(filename))
        output = subprocess.Popen(
            RUN_COMMAND.format(
                APP_PATH, TESTS_FOLDER, OUT_FOLDER, filename
            ),
            shell=True,
            stdout=subprocess.PIPE
        ).stdout.read()
        output = str(output, 'utf-8')
        print(output)