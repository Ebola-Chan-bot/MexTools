#include"pch.h"
import Mex工具;
void Mex工具::初始化()
{
	//首次载入MEX时，显示作者和版本信息
	Mex工具::MATLAB引擎->feval<void>("disp", matlab::data::String(u"Mex工具示例\n作者：埃博拉酱\n版本：v7.0.0"));
}
void Mex工具::执行(matlab::mex::ArgumentList& 输出, matlab::mex::ArgumentList& 输入)
{
	//此处为示例代码，用户应当在此处编写MEX函数的实际代码
	//以下代码仅为示例，将输入参数拷贝到输出参数
	输出[0] = 输入[0];
}
void Mex工具::清理()
{
	//MEX卸载时，显示再见信息
	Mex工具::MATLAB引擎->feval<void>("disp", matlab::data::String(u"再见！"));
}