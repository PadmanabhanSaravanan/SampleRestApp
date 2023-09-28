# Use latest Ubuntu version
FROM ubuntu:latest

# Update apt and install necessary dependencies
RUN apt-get update && \
    apt-get install -y sudo wget unzip g++ cmake curl pkg-config libssl-dev libsasl2-dev git python3 nano vim && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

# Install mongo-c-driver
RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.24.4/mongo-c-driver-1.24.4.tar.gz && \
    tar -xzvf mongo-c-driver-1.24.4.tar.gz && \
    cd mongo-c-driver-1.24.4 && \
    cd build && \
    cmake .. && \
    cmake --build . --config RelWithDebInfo --target install && \
    cd .. && \
    cd .. && \
    rm mongo-c-driver-1.24.4.tar.gz

# Install mongo-cxx-driver
RUN wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.0/mongo-cxx-driver-r3.7.0.tar.gz && \
    tar -xzvf mongo-cxx-driver-r3.7.0.tar.gz && \
    cd mongo-cxx-driver-r3.7.0 && \
    cd build && \
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local .. && \
    cmake --build . --target install && \
    cd .. && \
    cd .. && \
    rm mongo-cxx-driver-r3.7.0.tar.gz

# Download and unzip Crow
RUN wget https://github.com/CrowCpp/Crow/releases/download/v1.0%2B5/crow-v1.0+5.tar.gz && \
    mkdir crow && \
    tar xvfz crow-v1.0+5.tar.gz -C crow --strip-components=1 && \
    rm crow-v1.0+5.tar.gz

# Download and unzip Boost
RUN wget https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/boost_1_83_0.tar.gz && \
    tar -xzvf boost_1_83_0.tar.gz && \
    rm boost_1_83_0.tar.gz

# Copy main.cpp and CMakeLists.txt into the Docker container 
COPY main.cpp Methods.h CMakeLists.txt /app/

# Setup for building the app using CMake
RUN mkdir build
WORKDIR /app/build

# Run cmake and make
RUN cmake .. && make

# Expose port 8080
EXPOSE 8080

# Command to run your CrowSample app
CMD ["./CrowSample &"]