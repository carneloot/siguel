import os
from time import time
import subprocess

BIN_FOLDER   = "./bin"
TESTS_FOLDER = "./test/t5"
OUT_FOLDER   = "./out/t5"
APP_NAME     = "siguel"

APP_PATH     = "{0}/{1}".format(BIN_FOLDER, APP_NAME)

RUN_COMMAND  = "{0} -e {1} -o {2} -f {3}"
QRY_COMMAND  = " -q {0}"
PM_COMMAND   = " -pm {0}"
EC_COMMAND   = " -ec {0}"
VIA_COMMAND  = " -v {0}"


def rodar_comando(comando):
    before_time = time()
    output = subprocess.Popen(
        comando,
        shell=True,
        stdout=subprocess.PIPE            
    ).stdout.read()
    output = str(output, 'utf-8')
    after_time = time()

    time_elapsed = round(after_time - before_time, 2)

    return(time_elapsed, output)

def rodar_comando_verbose(filename, comando):
    print("Rodando \"{0}\".".format(filename))

    tempo, output = rodar_comando(comando_string)

    print(output)

    print("Rodou em {} seg.\n".format(tempo))
    

if __name__ == "__main__":

    tests = os.listdir(TESTS_FOLDER)
    tests.sort()

    for filename in tests:

        file_name, file_ext = os.path.splitext(filename)

        if file_ext != ".geo":
            continue

        comando_string = RUN_COMMAND.format(
            APP_PATH, TESTS_FOLDER, OUT_FOLDER, filename
        )

        pm_name = file_name + ".pm"
        if os.path.exists(TESTS_FOLDER + "/" + pm_name):
            comando_string += PM_COMMAND.format(pm_name)

        ec_name = file_name + ".ec"
        if os.path.exists(TESTS_FOLDER + "/" + ec_name):
            comando_string += EC_COMMAND.format(ec_name)

        via_name = file_name + "-v.via"
        if os.path.exists(TESTS_FOLDER + "/" + via_name):
            comando_string += VIA_COMMAND.format(via_name)

        # print(comando_string)
        
        query_dir = "{}/{}".format(TESTS_FOLDER, file_name)

        if os.path.exists(query_dir):
            querys = os.listdir(query_dir)
            querys.sort()

            for queryname in querys:
                
                query_path = "{}/{}".format(file_name, queryname)

                query_string = comando_string + QRY_COMMAND.format(query_path)

                print(query_string)
