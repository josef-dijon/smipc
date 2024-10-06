#include "libsmipc/serdes/variable-integer.hpp"

#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>

#include <fmt/core.h>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct Field
{
	Field(const nlohmann::json& json)
		: name {json.at("name").get<std::string>()}
		, description {json.at("description").get<std::string>()}
		, datatype {json.at("datatype").get<std::string>()}
		, isOptional {json.contains("optional") && json.at("optional").get<bool>()}
		, isArray {json.contains("array") && json.at("array").get<bool>()}
	{}

	std::string name {};
	std::string description {};
	std::string datatype {};
	bool isOptional {false};
	bool isArray {false};
};

struct Datatype
{
	Datatype(const nlohmann::json& json)
		: name {json.at("name").get<std::string>()}
		, description {json.at("description").get<std::string>()}
	{
		const auto jsonFields = json.at("fields").get<std::vector<nlohmann::json>>();
		fields.reserve(jsonFields.size());
		for (const auto& field : jsonFields)
		{
			fields.emplace_back(field);
		}
	}

	std::string name {};
	std::string description {};
	std::vector<Field> fields {};
};

struct Request
{
	Request(const nlohmann::json& json)
		: description {json.at("description").get<std::string>()}
	{
		const auto jsonFields = json.at("fields").get<std::vector<nlohmann::json>>();
		fields.reserve(jsonFields.size());
		for (const auto& field : jsonFields)
		{
			fields.emplace_back(field);
		}
	}

	std::string description {};
	std::vector<Field> fields {};
};

using Response = Request;

struct Endpoint
{
	Endpoint(const nlohmann::json& json)
		: name {json.at("name").get<std::string>()}
		, description {json.at("description").get<std::string>()}
		, request {json.at("request")}
		, response {json.at("response")}
	{}

	std::string name;
	std::string description;
	Request request;
	Response response;
};

struct Service
{
	Service(const nlohmann::json& json)
		: version {json.at("version").get<std::string>()}
		, name {json.at("name").get<std::string>()}
		, description {json.at("description").get<std::string>()}
	{
		const auto jsonDatatypes = json.at("datatypes").get<std::vector<nlohmann::json>>();
		datatypes.reserve(jsonDatatypes.size());
		for (const auto& datatype : jsonDatatypes)
		{
			datatypes.emplace_back(datatype);
		}

		const auto jsonEndpoints = json.at("endpoints").get<std::vector<nlohmann::json>>();
		endpoints.reserve(jsonEndpoints.size());
		for (const auto& endpoint : jsonEndpoints)
		{
			endpoints.emplace_back(endpoint);
		}
	}

	std::string version {};
	std::string name {};
	std::string description {};
	std::vector<Datatype> datatypes {};
	std::vector<Endpoint> endpoints {};
};

struct CodeGenerator
{
	void writeLine(const std::string& line)
	{
		code += line + "\n";
	}

	std::string code;
};

int main()
{
	std::cout << fmt::format("Hello, {}!", "world") << std::endl;

	// read json file
	nlohmann::json json;
	{
		std::ifstream file("examples/math.service.json");
		file >> json;
	}

	const auto serviceDescriptionVersion = json.at("service-description-version").get<uint32_t>();
	std::cout << "Service description version: " << serviceDescriptionVersion << std::endl;

	Service service {json.at("service")};

	std::cout << "Service: " << service.name << std::endl;
	std::cout << "  Version: " << service.version << std::endl;
	std::cout << "  Name: " << service.name << std::endl;
	std::cout << "  Description: " << service.description << std::endl;
	std::cout << "  Datatypes: " << std::endl;
	for (const auto& datatype : service.datatypes)
	{
		std::cout << "    " << datatype.name << std::endl;
		std::cout << "      Description: " << datatype.description << std::endl;
		std::cout << "      Fields: " << std::endl;
		for (const auto& field : datatype.fields)
		{
			std::cout << "        " << field.name << std::endl;
			std::cout << "          Description: " << field.description << std::endl;
			std::cout << "          Datatype: " << field.datatype << std::endl;
			std::cout << "          Optional: " << field.isOptional << std::endl;
			std::cout << "          Array: " << field.isArray << std::endl;
		}
	}
	std::cout << "  Endpoints: " << std::endl;
	for (const auto& endpoint : service.endpoints)
	{
		std::cout << "    " << endpoint.name << std::endl;
		std::cout << "      Description: " << endpoint.description << std::endl;
		std::cout << "      Request: " << std::endl;
		std::cout << "        Description: " << endpoint.request.description << std::endl;
		std::cout << "        Fields: " << std::endl;
		for (const auto& field : endpoint.request.fields)
		{
			std::cout << "          " << field.name << std::endl;
			std::cout << "            Description: " << field.description << std::endl;
			std::cout << "            Datatype: " << field.datatype << std::endl;
			std::cout << "            Optional: " << field.isOptional << std::endl;
			std::cout << "            Array: " << field.isArray << std::endl;
		}

		std::cout << "      Response: " << std::endl;
		std::cout << "        Description: " << endpoint.response.description << std::endl;
		std::cout << "        Fields: " << std::endl;
		for (const auto& field : endpoint.response.fields)
		{
			std::cout << "          " << field.name << std::endl;
			std::cout << "            Description: " << field.description << std::endl;
			std::cout << "            Datatype: " << field.datatype << std::endl;
			std::cout << "            Optional: " << field.isOptional << std::endl;
			std::cout << "            Array: " << field.isArray << std::endl;
		}
	}

	return 0;
}
