#! /usr/bin/env python
# Copyright (C) 2016 Marvell International Ltd.
# All Rights Reserved.

# Flash programming wrapper/helper script (using OpenOCD flash commands)
# Note: sys.stdout.flush() and sys.stderr.flush() are required for proper
# console output in eclipse

import os, sys, platform, getopt, subprocess
from sys import platform as _platform

IFC_FILE = "ftdi.cfg"
SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

# We define which as it may not be available on Windows
def which(program):
    if _platform == "win32" or _platform == "win64":
        program = program + '.exe'

    def is_exe(fpath):
        return os.path.isfile(fpath) and os.access(fpath, os.X_OK)

    fpath, fname = os.path.split(program)
    if fpath:
        if is_exe(program):
            return program
    else:
        for path in os.environ["PATH"].split(os.pathsep):
            path = path.strip('"')
            exe_file = os.path.join(path, program)
            if is_exe(exe_file):
                return exe_file
    return ""

if _platform == "linux" or _platform == "linux2":
    if (platform.machine() == "i686"):
        OPENOCD_PATH = which(SCRIPT_DIR + "/Linux/openocd")
    else:
        OPENOCD_PATH = which(SCRIPT_DIR + "/Linux/openocd64")
elif _platform == "darwin":
    OPENOCD_PATH = which("openocd")
elif _platform == "win32" or _platform == "win64":
    OPENOCD_PATH = which(SCRIPT_DIR + "/Windows/openocd")

if not len(OPENOCD_PATH):
    print "Error: Please install OpenOCD for your platform"
    sys.exit()

def create_config():
    cfile = open ('config.cfg','w')
    cfile.write("# This is an auto-generated config file\n")
    cfile.write("set CONFIG_FLASH flash\n")
    cfile.close()

def cleanup():
    if (os.path.exists("config.cfg")):
        os.remove("config.cfg")

def exit():
    print_usage()
    cleanup()
    sys.exit()

def print_usage():
    print ""
    print "Usage:"
    print sys.argv[0]
    print " [--mcufw </path/to/mcufw>]"
    print "          Write MCU firmware binary <bin> to flash"
    print " [<-f | --flash> </path/to/flash_blob>]"
    print "          Program entire flash"
    print " [-r | --reset]"
    print "          Reset board"
    print " [-h | --help] "
    print "          Display usage"
    sys.stdout.flush()

def flash_file(addr, msg, upload_file):
    if (os.path.isfile(upload_file) == False):
        print "Error: Could not find file", upload_file
        exit()
    print msg + "..."

    print "Using OpenOCD interface file", IFC_FILE
    sys.stdout.flush()
    p = subprocess.call([OPENOCD_PATH, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f', 'config.cfg', '-f', 'openocd.cfg', '-c', ' init', '-c', 'program_image ' + addr + ' ' + upload_file , '-c', 'shutdown'])
    sys.stderr.flush()
    if (p==0):
        print msg + " done..."
    else:
        print msg + " failed..."
    sys.stdout.flush()

def reset_board():
    msg = "Resetting board"
    print msg + "..."

    print "Using OpenOCD interface file", IFC_FILE
    sys.stdout.flush()
    p = subprocess.call([OPENOCD_PATH, '-s', SCRIPT_DIR + '/interface', '-f', IFC_FILE, '-s', SCRIPT_DIR, '-f', 'openocd.cfg', '-c', 'init', '-c', 'reset', '-c', 'shutdown'])
    sys.stderr.flush()
    if (p==0):
        print msg + " done..."
    else:
        print msg + " failed..."
    sys.stdout.flush()

if len(sys.argv) <= 1:
    exit()

else:
    try:
        opts, args = getopt.getopt(sys.argv[1:], "f:r", ["flash=","mcufw=","reset"])
        if len(args):
            exit()
    except getopt.GetoptError:
        exit()

    create_config()

    if ("-f" in sys.argv) or ("--flash" in sys.argv):
	try:
            index = sys.argv.index("-f")
        except ValueError:
            index = sys.argv.index("--flash")
        flash_file("0x0", "Resetting flash to factory settings", os.path.abspath(sys.argv[index + 1]).replace('\\', '/'))

    if ("--mcufw" in sys.argv):
        index = sys.argv.index("--mcufw")
        flash_file("0x10000", "Writing MCU firmware to flash", os.path.abspath(sys.argv[index + 1]).replace('\\', '/'))

    cleanup()

    if ("-r" in sys.argv) or ("--reset" in sys.argv):
        reset_board()
