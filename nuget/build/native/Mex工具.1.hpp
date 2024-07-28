#pragma once
#include <magic_enum.hpp>
#include <mex.hpp>
#ifndef __MEX_IO_ADAPTER_HPP__
//R2024a及以前，此类的定义hpp中还包含了外部链接的函数定义，会导致编译单元之间符号多次定义。所以本工具不在头文件中包含那个头文件，而是将此类定义直接拷贝过来。
namespace matlab {
    namespace mex {
        template <typename iterator_type>
        class MexIORange {

            iterator_type begin_;

            iterator_type end_;

            size_t size_;
        public:

            MexIORange(iterator_type b, iterator_type e, size_t size) : begin_(b), end_(e), size_(size) {}

            size_t size() {
                return size_;
            }

            typename std::iterator_traits<iterator_type>::difference_type internal_size() {
                return std::distance(begin_, end_);
            }

            iterator_type begin() {
                return begin_;
            }

            iterator_type end() {
                return begin_ + size();
            }

            bool empty() {
                return size() == 0;
            }

            typename std::iterator_traits<iterator_type>::reference operator[](size_t i) {
                if (static_cast<int>(i) + 1 > internal_size())
                    throw matlab::engine::Exception("ArgumentList index out of range.");

                return *(begin_ + i);
            }
        };
    }
}
#endif
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