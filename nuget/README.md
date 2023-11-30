本项目发布在 NuGet MATLAB.MexTools，GitHub上只有标签

使用本工具快速生成 MATLAB C++ MEX 数据API文件函数。MATLAB自带的 extern include 将函数定义在了头文件中，这导致在使用多个CPP文件实现功能时会出现链接时函数重定义问题。本工具则不存在此问题。
# Mex实现
MATLAB C++ MEX 数据API文件函数本质上就是个实现了一套MEX标准接口的动态链接库，扩展名mexw64。你只需要在 Visual Studio 中新建一个C++动态链接库项目，安装此NuGet包，将输出扩展名改为mexw64，包含`<Mex实现.h>`，然后在自己的项目中定义函数对象，例：
```C++
using namespace matlab::mex;
struct MexFunction :public Function //必须命名为MexFunction，public继承Function
{
	MexFunction(); //此方法可选，用于初始化，不能有任何参数输入
	void operator()(ArgumentList& outputs, ArgumentList& inputs)override; //必须定义此方法
	virtual ~MexFunction(); //此方法可选，用于 clear mex 时释放资源，必须虚析构
};
Function* 创建Mex函数()
{
	return new MexFunction();
}
void 销毁Mex函数(Function* 函数指针)
{
	delete 函数指针;
}
```
当MATLAB初次调用MEX文件函数时，会先调用`创建Mex函数()`以取得`matlab::mex::Function`指针，然后调用其`operator()`操作符，输入参数并获取输出。执行`clear mex`命令时，则会`delete`此指针，级联调用用户定义的虚析构函数（如果有）。总之，MEX文件函数实际上是一个具有初始化、调用、析构生命周期的C\++类型，用户应当分别在初始化和析构阶段定义资源获取和释放操作，并在操作符中定义函数调用时的行为，根据输入参数产生输出。这样，即可编译出符合MEX标准、可被MATLAB直接调用的 C++ MEX 文件函数。此外，操作MATLAB数组通常还需要包含`<Mex类型.h>`。

[埃博拉酱的MATLAB扩展](https://github.com/Silver-Fang/MATLAB-Extension)项目就使用了本库参与的MEX文件函数，你可以查看此项目的C++代码，进一步了解如何使用本库编写MEX文件函数。
# Mex工具
为了进一步简化MEX编程工作流，`<Mex工具.h>`还提供了一系列MEX编程常用的工具函数。
## 万能转码
MATLAB输入给C\++的是Array类型的数组对象，我们常常需要将它转换为标量数值，或者拷贝到C\++缓冲区，或者转换为UTF8字符串（MATLAB字符串采用UTF16编码）。万能转码通过函数模板实现所有常见类型的一行转换，只需将转换目标类型设为模板参数，然后调用此函数模板，输入 MATLAB Array 对象即可。

反之，C\++产生的标量值、缓冲区和字符串也常常需要转换回Array对象以返回给MATLAB。同样使用万能转码，将MATLAB数组类型作为模板参数，输入C\++类型值，即可返回MATLAB数组类型。

某些转换可能还需要输入其它参数，详见函数签名。
## 自动析构表
C\++类对象无法直接转换为MATLAB对象。我们通常将指针传给MATLAB，然后在MATLAB端调用针对此指针的操作接口。C\++对象没有自动垃圾回收，必须手动删除对象。一般可以在MATLAB端对指针调用析构函数实现对象释放。

但是，用户可能会使用MATLAB`clear mex`命令直接卸载MEX文件函数，使得指针未被正确释放。而这些指针可能仍留在工作区中，MATLAB仍可能再次调用这些已变成无效的指针的方法。这些非法操作将导致整个MATLAB进程崩溃。为了避免用户误操作导致崩溃，建议使用自动析构表。

Mex工具维护一个内置的自动析构表。每当有C\++对象指针在堆中被新建，并且需要回传给MATLAB前，将此指针登记在自动析构表中：
```C++
namespace Mex工具
{
	//将对象指针加入自动析构表。clear mex 时此指针将被自动delete。只能对new创建的对象指针使用此方法。
	template<typename T>
	inline void 自动析构(T* 对象指针)noexcept;
	//将对象指针加入自动析构表。clear mex 时此对象将被自动析构。使用指定的删除器。
	template<typename T>
	inline void 自动析构(T* 对象指针, void(*删除器)(T*))noexcept;
}
```
可选同时提供特殊的删除器函数。登记在自动析构表中的对象指针，会在`clear mex`时自动析构，释放资源。

另一个问题是，析构后变为无效的对象指针可能仍然存在于MATLAB工作区中。这些无效对象一旦被调用了类方法，就可能导致崩溃。如果对象方法中使用了某些被标记为`noexcept`的库函数，这种崩溃异常是无法捕获的。因此，只能在执行具体功能前必须对对象指针的有效性进行检查：
```C++
namespace Mex工具
{
	//指示此对象已被手动析构，可从自动析构表中移除。
	inline void 手动析构(void* 对象指针)noexcept;
	//检查对象指针是否存在于自动析构表中。如不存在，此指针可能是无效的，或者创建时未加入自动析构表。
	inline bool 对象存在(void* 对象指针)noexcept;
}
```
如果用户手动析构对象，必须在析构函数执行前调用`手动析构`函数，将对象指针从自动析构表中移除（此函数本身不执行析构），否则`clear mex`时会发生重复析构错误。用户调用对象方法执行前，必须用`对象存在`函数检查对象指针是否存在于自动析构表中。由于所有回传给MATLAB端的对象指针都应当加入自动析构表，所以如果发现指针不存在于自动析构表中，说明指针是无效的，应当拒绝执行对象方法，返回异常信息。
## 除此之外
`<Mex工具.h>`还公开接口：
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
## 使用示例
```C++
#include <Mex工具.h>
#include <Mex实现.h>
#include "Oir读入器.h" //假定此文件中定义了一个“Oir读入器”，现在要将它的指针传给MATLAB端使用
using namespace matlab::mex;
using namespace Mex工具;
//使用此函数将MATLAB端传来的uint64值转换为对象指针，并检查指针是否有效。
Oir读入器* 取指针(const Array& 指针标量)
{
	Oir读入器* const 指针 = 万能转码<Oir读入器*>(指针标量);
	if (对象存在(指针))
		return 指针;
	else
	{
		//处理指针无效的异常情况
	}
}
//创建对象，返回指针。不提供模板参数时，万能转码默认将指针转换为 MATLAB uint64 标量，还要加入自动析构表。
API声明(Oir_CreateReader)
{
	const String 文件路径 = 万能转码<String>(inputs[1]);
	Oir读入器*const 对象指针 = new Oir读入器((LPCWSTR)文件路径.c_str());
	outputs[1] = 万能转码(对象指针);
	自动析构(对象指针);
}
//手动删除用户提供的对象指针。删除前必须先检查指针有效性。如果指针有效，还需从自动析构表中移除此指针。
API声明(Oir_DeleteReader)
{
	Oir读入器* const 指针 = 万能转码<Oir读入器*>(inputs[1]);
	if (手动析构(指针))
		delete 指针;
}
//一个执行功能的对象方法示例。不提供模板参数时，万能转码默认将C++基本数值类型转换为等效的MATLAB标量类型。
API声明(Oir_SizeX)
{
	outputs[1] = 万能转码(取指针(inputs[1])->SizeX());
}
struct MexFunction :public Function
{
	void operator()(ArgumentList& outputs, ArgumentList& inputs)
	{
		API索引{
			// OirReader
			Oir_CreateReader,
			Oir_DeleteReader,
			Oir_SizeX,
			//……在此列出所有API函数名称
		};
		try
		{
			API调用;
		}
		catch (const Image5D异常& 异常)
		{
			//处理已知异常
		}
		catch (...)
		{
			//处理未知异常
		}
	}
};
Function* 创建Mex函数()
{
	return new MexFunction();//必须用new创建此对象指针，因为 clear mex 时将用delete析构
}
void 销毁Mex函数(Function* 函数指针)
{
	delete 函数指针;
}
```
# FAQ
某些计算机/编译器可能存在中文编码错误问题。你需要设置中文编码为UTF-8。对于 Windows 11，可以在任务栏搜索intl.cpl，转到【管理\非Unicode程序的语言\更改系统区域设置】，勾选【Beta版：使用 Unicode UTF-8 提供全球语言支持】。