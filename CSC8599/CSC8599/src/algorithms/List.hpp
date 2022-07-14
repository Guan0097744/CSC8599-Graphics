#ifndef LIST_HPP
#define LIST_HPP

namespace List 
{
	template<typename T>
	int GetIndexOf(std::vector<T> v, T x)
	{
		return std::find(v.begin(), v.end(), x) - v.begin();
	}

	template<typename T>
	bool Contains(std::vector<T> v, T x) 
	{
		return std::find(v.begin(), v.end(), x) != v.end();
	}
}

#endif // !LIST_HPP
