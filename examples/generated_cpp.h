#pragma once

#include <memory>
#include <string_view>

namespace service
{
namespace math
{
	enum class RequestType;
	enum class ResponseType;

	// Predefined
	class AbstractRequestData
	{
	public:
		virtual ~AbstractRequestData() = default;
		virtual RequestType getType() const = 0;
		virtual std::string_view getName() const = 0;
		virtual std::string_view getDescription() const = 0;
		virtual void serialise() = 0;
		virtual void deserialise() = 0;
	};

	// Predefined
	class AbstractResponseData
	{
	public:
		virtual ~AbstractResponseData() = default;
		virtual ResponseType getType() const = 0;
		virtual std::string_view getName() const = 0;
		virtual std::string_view getDescription() const = 0;
		virtual void serialise() = 0;
		virtual void deserialise() = 0;
	};

	// Predefined
	class AbstractService
	{
	public:
		virtual ~AbstractService() = default;
		virtual std::unique_ptr<AbstractResponseData> handleRequest(const AbstractRequestData* r) = 0;
		virtual std::string_view getName() const = 0;
		virtual std::string_view getDescription() const = 0;
	};

	// Generated
	enum class RequestType
	{
		Add,
		Subtract
	};

	// Generated
	enum class ResponseType
	{
		Add,
		Subtract
	};

	// Generated
	class AddRequestData: public AbstractRequestData
	{
	public:
		RequestType getType() const final
		{
			return RequestType::Add;
		}

		std::string_view getName() const final
		{
			return "Add request data";
		}

		std::string_view getDescription() const final
		{
			return "Description of Add request data";
		}

		void serialise() final
		{
			// Serialise
		}

		void deserialise() final
		{
			// Deserialise
		}

		int a;
		int b;
	};

	// Generated
	class SubtractRequestData: public AbstractRequestData
	{
	public:
		RequestType getType() const final
		{
			return RequestType::Subtract;
		}

		std::string_view getName() const final
		{
			return "Subtract request data";
		}

		std::string_view getDescription() const final
		{
			return "Description of Subtract request data";
		}

		void serialise() final
		{
			// Serialise
		}

		void deserialise() final
		{
			// Deserialise
		}

		int a;
		int b;
	};

	// Generated
	class AddResponseData: public AbstractResponseData
	{
	public:
		ResponseType getType() const final
		{
			return ResponseType::Add;
		}

		std::string_view getName() const final
		{
			return "Add response data";
		}

		std::string_view getDescription() const final
		{
			return "Description of Add response data";
		}

		void serialise() final
		{
			// Serialise
		}

		void deserialise() final
		{
			// Deserialise
		}

		int c;
	};

	// Generated
	class SubtractResponseData: public AbstractResponseData
	{
	public:
		ResponseType getType() const final
		{
			return ResponseType::Subtract;
		}

		std::string_view getName() const final
		{
			return "Subtract response data";
		}

		std::string_view getDescription() const final
		{
			return "Description of Subtract response data";
		}

		void serialise() final
		{
			// Serialise
		}

		void deserialise() final
		{
			// Deserialise
		}

		int c;
	};

	// Generated
	class AbstractMathService: public AbstractService
	{
	public:
		std::string_view getName() const final
		{
			return "Math service";
		}

		std::string_view getDescription() const final
		{
			return "Description of Math service";
		}

		std::unique_ptr<AbstractResponseData> handleRequest(const AbstractRequestData* r) final
		{
			switch (r->getType())
			{
				case RequestType::Add:
					return handleAddRequest(static_cast<const AddRequestData*>(r));
				case RequestType::Subtract:
					return handleSubstractRequest(static_cast<const SubtractRequestData*>(r));
			}
		}

		virtual std::unique_ptr<AddResponseData> handleAddRequest(const AddRequestData* r) = 0;
		virtual std::unique_ptr<SubtractResponseData> handleSubstractRequest(const SubtractRequestData* r) = 0;
	};

	// User defined
	class MathService: public AbstractMathService
	{
	public:
		// Needs to be a singleton
		// Create and own shared memory region called service.math

		std::unique_ptr<AddResponseData> handleAddRequest(const AddRequestData* r) final
		{
			return new AddResponseData();
		}

		std::unique_ptr<SubtractResponseData> handleSubstractRequest(const SubtractRequestData* r) final
		{
			return new SubtractResponseData();
		}
	};

	// Generated
	class ServiceClient
	{
	public:
		std::unique_ptr<AbstractResponseData> makeRequest(const AbstractRequestData* r)
		{
			// Connect to shared memory region called service.math
			// Send request
			// Receive response
			// Return response
			return {};
		}
	};

	// Generated
	class MathServiceClient: public ServiceClient
	{
	public:
		// Connect to shared memory region called service.math

		int add(int a, int b)
		{
			AddRequestData request;
			request.a = a;
			request.b = b;
			request.serialise();
			const auto response = makeRequest(&request);
			return static_cast<AddResponseData*>(response.get())->c;
		}

		int subtract(int a, int b)
		{
			SubtractRequestData request;
			request.a = a;
			request.b = b;
			request.serialise();
			const auto response = makeRequest(&request);
			return static_cast<SubtractResponseData*>(response.get())->c;
		}
	};

}  // namespace math
}  // namespace service

int main()
{
	service::math::MathService service;
	service::math::MathServiceClient client;

	service::math::AddRequestData addRequest;
	const auto c = client.add(1, 2);
	const auto d = client.subtract(1, 2);

	return 0;
}
