#include"pch.h"
import Mex工具;
void 初始化()
{
	//首次载入MEX时，显示作者和版本信息
	Mex工具::MATLAB引擎->feval<void>("disp", matlab::data::String(u"Mex工具示例\n作者：埃博拉酱\n版本：v7.0.0"));
}