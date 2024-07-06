export module Mex工具:实现;
import "MexFunction.hpp";
import std;
namespace Mex工具
{
	using namespace matlab::data;
	template<ArrayType T>struct 动态类型转静态_s;
	template<>struct 动态类型转静态_s<ArrayType::LOGICAL> { using 类型 = bool; };
	template<>struct 动态类型转静态_s<ArrayType::CHAR> { using 类型 = CHAR16_T; };
	template<>struct 动态类型转静态_s<ArrayType::MATLAB_STRING> { using 类型 = MATLABString; };
	template<>struct 动态类型转静态_s<ArrayType::DOUBLE> { using 类型 = double; };
	template<>struct 动态类型转静态_s<ArrayType::SINGLE> { using 类型 = float; };
	template<>struct 动态类型转静态_s<ArrayType::INT8> { using 类型 = int8_t; };
	template<>struct 动态类型转静态_s<ArrayType::INT16> { using 类型 = int16_t; };
	template<>struct 动态类型转静态_s<ArrayType::INT32> { using 类型 = int32_t; };
	template<>struct 动态类型转静态_s<ArrayType::INT64> { using 类型 = int64_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT8> { using 类型 = uint8_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT16> { using 类型 = uint16_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT32> { using 类型 = uint32_t; };
	template<>struct 动态类型转静态_s<ArrayType::UINT64> { using 类型 = uint64_t; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_DOUBLE> { using 类型 = std::complex<double>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_SINGLE> { using 类型 = std::complex<float>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT8> { using 类型 = std::complex<int8_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT16> { using 类型 = std::complex<int16_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT32> { using 类型 = std::complex<int32_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_INT64> { using 类型 = std::complex<int64_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT8> { using 类型 = std::complex<uint8_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT16> { using 类型 = std::complex<uint16_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT32> { using 类型 = std::complex<uint32_t>; };
	template<>struct 动态类型转静态_s<ArrayType::COMPLEX_UINT64> { using 类型 = std::complex<uint64_t>; };
	template<>struct 动态类型转静态_s<ArrayType::CELL> { using 类型 = Array; };
	template<>struct 动态类型转静态_s<ArrayType::STRUCT> { using 类型 = Struct; };
	template<>struct 动态类型转静态_s<ArrayType::ENUM> { using 类型 = Enumeration; };
	template<>struct 动态类型转静态_s<ArrayType::OBJECT> { using 类型 = Object; };
	template<>struct 动态类型转静态_s<ArrayType::SPARSE_LOGICAL> { using 类型 = SparseArray<bool>; };
	template<>struct 动态类型转静态_s<ArrayType::SPARSE_DOUBLE> { using 类型 = SparseArray<double>; };
	template<>struct 动态类型转静态_s<ArrayType::SPARSE_COMPLEX_DOUBLE> { using 类型 = SparseArray<std::complex<double>>; };

	//自动析构实现
	
	extern std::map<void*, std::move_only_function<void(void*)const>>自动析构表;
	//将对象指针加入自动析构表。clear mex 时此指针将被自动delete。只能对new创建的对象指针使用此方法。
	template<typename T>
	inline void 自动析构(T* 对象指针)noexcept
	{
		自动析构表[对象指针] = [](void* 指针) {delete (T*)指针; };
	}
	//将对象指针加入自动析构表。clear mex 时此对象将被自动析构。使用指定的删除器。
	template<typename T>
	inline void 自动析构(T* 对象指针, std::move_only_function<void(void*)>&& 删除器)noexcept
	{
		自动析构表[对象指针] = std::move(删除器);
	}
	//此方法用于提示指定对象已被自动析构，从而避免自动析构表重复析构。此方法不负责析构对象本身，对象本身仍由调用方负责析构。
	inline bool 手动析构(void* 对象指针)noexcept
	{
		return 自动析构表.erase(对象指针);
	}
	//检查对象指针是否存在于自动析构表中。如不存在，此指针可能是无效的，或者创建时未加入自动析构表。
	inline bool 对象存在(void* 对象指针)noexcept
	{
		return 自动析构表.contains(对象指针);
	}
}