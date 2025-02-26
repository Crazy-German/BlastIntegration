#include "pch.h"
#include "ScriptGraphTypes.h"

DEFINE_TYPEGROUP(ScriptGraph)

//~ Internal type to handle Owner variable on ScriptGraphs.
IMPLEMENT_REGISTERED_TYPE(void*, Internal_VoidPtr);
//~

IMPLEMENT_REGISTERED_TYPE(float, Float);

BEGIN_IMPLEMENT_REGISTERED_TYPE(std::string, String);
registeredType.myCopyFunc = [](const std::shared_ptr<void>& aOtherPtr)
	{
		if (const std::shared_ptr<const std::string> ptrT = std::static_pointer_cast<const std::string>(aOtherPtr))
		{
			return std::make_shared<std::string>(*ptrT);
		}
		return std::make_shared<std::string>();
	};
registeredType.mySerializeFunc = []([[maybe_unused]] const RegisteredType& ThisType, const void* aDataPtr, std::vector<uint8_t>& outDataBlock)
	{
		// Strings are "fun" :P
		// Let's assume that this is a normal string...
		const std::string* strPtr = static_cast<const std::string*>(aDataPtr);

		// We can't trust the size in typeinfo for the size of a string data block.
		// Since we're not wstring we're just string length * sizeof(char) which is string length in bytes.
		outDataBlock.resize(strPtr->length() * sizeof(char));
		memcpy_s(outDataBlock.data(), outDataBlock.size(), strPtr->data(), strPtr->length());
		return true;
	};
registeredType.myDeserializeFunc = []([[maybe_unused]] const RegisteredType& ThisType, const std::vector<uint8_t>& aDataBlock, void* outDataPtr)
	{
		// And when we build a string back from a vector we need to make sure it's sized properly.
		// outDataPtr is already initialized as a pointer of the correct type.
		std::string* strPtr = static_cast<std::string*>(outDataPtr);
		*strPtr = std::string(aDataBlock.begin(), aDataBlock.end());
		return true;
	};
END_IMPLEMENT_REGISTERED_TYPE();

IMPLEMENT_REGISTERED_TYPE(CommonUtilities::Vector3<float>, Vec3);
//BEGIN_IMPLEMENT_REGISTERED_TYPE(CommonUtilities::Vector3<float>);
//
//
//registeredType.myCopyFunc = [](const std::shared_ptr<void>& aOtherPtr)
//	{
//		if (const std::shared_ptr<const CommonUtilities::Vector3<float>> ptrT = std::static_pointer_cast<CommonUtilities::Vector3<float>>(aOtherPtr))
//		{
//			return std::make_shared<CommonUtilities::Vector3<float>>(*ptrT);
//		}
//		return std::make_shared<CommonUtilities::Vector3<float>>();
//	};
////registeredType.mySerializeFunc = []([[maybe_unused]] const RegisteredType& ThisType, const void* aDataPtr, std::vector<uint8_t>& outDataBlock)
////	{
////		// Strings are "fun" :P
////		// Let's assume that this is a normal string...
////		const std::string* strPtr = static_cast<const std::string*>(aDataPtr);
////
////		// We can't trust the size in typeinfo for the size of a string data block.
////		// Since we're not wstring we're just string length * sizeof(char) which is string length in bytes.
////		outDataBlock.resize(strPtr->length() * sizeof(char));
////		memcpy_s(outDataBlock.data(), outDataBlock.size(), strPtr->data(), strPtr->length());
////		return true;
////	};
////registeredType.myDeserializeFunc = []([[maybe_unused]] const RegisteredType& ThisType, const std::vector<uint8_t>& aDataBlock, void* outDataPtr)
////	{
////		// And when we build a string back from a vector we need to make sure it's sized properly.
////		// outDataPtr is already initialized as a pointer of the correct type.
////		std::string* strPtr = static_cast<std::string*>(outDataPtr);
////		*strPtr = std::string(aDataBlock.begin(), aDataBlock.end());
////		return true;
////	};
//END_IMPLEMENT_REGISTERED_TYPE();

IMPLEMENT_REGISTERED_TYPE(bool, Boolean);

IMPLEMENT_REGISTERED_TYPE(CommonUtilities::Keys, Key);

IMPLEMENT_REGISTERED_TYPE(int, Integer);

IMPLEMENT_REGISTERED_TYPE(unsigned, Unsignedint);

IMPLEMENT_REGISTERED_TYPE(GameObject*, Actor);