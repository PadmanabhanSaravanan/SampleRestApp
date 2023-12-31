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

// Update a todo in the given collection based on a specific key-value pair with new key-value pairs.
bool updateTodo(mongocxx::collection& collection,
    const string& key, const string& value,
    const vector<pair<string, string>>& newKeyValues)
{
    // Create the query filter to find the document to update
    auto filter = bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize;

    // Create the new values for the document
    bsoncxx::builder::stream::document updated_document{};
    for (auto& keyValue : newKeyValues)
    {
        updated_document << keyValue.first << keyValue.second;
    }

    auto update_doc = bsoncxx::builder::stream::document{} << "$set" << updated_document << bsoncxx::builder::stream::finalize;

    // Update the document
    auto result = collection.update_one(filter.view(), update_doc.view());

    // Return true if at least one document was modified
    return result && result->modified_count() > 0;
}

// Delete a todo from the given collection based on a specific key-value pair.
bool deleteTodo(mongocxx::collection& collection, const string& key, const string& value)
{
    // Create the query filter to find the document to delete
    auto filter = bsoncxx::builder::stream::document{} << key << value << bsoncxx::builder::stream::finalize;

    // Delete the document
    auto result = collection.delete_one(filter.view());

    // Return true if at least one document was deleted
    return result && result->deleted_count() > 0;
}