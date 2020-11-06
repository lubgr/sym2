
import lldb
import sys
import os
import glob

def newestExececutablesByDate():
    allExecs = glob.glob('build*/tests/unit-tests')
    allExecs.sort(key = lambda f: os.path.getmtime(f))

    if len(allExecs) == 0:
        return None

    return allExecs[-1]

def createNewestTestTarget(debugger, command, result, unused):
    testExec = newestExececutablesByDate()
    target = debugger.CreateTargetWithFileAndArch(testExec, "")
    if target:
        print("Target: ", testExec, file = result)

def __lldb_init_module(debugger, ununsed):
    debugger.HandleCommand('command script add -f lldbtarget.createNewestTestTarget test')
