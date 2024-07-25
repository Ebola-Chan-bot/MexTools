#pragma once
namespace Mex工具::内部
{
	template<typename 字符类型, typename 枚举类型>
	void 枚举转标识符(std::basic_ostream<字符类型>& 输出流, 枚举类型 枚举)
	{
		//typeid().name前5个字符是"enum "，之后是枚举类名
		for (const char* 字符指针 = typeid(枚举类型).name() + 5; const char 字符 = *字符指针; 字符指针 += 字符 == ':' ? 2 : 1)
			输出流 << 字符;
		输出流 << ':' << magic_enum::enum_name(枚举);
	}
}