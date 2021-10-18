
# use Ubuntu 20.04 as build image
FROM ubuntu:20.04

# install the build toolchain (gcc, cmake)
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y build-essential cmake \
        python3 python3-distutils python3-pip python3-dev ninja-build && \
    pip3 install pip --upgrade && pip3 install scikit-build && \
    rm -rf /var/lib/apt/lists/*

# print the build toolchain versions
RUN cmake --version && gcc --version

# copy the source code
ADD ./ /app/src

# compile the source code as python wheel
WORKDIR /app/src
RUN python3 setup.py bdist

# build the source code
# WORKDIR /app/build
#RUN cmake ../src -DCMAKE_BUILD_TYPE=Release && cmake --build .

# run the unit tests
# WORKDIR /app/build/tests
# RUN ./move_gen_test

# TODO: package the lib binaries
