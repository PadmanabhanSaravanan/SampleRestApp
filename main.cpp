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
	CROW_ROUTE(app, "/api/v1/todos").methods(HTTPMethod::GET)
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

	//API endpoint to update document based on the given id in the JSON body
		CROW_ROUTE(app, "/api/v1/todos").methods(HTTPMethod::PUT)
		([&collection](const request& req) {
		crow::json::rvalue request_body = json::load(req.body);

		if (!request_body.has("Id")) {
			return crow::response(400, "Required key 'Id' missing in request body");
		}

		std::string id = std::string(request_body["Id"]);
		bool id_already_present = findTodoRecord(collection, id);

		if (id_already_present) {
			// ID is present, so update the record
			 vector<pair<string, string>> updates;
			 for (auto& item : request_body) {
				 string key = item.key();
				 if (key != "Id") {
					 try {
						 string value = item.s();
						 updates.push_back({ key, value });
					 }
					 catch (const std::runtime_error&) {
						 // Not a string, ignore or handle accordingly
					 }
				 }
			 }
			if (updateTodo(collection, "Id", id, updates)) {
				return crow::response(200, "Todo Updated Successfully!!");
			}
			else {
				return crow::response(500, "Failed to update Todo");
			}
		}
		else {
			return crow::response(400, "ID not found in the database");
		}
			});

		#undef DELETE

	// endpoint to delete a document based on the given id in the JSON body
		CROW_ROUTE(app, "/api/v1/todos").methods(HTTPMethod::DELETE)
		([&collection](const request& req) {
		crow::json::rvalue request_body = json::load(req.body);

		if (!request_body.has("Id")) {
			return crow::response(400, "Required key 'Id' missing in request body");
		}

		std::string id = std::string(request_body["Id"]);
		if (deleteTodo(collection, "Id", id)) {
			return crow::response(200, "Todo Deleted Successfully!!");
		}
		else {
			return crow::response(400, "Failed to delete Todo with given ID");
		}
			});

		//API endpoint to read a specific document by Id
		CROW_ROUTE(app, "/api/v1/todos/<string>")
			([&collection](const string& id) {
			// Create the query filter based on the provided Id
			auto filter = bsoncxx::builder::stream::document{} << "Id" << id << bsoncxx::builder::stream::finalize;

			auto maybe_result = collection.find_one(filter.view());

			if (maybe_result) {
				auto doc = maybe_result->view();
				crow::json::wvalue dto;
				dto["todo"] = json::load(bsoncxx::to_json(doc));
				return crow::response{ dto };
			}
			else {
				return crow::response(404, "Todo not found");
			}
				});


	//set the port, set the app to run on multiple threads, and run the app
	app.bindaddr("127.0.0.1").port(8080).multithreaded().run();

}