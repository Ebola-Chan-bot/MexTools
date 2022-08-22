#pragma once
#include <mex.hpp>
//ErrorDispatch和ApiAdapterImpl互相依赖，因此必须前置mex
#include "cppmex/detail/mexErrorDispatch.hpp"
#include "cppmex/detail/mexApiAdapterImpl.hpp"
#include "cppmex/detail/mexExceptionImpl.hpp"
#include "cppmex/detail/mexIOAdapterImpl.hpp"
#include "cppmex/detail/mexFutureImpl.hpp"
#include "cppmex/detail/mexTaskReferenceImpl.hpp"