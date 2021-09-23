
# use Ubuntu 20.04 as build image
FROM ubuntu:20.04

# install the build toolchain (gcc, cmake)
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y build-essential cmake && \
    rm -rf /var/lib/apt/lists/*

# print the build toolchain versions
RUN cmake --version && gcc --version

# copy the source code
ADD ./ /app/src
WORKDIR /app/build

# build the source code
RUN cmake ../src && cmake --build .

# run the unit tests
RUN ctest