#pragma once

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
	};

	// Predefined
	class AbstractResponseData
	{
	public:
		virtual ~AbstractResponseData() = default;
		virtual ResponseType getType() const = 0;
		virtual std::string_view getName() const = 0;
		virtual std::string_view getDescription() const = 0;
	};

	// Predefined
	class AbstractService
	{
	public:
		virtual ~AbstractService() = default;
		virtual AbstractResponseData* handleRequest(const AbstractRequestData* r) = 0;
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

		AbstractResponseData* handleRequest(const AbstractRequestData* r) final
		{
			switch (r->getType())
			{
				case RequestType::Add:
					return handleAddRequest(static_cast<const AddRequestData*>(r));
				case RequestType::Subtract:
					return handleSubstractRequest(static_cast<const SubtractRequestData*>(r));
			}
		}

		virtual AddResponseData* handleAddRequest(const AddRequestData* r) = 0;
		virtual SubtractResponseData* handleSubstractRequest(const SubtractRequestData* r) = 0;
	};

	// User defined
	class MathService: public AbstractMathService
	{
	public:
		AddResponseData* handleAddRequest(const AddRequestData* r) final
		{
			return new AddResponseData();
		}

		SubtractResponseData* handleSubstractRequest(const SubtractRequestData* r) final
		{
			return new SubtractResponseData();
		}
	};

	class MathServiceClient: public AbstractMathService
	{
	public:
		AddResponseData* add(const AddRequestData* r)
		{
			return new AddResponseData();
		}

		SubtractResponseData* substract(const SubtractRequestData* r)
		{
			return new SubtractResponseData();
		}
	};

}  // namespace math
}  // namespace service
