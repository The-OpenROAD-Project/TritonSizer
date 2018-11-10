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

### Authors
- Hyein Lee and Dr. Jiajia Li (Ph.D. advisors: Prof. Andrew B. Kahng)
- Many subsequent improvements were made by Minsoo Kim leading up to the initial release.
