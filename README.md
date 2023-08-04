本项目发布在 NuGet MATLAB.MexTools，GitHub上只有标签

使用本工具快速生成 MATLAB C++ MEX 数据API文件函数。MATLAB自带的 extern include 将函数定义在了头文件中，这导致在使用多个CPP文件实现功能时会出现链接时函数重定义问题。本工具则不存在此问题。

MATLAB C++ MEX 数据API文件函数本质上就是个实现了一套MEX标准接口的动态链接库，扩展名mexw64。你只需要在 Visual Studio 中新建一个C++动态链接库项目，安装此NuGet包，将输出扩展名改为mexw64，包含`<Mex实现.h>`，然后实现其中声明的MexFunction::operator()，就可以生成出合法的MEX文件函数。此外，操作MATLAB数组通常还需要包含`<Mex类型.h>`。

[埃博拉酱的MATLAB扩展](https://github.com/Silver-Fang/MATLAB-Extension)项目就使用了本库参与的MEX文件函数，你可以查看此项目的C++代码，进一步了解如何使用本库编写MEX文件函数。

为了进一步简化MEX编程工作流，`<Mex工具.h>`还提供了一系列MEX编程常用的工具函数。其中最常用的就是`万能转码`，用于在C++类型和MATLAB数组类型之间，提供各种方便的快速转换，无论是数组还是标量，并且自动转换数据类型和字符编码。除此之外，`<Mex工具.h>`还公开接口：
```C++
#include<Mex类型.h>
#include<array>
using namespace matlab::data;
using namespace matlab::mex;
namespace Mex工具
{
	enum Mex异常;
	//这里使用static而不是extern，因为从其它编译单元链接的变量不一定能在DllMain阶段完成初始化，会造成意外错误。
	static ArrayFactory 数组工厂;
	/*
* 根据动态类型选择类模板并返回成员
* MATLAB数组使用一个枚举值指示数组的动态类型。当你试图编写一个泛型函数时，一般来说需要用一个冗长的switch语句将其正确转换为C++静态类型，然后调用对应的函数模板。本函数封装此过程，根据MATLAB动态类型，实例化对应的模板类，取其返回成员。调用方可按需求设计一个模板类来包装类型特定的方法。本文件中的万能转码函数就用此方法实现标量转换，实现方法亦可作为参考示例。
* @param 模板，必须至少接受一个类型参数作为第一个模板参数，至少包含一个名为“返回”的静态成员，此成员的类型必须与运行时输入的MATLAB类型（即第一个模板参数）无关；如果是静态函数，则函数的所有输入参数和返回值类型必须与运行时输入的MATLAB类型无关。模板的第一个参数应当为所有MATLAB对应类型提供“返回”静态成员，还应当为nullptr_t类型也提供“返回”静态成员——这用于对不支持的MATLAB类型进行异常处理。例如，你可以为nullptr_t类型特化的返回成员提供一个异常输出。
* @param Ts，将要传递给模板的后续模板参数。如果你提供的模板不具有或具有可选的后续模板参数，则此处的后续模板参数也是可选的。
* @param 类型，运行时得到的MATLAB动态类型枚举
* @return 返回模板类名为“返回”的成员
*/
	template<template <typename T, typename...Ts>typename 模板, typename...Ts>
	constexpr auto 动态类型选择模板(ArrayType 类型);
	//此using在将动态类型枚举ArrayType转为静态类型
	template<ArrayType T>
	using 动态类型转静态 = 内部::动态类型转静态<T>::类型;
	constexpr uint8_t 类型总数 = 32;
	//此数组存储了每种动态类型枚举对应的静态类型的尺寸。使用方法示例：类型尺寸[(int)inputs[1].getType()]
	constexpr std::array<uint8_t, 类型总数>类型尺寸 = 内部::类型尺寸<std::make_integer_sequence<int, 类型总数>>;

	//增强功能，可以使用如下三个宏定义在一个MEX文件函数中定义多个API

#define API声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
#define API索引 constexpr void (*(API[]))(ArgumentList&, ArgumentList&) =
#define API调用 const uint8_t 选项=万能转码<uint8_t>(std::move(inputs[0]));if(选项<std::extent_v<decltype(API)>)API[选项](outputs, inputs);else throw 不支持的API;
	/*
	出错时，将后续返回值设为空数组
	为了将C++异常传递给MATLAB，我们通常需要将MEX文件函数的第一个返回值保留作为错误代码
	*/
	void 异常输出补全(ArgumentList& outputs);
	/*
	使用动态类型创建一个动态类型缓冲，执行内存拷贝然后打包为 MATLAB Array
	泛型编程的一个常见情境，就是我们不关心用户提供的运行时数据类型，只是单纯进行内存拷贝。但是MATLAB官方提供的 MEX API 不允许动态类型的内存拷贝，因此这里提供一个动态类型缓冲区，可以接受一个运行时类型枚举，创建一个动态类型缓冲，完成数据拷贝后再打包成无类型的 MATLAB Array。
	*/
	struct 动态类型缓冲
	{
		void* const 指针;
		const size_t 字节数;
		/*
		创建动态类型缓冲。
		语法：Mex工具::动态类型缓冲::创建(类型,元素数)
		# 参数
		ArrayType 类型，动态类型枚举
		size_t 元素数
		# 返回值
		std::unique_ptr<动态类型缓冲>，请通过指针操作对象，因为内部实现是被子类继承的，直接取值会导致对象被截断。
		*/
		static std::unique_ptr<动态类型缓冲>创建(ArrayType 类型, size_t 元素数);
		virtual ~动态类型缓冲() {}
		//打包后本对象变为不可用，所有数据封装在 MATLAB Array 中
		virtual Array 打包(ArrayDimensions 各维尺寸)noexcept = 0;
		virtual Array 打包()noexcept = 0;
	};
}
```
某些计算机/编译器可能存在中文编码错误问题。你需要设置中文编码为UTF-8。对于 Windows 11，可以在任务栏搜索intl.cpl，转到【管理\非Unicode程序的语言\更改系统区域设置】，勾选【Beta版：使用 Unicode UTF-8 提供全球语言支持】。