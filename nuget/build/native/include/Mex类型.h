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