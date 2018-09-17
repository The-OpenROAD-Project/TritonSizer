# TritonSizer
UCSD Sizer 

### Pre-requisite
* OpenSTA API

### How To Compile
    $ git clone --recursive https://github.com/abk-openroad/TritonSizer.git
    
Then, modify the __ZLIB_HOME__ and __OPENSTA_HOME__ to the corresponding install paths in [src/Makefile](src/Makefile)

    $ cd ~/TritonSizer
    $ make clean
    $ make 
    
### How To Execute

    $ sizer -env <environment file> -f <command file> | tee log

    //<environment file> is used to set environment variables and library information. <command file> contains information for input/output files and command options. 

### Manual
* https://docs.google.com/document/d/1jCDMGJbCMSK3CwtcBq9Lcs2AmPfUcAAw5R6qbl-HDng
    
### 3-party module
* Tcl v8.4.20
* zlib v1.2.3
* OpenSTA API

### License
* BSD-3-clause License

### Authors
- Hyein Lee and Dr. Jiajia Li (Ph.D. advisors: Prof. Andrew B. Kahng)
- Many subsequent improvements were made by Minsoo Kim leading up to the initial release.
