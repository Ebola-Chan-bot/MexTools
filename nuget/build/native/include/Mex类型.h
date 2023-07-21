#pragma once
//编写自己的MEX辅助库（不实现MEX，而是MEX依赖的DLL等）至少要包含本头文件以使用MATLAB类型
#include <mex.hpp>
//ErrorDispatch和ApiAdapterImpl互相依赖，因此必须前置mex
#include "cppmex/detail/mexErrorDispatch.hpp"
#include "cppmex/detail/mexApiAdapterImpl.hpp"
#include "cppmex/detail/mexExceptionImpl.hpp"
#include "cppmex/detail/mexIOAdapterImpl.hpp"
#include "cppmex/detail/mexFutureImpl.hpp"
#include "cppmex/detail/mexTaskReferenceImpl.hpp"
namespace matlab
{
	namespace data
	{
		template<>
		struct GetArrayType<long> { static constexpr ArrayType type = sizeof(long) == sizeof(int32_t) ? ArrayType::INT32 : ArrayType::INT64; };
		template<>
		struct GetArrayType<unsigned long> { static constexpr ArrayType type = sizeof(unsigned long) == sizeof(uint32_t) ? ArrayType::UINT32 : ArrayType::UINT64; };
	}
}