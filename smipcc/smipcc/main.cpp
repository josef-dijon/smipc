#include <nlohmann/json.hpp>

#include <fmt/core.h>

#include <iostream>
#include <string>
#include <vector>

struct DatatypeImport
{
	std::string name;
	std::string path;
};

struct Datatype
{
	std::string name;
	std::string description;
	bool isOptional;
	bool isArray;
	std::vector<Datatype> children;
};

struct Endpoint
{
	std::string name;
	std::string description;
};

struct CommandEndpoint: public Endpoint
{
	std::string command;
};

struct NotificationEndpoint: public Endpoint
{
	std::string notification;
};

struct RequestResponseEndpoint: public Endpoint
{
	std::string request;
	std::string response;
};

struct Service
{
	std::string name;
	std::string description;
	std::vector<std::string> datatypes;
	std::vector<std::unique_ptr<Endpoint>> endpoints;
};

int main()
{
	std::cout << fmt::format("Hello, {}!", "world") << std::endl;
	return 0;
}
