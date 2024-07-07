export module A;
export template<typename T>
struct B;
template<>
struct B<int>
{
	using type = int;
};