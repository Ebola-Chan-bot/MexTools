#include"pch.h"
#include <Mex实现.h>
#include <Mex工具.h>
//本示例展示Mex工具的各种功能
using namespace Mex工具;
using namespace matlab::data;
void 初始化()
{
	//首次载入MEX时，显示作者和版本信息
	MATLAB引擎.feval<void>("disp", String(u"Mex工具示例\n作者：埃博拉酱\n版本：v7.0.0"));
}
void 执行(matlab::mex::ArgumentList& 输出, matlab::mex::ArgumentList& 输入)
{
	//输入标量转换

	万能转码<int>(std::move(输入[0]));
	万能转码<void*>(std::move(输入[1]));
	万能转码<String>(std::move(输入[2]));
	万能转码<MATLABString>(std::move(输入[3]));
	万能转码<CharArray>(std::move(输入[4]));
	万能转码<std::string>(std::move(输入[5]));

	//输出标量转换

	//算术类型无需指定模板参数，自动转换为对应的MATLAB标量
	万能转码(0);
	//指针类型通常转换为size_t标量
	万能转码<TypedArray<size_t>>(nullptr);
}