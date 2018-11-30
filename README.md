# TritonSizer
UCSD Sizer 

### Pre-requisite
* Tcl (for OpenSTA) == 8.4 (Currently, it only supports Tcl v8.4.)
* bison
* GCC
* Recommended OS: CentOS 6

### How To Compile
    $ git clone --recursive https://github.com/abk-openroad/TritonSizer.git
    
    $ cd ~/TritonSizer
    $ make clean
    $ make 
    
### How To Execute

    $ source load.sh
	$ cd src
	$ sizer -env <environment file> -f <command file> | tee log

    //<environment file> is used to set environment variables and library information. <command file> contains information for input/output files and command options. 

### Manual
* doc/TritonSizer-UserGuide-v*.pdf
    
### 3-party module
* Tcl v8.4.20
* OpenSTA API

### License
* BSD-3-clause License

### Note for the Liberty parser in TritonSizer

The Liberty parser in TritonSizer is implemented based on the Liberty parser provided by the ISPD13 Discrete Gate Sizing contest (link: http://www.ispd.cc/contests/13/ispd2013_contest.html). We have extended and improved the original Liberty parser significantly to enable support for Liberty files from various foundries including TSMC65, TSMC45, ST28, TSMC16 and ASAP7. However, as we support a limited set of keywords for parsing only the information consumable by TritonSizer, some Liberty files might not work.

Here is the list of foundry libraries that we have verified:
TSMC65
TSMC45
ST28
TSMC16
ASAP7

To obtain support for another library, please provide us access to the Liberty files along with the TritonSizer log file and the error messages.  Alternatively, you make code changes in ckt.cpp files and run the regression tests in the bench directory (with ASAP7 library), then issue a pull request.

Contact: Minsoo Kim, mik226@eng.ucsd.edu

### Authors
- Hyein Lee and Dr. Jiajia Li (Ph.D. advisors: Prof. Andrew B. Kahng)
- Many subsequent improvements were made by Minsoo Kim leading up to the initial release.
