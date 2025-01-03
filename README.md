本项目发布在 NuGet MATLAB.MexTools，GitHub上只有标签

使用本工具快速生成 MATLAB C++ MEX 数据API文件函数。关于MEX文件函数的基础知识，参阅[MATLAB官方文档](https://ww2.mathworks.cn/help/releases/R2024b/matlab/call-mex-file-functions.html)和[实现原理与使用技巧](https://juejin.cn/post/7387394977128677376)。本文假设你已经了解这些文档中的相关知识，不再过多解释。

本工具仅在Windows和 Visual Studio 上测试通过。其它开发环境请自行适配。
# 基本工作流
使用 Visual Studio 新建一个C++动态链接库项目，然后添加此NuGet程序包。在任意.cpp源文件中，定义如下函数：
```C++
#include<Mex工具.hpp>

/*全局初始化，在MATLAB首次载入MEX时调用。用户应当在此函数中进行全局变量初始化、持久资源分配等不应在每次调用时重复进行的操作。全局变量也可以在本函数外初始化，但这样做不能保证初始化顺序，仅适用于不依赖其它全局变量的情况。在此方法中进行具有严格顺序依赖要求的全局变量初始化。
初始化约定为noexcept。不应在初始化阶段抛出任何异常。
*/
void Mex工具::初始化()noexcept{}

//执行调用，MATLAB每次调用MEX文件函数时调用此方法。用户应当在此函数中处理输入参数，充分利用初始化阶段分配的持久资源，然后将结果写入输出参数
Mex工具API(Mex工具::执行){}

/*全局清理，在MATLAB卸载MEX（包括 clear mex 以及MATLAB会话退出）时调用。用户应当在此函数中释放全局变量、持久资源等
清理约定为noexcept。不应在清理阶段抛出任何异常。
*/
void Mex工具::清理()noexcept{}
```
不需要执行任何代码的函数可以留空定义。生成。将生成的.mexw64文件置于MATLAB搜索目录中即可调用。
# 进阶使用
本工具所有高级功能都在<Mex工具.hpp\>中详述。此处作简要介绍。代码示例可参考[埃博拉酱的MATLAB扩展](https://github.com/Silver-Fang/MATLAB-Extension/tree/master/Windows%E5%B9%B3%E5%8F%B0)
## 异常处理
MATLAB只能捕获`std::exception`派生的异常。将非继承自`std::exception`的异常抛出时，将丢失异常详细信息。MATLAB会将异常抛出为`MException`类型，所有异常详细信息都包含在此类型的`identifier`和`message`两个属性中。根据你的需求，本工具提供了以下异常处理方法可选：
- `EnumThrow`。使用此方法要求你将所有可能的异常列举为一个`enum class`，并且从命名空间、类型名到枚举项名称都只能包含英文和数字，因为这些文本都会被`EnumThrow`转换为MATLAB`MException.identifier`，MATLAB要求此字段只能包含英文和数字。除此之外，`EnumThrow`还可以设置`MException.message`。
- `ThrowLastError`。此方法检查`GetLastError()`，然后使用`FormatMessageW`将错误代码转换为文本，作为`MException.message`。你仍须向此函数输入枚举类型作为`MException.identifier`。
- `WindowsErrorMessage`。此方法不抛出异常，只是将Windows异常码转换为UTF16文本返回。你可以将其自行处理，例如交给`EnumThrow`。此方法可以接受你自行获得的错误码（通常来自`GetLastError`或`GetExceptionCode`），也可以不指定错误码以自动`GetLastError`。
- <cppmex/mexException.hpp\>中定义了MATLAB提供的几种异常类型，均派生自`std::exception`。你可以自行构造这些异常然后直接`throw`，比使用本工具更灵活。
- 意外的异常。本工具提供了兜底保护机制，可以捕获大多数未被正确处理的意外异常，而不至于使MATLAB进程崩溃；但这种机制可能丢失异常详细信息，因此多数情况下您仍应有意识地妥善处理各种意外情况。但是有两种已知情况的异常无法捕获，将导致MATLAB进程崩溃：一是违反noexcept约定抛出的异常，C\++标准规定这将导致进程崩溃，无法捕获；二是在C\++标准异常处理块以外的空`throw`，同样C++标准规定这将导致进程崩溃。
## 数据交换
MEX文件函数最基本的工作就是将MATLAB数据类型转换为C++数据类型，完成处理后再转回MATLAB数据类型。提供了以下强有力的工具：
### 万能转码
本工具提供了多种万能转码的重载和模板，几乎可以覆盖所有的MATLAB与C\++数据类型相互转换任务，无论是标量转换、数组和迭代器转换还是字符串编码转换。使用时需要注意，MATLAB `Array`转C\++时，通常都需要使用`std::move`将数组转换为右值引用，转换后原数组对象将变成不可用。此外，MATLAB使用的字符（串）是UTF16编码的char16_t类型，本工具可以自动转换，请勿不经转码直接使用指针拷贝。
```C++
/*一对一转换。支持以下所有转换：
将输入MATLAB类型视为标量，转换为C++类型。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
将所有能被转换为string的MATLAB类型视为标量，转换为CharArray、MATLABString、String（i.e. std::u16string）、StringArray、std::string或std::wstring。自动执行必要的UTF16到UTF8转换。
输入右值引用，这意味着转换后输入数组可能不再可用。
*/
template<typename 输出>
inline 输出 万能转码(matlab::data::Array&& 输入);

/*一对一转换。支持以下所有转换：
将输入C++类型转换为MATLAB标量。如果类型不完全匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
将所有能被MATLAB转换为StringArray的Array类型视为标量，或所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型，转换为CharArray、MATLABString、String（i.e. std::u16string）或StringArray。自动执行必要的UTF8到UTF16转换。
最好使用std::move输入右值引用。某些转换支持常量引用，某些不支持，可以根据是否能通过编译来判定。
*/
template<typename 输出, typename T>
inline 输出 万能转码(T&& 输入);

//将MATLAB标量元素类型转换为标量数组类型
template<typename T>
	requires requires(T&& 输入) { 数组工厂.createScalar<内部::数值标准化_t<T>>(std::move(输入)); }
inline matlab::data::TypedArray<内部::数值标准化_t<T>>万能转码(T&& 输入);

//将C++指针转换为 MATLAB uint64 标量
inline matlab::data::TypedArray<size_t>万能转码(const void* 输入);

/*将MATLAB数组拷出到迭代器。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换；如果还不行，还会尝试调用MATLAB引擎转换。
特别地，所有能被转换为string的MATLAB类型数组可以被拷出到接受CharArray、MATLABString、String（i.e.std::u16string）、std::string或std::wstring的迭代器。自动执行必要的UTF16到UTF8转换。
特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
特别地，SparseArray将被填充为满数组。
输入matlab::data::Array右值引用，这意味着转换后输入对象可能不再可用。
如果输入迭代器的左值引用，函数执行后，迭代器将指向拷出的最后一个元素的下一个位置。如果输入稀疏数组，迭代器必须支持使用operator[]的随机写入方式。
*/
template<typename 迭代器>
inline void 万能转码(matlab::data::Array&& 输入, 迭代器&& 输出);

/*从迭代器创建具有指定维度的MATLAB满数组。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
特别地，如果输出类型是StringArray，迭代器对应的值类型可以是所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型；如果不支持，还可以是任何能被MATLAB转换为string的MATLAB元素对象（如枚举、分类数组或任何实现了string方法的对象等）
特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。
函数执行后，迭代器将指向最后一个元素的下一个位置。
*/
template<typename 输出类型, typename 迭代器>
inline 输出类型 万能转码(迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸);

//从迭代器创建具有迭代器值类型的MATLAB数组。输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。函数执行后，迭代器将指向最后一个元素的下一个位置。
template<typename 迭代器>
inline matlab::data::TypedArray<内部::取迭代器值类型<迭代器>>万能转码(迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸);

//从指针迭代器（指迭代器解引用得到指针，迭代器本身不一定是指针）创建具有uint64类型的MATLAB数组。输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。函数执行后，迭代器将指向最后一个元素的下一个位置。
template<typename 迭代器>
requires std::is_pointer_v<内部::取迭代器值类型<迭代器>>
inline matlab::data::TypedArray<size_t>万能转码(迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸);

/*从迭代器创建具有动态类型的MATLAB数组。如果类型不匹配，将优先执行隐式转换；如果不能隐式转换，再尝试显式转换。
特别地，如果输出类型是ArrayType::MATLAB_STRING，迭代器对应的值类型可以是所有被std::ostringstream::operator<<或std::wostringstream::operator<<支持的类型；如果不支持，还可以是任何能被MATLAB转换为string的MATLAB元素对象（如枚举、分类数组或任何实现了string方法的对象等）
特别地，如果迭代器是void*，将被强制转换为指向数组值类型的指针使用。
输入matlab::data::ArrayDimensions右值引用，这意味着转换后输入对象可能不再可用。
函数执行后，迭代器将指向最后一个元素的下一个位置。
*/
template<typename 迭代器>
inline matlab::data::Array 万能转码(matlab::data::ArrayType 元素类型, 迭代器&& 输入, matlab::data::ArrayDimensions&& 各维尺寸);

//将给定指针直接作为指定MATLAB满数组的基础数据缓冲区。数据类型必须完全匹配。必须额外指定删除器。从R2024b开始支持，之前的版本不支持。
template<typename 输出类型>
inline 输出类型 万能转码(数组类型转元素<输出类型>* 输入, matlab::data::ArrayDimensions&& 各维尺寸, matlab::data::buffer_deleter_t<void> 自定义删除器);

//将给定指针直接作为动态类型MATLAB满数组的基础数据缓冲区。数据类型必须完全匹配。必须额外指定删除器。从R2024b开始支持，之前的版本不支持。
inline matlab::data::Array 万能转码(matlab::data::ArrayType 元素类型, void* 输入, matlab::data::ArrayDimensions&& 各维尺寸, matlab::data::buffer_deleter_t<void> 自定义删除器);
```
### 动态类型缓冲
有时你不关心或无法确定运行时MATLAB数组的动态类型，只想要操作`void*`指针，可以使用动态类型缓冲。你可以将一个运行时类型枚举值和元素个数传入，获取一个动态类型的`void*`缓冲（MATLAB提供的方法必须在编译时确定类型）。向缓冲写入数据后，直接打包创建一个无类型的`Array`。反之，也可以将输入的`Array`解包为一个动态类型缓冲，取得其基础数据指针`void*`。
## 自动析构
有时我们需要将C\++对象的指针、句柄或其它具有类似功能的索引返回给MATLAB。这就意味着，这个对象的生命周期被托管给了MATLAB来管理，由MATLAB决定何时析构这个对象，而无法使用传统的C\++资源管理技术。一般来说，应当在MATLAB端也为此类对象编写一个接口类，在MATLAB对象`delete`时也自动调用C\++对象的析构方法。但是，这样做并不保证安全，有两种常见情况仍会导致资源泄漏或进程崩溃：
- 有特殊析构顺序要求的全局对象。全局对象的生命周期与MEX模块相同，一般来说MEX模块卸载时它也会自动析构。但是，有些全局对象（常见于 Windows COM）不能在DllMain卸载过程中析构。
- `clear mex`。这个命令会导致MATLAB立即卸载MEX模块，但不会调用堆上对象的析构方法。即使再次载入MEX模块，许多内部状态也已经改变，这些对象将不会被正确析构，导致资源泄漏。

本工具提供了一个自动析构表解决此问题。所有无法在DllMain卸载阶段析构的对象，和交给MATLAB管理的对象，都应当在自动析构表中注册：
```C++
void 自动析构(void* 对象指针, std::move_only_function<void(void*)const>&& 删除器)noexcept;
template<typename T>
inline void 自动析构(T* 对象指针)noexcept;
```
输入指针和可选的删除器（默认使用`delete`，仅适用于`new`创建的指针）。当MEX模块卸载时（包括`clear mex`），每个指针都将被对应的删除器删除。除此之外，如果MATLAB传来了已经被删除或无效的指针，可以用以下方法判断：
```C++
bool 对象存在(void* 对象指针)noexcept;
```
此方法判断指针是否存在于自动析构表中。如果不存在，往往意味着此对象未注册自动析构，或者是无效的指针，通常应当拒绝执行任何操作并返回异常。此外，对于MATLAB主动要求删除的对象，必须从自动析构表中移除：
```C++
bool 手动析构(void* 对象指针)noexcept;
```
此方法尝试从自动析构表中移除指针。如果指针不存在于表中，将返回`false`，如同`对象存在`返回`false`的情形。如果指针存在于表中，将其移除并返回`true`。注意，此方法并不会调用删除器，仅仅是从表中移除。你仍需要手动删除该指针。如果已被你手动删除的指针没有从自动析构表中移除，将导致重复析构。
# FAQ
某些计算机/编译器可能存在中文编码错误问题。你需要设置中文编码为UTF-8。对于 Windows 11，可以在任务栏搜索intl.cpl，转到【管理\非Unicode程序的语言\更改系统区域设置】，勾选【Beta版：使用 Unicode UTF-8 提供全球语言支持】，然后重启计算机。
