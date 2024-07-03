#include"pch.h"
#include <Mex实现.h>
#include <Mex工具.h>
//本示例展示将std::vector的功能通过MEX文件函数接口暴露给MATLAB
void 初始化()
{
	//首次载入MEX时，显示作者和版本信息
	Mex工具::MATLAB引擎.feval<void>("disp", matlab::data::String(u"Mex工具示例\n作者：埃博拉酱\n版本：v7.0.0"));
}
