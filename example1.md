# Example1.md

**Essential Packages on Ubuntu**

1. apt-get update

```python
apt-get update
```

2.  apt-get install -y sudo vim wget unzip g++ cmake curl pkg-config libssl-dev libsasl2-dev git python3 nano

```c
apt-get install -y sudo vim wget unzip g++ cmake curl pkg-config libssl-dev libsasl2-dev git python3 nano
```

3. mkdir simpleapp  

```python
mkdir simpleapp   
```

4. cd simpleapp 

```python
cd simpleapp
```

**Extract Crow Framework**

5.  wget https://github.com/CrowCpp/Crow/releases/download/v1.0%2B5/crow-v1.0+5.tar.gz

```python
wget https://github.com/CrowCpp/Crow/releases/download/v1.0%2B5/crow-v1.0+5.tar.gz
```

6.  mkdir crow

```python
mkdir crow
```

7.  tar xvfz crow-v1.0+5.tar.gz -C crow --strip-components=1

```python
tar xvfz crow-v1.0+5.tar.gz -C crow --strip-components=1
```

**Extract Boost Libraries**

8.  wget https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/boost_1_83_0.tar.gz

```python
wget https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/boost_1_83_0.tar.gz
```

9.  tar -xzvf boost_1_83_0.tar.gz

```python
tar -xzvf boost_1_83_0.tar.gz
```

<br>

10.  nano main.cpp

```python
nano main.cpp
```

```c
#include "crow.h"
//#include "crow_all.h"

int main()
{
    crow::SimpleApp app; //define your crow application

    //define your endpoint at the root directory
    CROW_ROUTE(app, "/")([](){
        return "Hello world";
    });

    //set the port, set the app to run on multiple threads, and run the app
    app.port(18080).multithreaded().run();
}
```

11.  nano CMakeLists.txt

```python
nano CMakeLists.txt
```

```cmake
cmake_minimum_required(VERSION 3.15)
project(simplecpp)

# Define the include directories
set(INCLUDE_PATHS ./boost_1_83_0 ./crow/include)

# Add the executable target
add_executable(simplecpp main.cpp)

# Include the defined paths
target_include_directories(simplecpp PUBLIC ${INCLUDE_PATHS})

# Specify the C++ standard
set_target_properties(simplecpp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
)
```

**Build the Application**

12.  mkdir builds

```python
mkdir builds
```

13.  cd build/

```python
cd build/
```

14.  cmake ..

```python
cmake ..
```

15.  make

```python
make
```

**Run the Application**

16.  ./restapicpp

```python
./restapicpp &
```