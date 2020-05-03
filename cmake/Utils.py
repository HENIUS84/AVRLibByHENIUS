import subprocess
import re
from _ast import Try

#*******************************************************************************
def get_coverage_tool_version(appName, versionArg = "--version"):
    """
    Prints the version of specified coverage tool
    """
    try:
        result = subprocess.run([appName, versionArg], \
                                stdout=subprocess.PIPE, \
                                stderr=subprocess.STDOUT).stdout.decode("utf-8")
        version_match = re.search(appName + " (Version:)?(.*)", result)
        print (appName, "version", version_match.group(2).strip())
    except:
        print ("?")
    
#*******************************************************************************
def get_gcovr_version():
    """
    Prints the version of gcovr
    """
    get_coverage_tool_version("gcovr")

#*******************************************************************************
def get_opencppcoverage_version():
    """
    Prints the version of OpenCppCoverage
    """
    get_coverage_tool_version("OpenCppCoverage", "--help")
        
if __name__ == "__main__":
    get_gcovr_version()