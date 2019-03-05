FROM centos:6
LABEL maintainer="Abdelrahman Hosny <abdelrahman@brown.edu>" 

# install gcc 7
RUN yum -y install centos-release-scl && \
    yum -y install devtoolset-7 devtoolset-7-libatomic-devel
ENV CC=/opt/rh/devtoolset-7/root/usr/bin/gcc \
    CPP=/opt/rh/devtoolset-7/root/usr/bin/cpp \
    CXX=/opt/rh/devtoolset-7/root/usr/bin/g++ \
    PATH=/opt/rh/devtoolset-7/root/usr/bin:$PATH

# install dependencies 
RUN yum install -y wget libstdc++-devel libstdc++-static libX11-devel yacc byacc \
    boost-devel zlib-devel tcl tcl-devel autoconf automake swig flex libtool

# add source code
COPY . TritonSizer

# install TritonSizer 
RUN cd TritonSizer && \
    make clean && \
    make
ENV LD_LIBRARY_PATH=/TritonSizer/module/tcl/unix/install-sp/lib

# set working directory
WORKDIR TritonSizer/src

ENTRYPOINT ["/TritonSizer/src/sizer"]
