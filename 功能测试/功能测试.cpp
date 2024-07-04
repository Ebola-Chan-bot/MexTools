#include <cppmex/detail/mexFunctionAdapterImpl.hpp>
#include <mex.hpp>
using namespace matlab::mex;
using namespace matlab::data;
template<typename T>
inline T 万能转码(const matlab::data::Array& 输入)
{
	if constexpr (requires  { [&输入]()->int {return 输入[0]; }; })
		return 输入[0];
	else if constexpr (requires  { [&输入]()->int {return 输入[0]; }; })
		return 输入[0].operator T();
}
struct MexFunction :Function
{
	void operator()(ArgumentList 输出, ArgumentList 输入) override
	{
		const Array& 数组 = 输入[0];
		uint8_t k = 万能转码<uint8_t>(数组[0]);
		uint8_t a = 数组[0];
		void* b = (void*)数组[0].operator uint8_t();
		Array c = 数组[0];
		CharArray d = 数组[0];
		size_t e = 数组[0].operator Array().getNumberOfElements();
		String f = 数组[0].operator CharArray().toUTF16();
		const CharArray& g = 数组[0];
		TypedArray<uint8_t> 可变数组 = std::move(输入[1]);
		uint8_t& h = 可变数组[0];
		void* i = (void*)可变数组[0].operator uint8_t& ();
		CellArray 元胞数组 = std::move(输入[2]);
		Reference<CellArray> j = 元胞数组[0];
	}
};