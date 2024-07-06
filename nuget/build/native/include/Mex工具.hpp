#pragma once
namespace Mex工具
{
	enum class Mex异常
	{
		No_exceptions,
		Type_conversion_failed,
		Memory_copy_failed,
		Enumerations_uncopyable,
		Unsupported_type,
		Unsupported_API,
		Array_cannot_convert_to_CharArray,
		Array_cannot_convert_to_String,
		Array_cannot_convert_to_std_string,
		Array_cannot_convert_to_StringArray,
		Array_cannot_convert_to_MATLABString,
		Array_cannot_copy_to_char,
		Array_cannot_copy_to_wchar_t,
		Cannot_get_pointer_of_SparseArray,
		Unexpected_SEH_exception,
		Unexpected_CPP_exception
	};
}