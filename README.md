使用本工具快速生成 MATLAB C++ MEX 数据API文件函数。

MATLAB C++ MEX 数据API文件函数本质上就是个实现了一套MEX标准接口的动态链接库，扩展名mexw64。你只需要在 Visual Studio 中新建一个C++动态链接库项目，将输出扩展名改为mexw64，包含【Mex工具.h】，然后实现其中声明的MexFunction::operator()，就可以生成出合法的MEX文件函数。此外，本工具还额外提供万能转码函数，用于常见的MATLAB与C++数据类型转换。

Mex工具.h公开接口
```C++
extern ArrayFactory 数组工厂;
template <typename T>
concept 数值 = std::is_arithmetic<T>::value;

//MATLAB转C++

//一般标量转换
template<typename T>
inline T 万能转码(Array& 输入);
//一般指针转换自uint64
template <typename T>
	requires std::is_pointer<T>::value
//字符串转换
inline T 万能转码(Array& 输入);
template<>
String 万能转码<String>(Array& 输入);
template<>
CharArray 万能转码<CharArray>(Array& 输入);
template<>
std::string 万能转码<std::string>(Array& 输入);

//C++转MATLAB

//一般数组转换
template <typename T>
inline buffer_ptr_t<T> 万能转码(const T* 指针, UINT64 尺寸);
//指针转uint64
inline TypedArray<uint64_t>万能转码(const void*指针);
//一般数值标量
template <数值 T>
inline TypedArray<T> 万能转码(T 输入);
//字符串转换
template<class T>
requires std::_Is_any_of_v<T,String,MATLABString,CharArray>
T 万能转码(const char*);
template<>
String 万能转码<String>(const char*);
template<>
inline MATLABString 万能转码<MATLABString>(const char* UTF8);
template<>
inline CharArray 万能转码<CharArray>(const char* UTF8);

class MexFunction :public Function
{
public:
	//用户实现此方法作为MEX文件函数体
	void operator()(ArgumentList& outputs, ArgumentList& inputs)override;
};

//增强功能，可以使用如下三个宏定义在一个MEX文件函数中定义多个API
#define API声明(函数名) void 函数名(ArgumentList& outputs,ArgumentList& inputs)
#define API索引 constexpr void (*(API[]))(ArgumentList&, ArgumentList&) =
#define API调用 API[万能转码<UINT8>(inputs[0])](outputs, inputs);
//增强功能，可以返回一个异常值并补齐额外的输出值
void 异常输出补全(ArgumentList& outputs);

```
一个示例实现，用单个MEX文件实现一个OirReader类，通过CreateOirReader创建对象，DestroyOirReader销毁对象。该示例的完整代码[参见此处](https://github.com/Silver-Fang/Image5D/blob/master/OirReaderMex/MexFunction.cpp)。
```C++
API声明(CreateOirReader)
{
	const String 文件路径 = 万能转码<String>(inputs[1]);
	//Oir读入器是另外实现的C++类构造
	outputs[0] = 万能转码(new Oir读入器((wchar_t*)文件路径.c_str()));
}
API声明(DestroyOirReader)
{
	delete 万能转码<Oir读入器*>(inputs[1]);
	//成功结构是返回一个表示操作成功的MATLAB类型
	outputs[0] = 成功结构;
}
void MexFunction::operator()(ArgumentList& outputs, ArgumentList& inputs)
{
	API索引
	{
		CreateOirReader,
		DestroyOirReader,
	};
	try
	{
		API调用;
	}
	//Image5D异常是内部异常类型
	catch (Image5D异常 异常)
	{
		//异常转换将内部异常转换为MATLAB类型
		outputs[0] = 异常转换(异常);
		//由于函数可能有多个输出，将其他输出补齐
		异常输出补全(outputs);
	}
}
```