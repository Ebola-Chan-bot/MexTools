export module Mex工具;
#include<cppmex/mexFunction.hpp>
#include<MatlabDataArray/ArrayFactory.hpp>
#include<cppmex/mexMatlabEngine.hpp>
namespace Mex工具
{
	//用户必须定义以下函数

	//全局初始化，在MATLAB首次载入MEX时调用。用户应当在此函数中进行全局变量初始化、持久资源分配等不应在每次调用时重复进行的操作。全局变量也可以在本函数外初始化，但这样做不能保证初始化顺序，仅适用于不依赖其它全局变量的情况。在此方法中进行具有严格顺序依赖要求的全局变量初始化。
	export void 初始化();
	//执行调用，MATLAB每次调用MEX文件函数时调用此方法。用户应当在此函数中处理输入参数，充分利用初始化阶段分配的持久资源，然后将结果写入输出参数
	export void 执行(matlab::mex::ArgumentList& 输出, matlab::mex::ArgumentList& 输入);
	//全局清理，在MATLAB卸载MEX（包括 clear mex 以及MATLAB会话退出）时调用。用户应当在此函数中释放全局变量、持久资源等
	export void 清理();

	//有用的全局变量

	export extern matlab::data::ArrayFactory 数组工厂;
	export extern std::shared_ptr<matlab::engine::MATLABEngine> MATLAB引擎;

	//万能转码

	/*一对一转换。支持以下所有转换：
	将输入MATLAB类型视为标量，转换为C++类型。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	将输入C++类型转换为MATLAB标量。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	将所有能被转换为string的MATLAB类型视为标量，转换为CharArray、MATLABString、String（i.e. std::u16string）、StringArray、std::string或std::wstring。自动执行必要的UTF16到UTF8转换。
	将所有能被MATLAB转换为StringArray的Array类型视为标量，或所有被std::ostring::operator<<或std::wostream::operator<<支持的类型，转换为CharArray、MATLABString、String（i.e. std::u16string）或StringArray。自动执行必要的UTF8到UTF16转换。
	*/
	export template<typename 输出,typename 输入>
		输出 万能转码(输入&&);
	/*将MATLAB数组拷出到迭代器。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	特别地，所有能被转换为string的MATLAB类型数组可以被拷出到接受CharArray、MATLABString、String（i.e.std::u16string）、std::string或std::wstring的迭代器。自动执行必要的UTF16到UTF8转换。
	特别地，如果迭代器是void*，将把MATLAB数组的底层字节直接拷出到目标指针，无论其是否为POD类型。
	特别地，SparseArray将被展开为满数组。
	函数执行后，迭代器将指向拷出的最后一个元素的下一个位置。
	*/
	export template<typename 迭代器>
		void 万能转码(matlab::data::Array&&, 迭代器&);
	/*从迭代器创建具有指定维度的MATLAB满数组。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
	特别地，如果输出类型是StringArray，迭代器对应的值类型可以是任何能被MATLAB转换为StringArray的类型，或所有被std::ostring::operator<<或std::wostream::operator<<支持的类型。
	特别地，如果迭代器是void*，将被解释为指向输出类型的元素类型的指针。
	函数执行后，迭代器将指向最后一个元素的下一个位置。
	*/
	export template<typename 迭代器, typename 输出类型 = matlab::data::TypedArray<std::remove_cvref_t<decltype(*(std::declval<迭代器>() + 1))>>>
		输出类型 万能转码(迭代器&, matlab::data::ArrayDimensions&&);

	//获取任意类型MATLAB数组的字节数，即元素个数×单个元素的字节数。稀疏数组的字节数包含其0元素所占字节。
	export size_t 数组字节数(const matlab::data::Array&);

	//此变量模板将静态的数组元素类型转换为ArrayType枚举值
	export template<typename T>
		extern constexpr matlab::data::ArrayType 静态类型转动态;

	export template<typename T>

}