# TritonSizer
UCSD Sizer 

### Pre-requisite
* OpenSTA API

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
* doc/TritonSizer-UserGuide-v0.1.pdf
    
### 3-party module
* Tcl v8.4.20
* zlib v1.2.3
* OpenSTA API

### License
* BSD-3-clause License

### Note for the Liberty parser in TritonSizer

The Liberty parser in TritonSizer is implemented based on the Liberty parser provided by the ISPD13 Discrete Gate Sizing contest (link: http://www.ispd.cc/contests/13/ispd2013_contest.html). We have extended and improved the original Liberty parser significantly to enable support for Liberty files from various foundries including TSMC65, TSMC45, TSMC28, TSMC16 and ASAP7. However, as we support a limited set of keywords for parsing only the information consumable by TritonSizer, some Liberty files might not work.

Here is the list of foundry libraries that we verified:
TSMC65
TSMC45
TSMC28
TSMC16
ASAP7

To get support, please provide us access to the Liberty files along with the log file and the error messages.

### Authors
- Hyein Lee and Dr. Jiajia Li (Ph.D. advisors: Prof. Andrew B. Kahng)
- Many subsequent improvements were made by Minsoo Kim leading up to the initial release.
