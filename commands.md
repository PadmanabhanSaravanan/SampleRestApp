# Commands

**Essential Packages on Ubuntu**

1. apt-get update

```python
apt-get update
```

2.  apt-get install -y sudo vim wget unzip g++ cmake curl pkg-config libssl-dev libsasl2-dev git python3 nano

**MongoDB C Driver**

3.  wget https://github.com/mongodb/mongo-c-driver/releases/download/1.24.4/mongo-c-driver-1.24.4.tar.gz
4.  tar -xzvf mongo-c-driver-1.24.4.tar.gz
5.  cd mongo-c-driver-1.24.4/build
6.  cmake ..
7.  cmake --build . --config RelWithDebInfo --target install
8.  cd ../..

**MongoDB C++ Driver**

9.  wget https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.7.0/mongo-cxx-driver-r3.7.0.tar.gz
10.  tar -xzvf mongo-cxx-driver-r3.7.0.tar.gz
11.  cd mongo-cxx-driver-r3.7.0/build
12.  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local ..
13.  cmake --build . --target install
14.  cd ../..

**Extract Crow Framework**

15.  wget https://github.com/CrowCpp/Crow/releases/download/v1.0%2B5/crow-v1.0+5.tar.gz
16.  mkdir crow
17.  tar xvfz crow-v1.0+5.tar.gz -C crow --strip-components=1

**Extract Boost Libraries**

18.  wget https://boostorg.jfrog.io/artifactory/main/release/1.83.0/source/boost_1_83_0.tar.gz
19.  tar -xzvf boost_1_83_0.tar.gz

<br>

20.  nano main.cpp

```c
#include "Methods.h"

// ********************************************** Main **********************************************
int main()
{
	crow::SimpleApp app; //define your crow application
	set_global_base("."); //search for the files in current dir.
	mongocxx::instance inst{};
	string mongoConnect = std::string("your-mongodb-uri");
	mongocxx::client conn{ mongocxx::uri{mongoConnect} };
	auto collection = conn["TodoRecords"]["TodoCollection"];//get collection from database

	//API endpoint to read all todos
	CROW_ROUTE(app, "/api/v1/todos")
		([&collection](const request& req) {
		mongocxx::options::find opts;
		auto docs = collection.find({}, opts);
		vector<crow::json::rvalue> todo;

		for (auto doc : docs) {
			todo.push_back(json::load(bsoncxx::to_json(doc)));
		}
		crow::json::wvalue dto;
		dto["todos"] = todo;
		return crow::response{ dto };
			});

	//API endpoint to insert todo from the given json body
	CROW_ROUTE(app, "/api/v1/todos").methods(HTTPMethod::POST)
		([&collection](const request& req) {
		crow::json::rvalue request_body = json::load(req.body);

		// List of required keys
		std::vector<std::string> required_keys = { "Id", "firstName", "lastName", "emailId", "location" };

		// Check if all required keys exist in the request body
		for (const auto& key : required_keys) {
			if (!request_body.has(key)) {
				return crow::response(400, "Required key '" + key + "' missing in request body");
			}
		}

		// Check if the ID is already in the database
		bool id_already_present = findTodoRecord(collection, std::string(request_body["Id"]));

		if (!id_already_present) {
			// ID is not present, so insert the new record
			insertTodo(collection, createTodo({
				{"Id", std::string(request_body["Id"])},
				{"firstName", std::string(request_body["firstName"])},
				{"lastName", std::string(request_body["lastName"])},
				{"emailId", std::string(request_body["emailId"])},
				{"location", std::string(request_body["location"])},
				}));
			return crow::response(200, "Todo Added Successfully!!");
		}
		else {
			// ID is already present
			return crow::response(400, "ID already present in the database");
		}
			});

	//set the port, set the app to run on multiple threads, and run the app
	app.bindaddr("127.0.0.1").port(8080).multithreaded().run();

}
```

21.  nano Methods.h

```c
#pragma once
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include "crow.h"
using namespace std;
using namespace crow;
using namespace crow::mustache;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

// Create a todo from the given key-value pairs.
bsoncxx::document::value createTodo(const vector<pair<string, string>>& keyValues)
{
	bsoncxx::builder::stream::document document{};
	for (auto& keyValue : keyValues)
	{
		document << keyValue.first << keyValue.second;
	}
	return document << bsoncxx::builder::stream::finalize;
}

// Add the todo to the given collection.
void insertTodo(mongocxx::collection& collection, const bsoncxx::document::value& document)
{
	collection.insert_one(document.view());
}

// Find a todo from the given key-value pairs and return true if found.
bool findTodo(mongocxx::collection& collection, const string& key, const string& value)
{
	// Create the query filter
	auto filter = bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize;
	//Add query filter argument in find
	auto cursor = collection.find({ filter });
	auto count = std::distance(cursor.begin(), cursor.end());
	if (count != 0L) {
		return true;
	}
	return false;
}

//Pass the given collection and key-value pairs.
bool findTodoRecord(mongocxx::collection& collection, const string& id)
{
	return findTodo(collection, "Id", id);
}
```

22.  nano CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.15)
project(restapicpp)

# Define the include directories
set(INCLUDE_PATHS ./boost_1_83_0 ./crow/include)

# Add the executable target
add_executable(restapicpp main.cpp)

# Include the defined paths
target_include_directories(restapicpp PUBLIC ${INCLUDE_PATHS})

# MongoDB C++ driver includes and links
# The Dockerfile does not provide the exact paths, but often, the drivers get installed to /usr/local
set(MONGOCXX_LIBS /usr/local/lib)
set(MONGOCXX_INCLUDE /usr/local/include/mongocxx/v_noabi)
set(BSONCXX_INCLUDE /usr/local/include/bsoncxx/v_noabi)

target_include_directories(restapicpp PRIVATE ${MONGOCXX_INCLUDE} ${BSONCXX_INCLUDE})
target_link_libraries(restapicpp PRIVATE 
    ${MONGOCXX_LIBS}/libmongocxx.so
    ${MONGOCXX_LIBS}/libbsoncxx.so
)

# Specify the C++ standard
set_target_properties(restapicpp PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED TRUE
)
```

**Build the Application**

23.  mkdir builds
24.  cd build/
25.  cmake ..
26.  make

**Run the Application**

27.  ./restapicpp