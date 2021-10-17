
# use Ubuntu 20.04 as build image
FROM ubuntu:20.04

# install the build toolchain (gcc, cmake)
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y build-essential cmake && apt-get install python3 && \
    rm -rf /var/lib/apt/lists/*
    

# print the build toolchain versions
RUN cmake --version && gcc --version

# copy the source code
ADD ./ /app/src
WORKDIR /app/build

# build the source code
RUN cmake ../src -DCMAKE_BUILD_TYPE=Release && cmake --build .

# run the unit tests
WORKDIR /app/build/tests
RUN ./move_gen_test

# TODO: package the lib binaries
