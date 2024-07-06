#include<Mex工具.hpp>
#include <cppmex/detail/mexFunctionAdapterImpl.hpp>
//未使用，仅用于确保这些函数被静态库导出
static constexpr volatile void* 导出列表[] = { mexCreateMexFunction, mexDestroyMexFunction ,mexFunctionAdapter };