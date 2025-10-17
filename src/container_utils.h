#pragma once
#include <iterator>

template <typename It, typename Comparator>
void QuickSort(It begin, It end, Comparator c)
{
    using std::swap;

    if (begin == end) return;
    if (std::next(begin) == end) return;
    
    typename It::value_type pivot = 
        *std::next(begin, std::distance(begin, end) / 2);

    It less = begin;
    while (c(*less, pivot)) ++less;

    It more = end;
    while (c(pivot, *std::prev(more))) --more;
    
    It equals = less;
    while (equals != more)
    {
        if (c(*equals, pivot)) swap(*less++, *equals++);
        else if (c(pivot, *equals)) swap(*equals, *--more);
        else ++equals;
    }

    QuickSort(begin, less, c);
    QuickSort(more, end, c);
}

// Using the default less comparison
template <typename It>
void QuickSort(It begin, It end)
{
    QuickSort(begin, end, 
        [](const typename It::value_type& v1, 
           const typename It::value_type& v2)
        {
            return v1 < v2;
        });
}

template <typename It, typename Comparator>
It BinarySearch(It begin, It end, 
    const typename It::value_type& val, 
    Comparator c)
{
    It from = begin; It to = end;
    
    while (from != to)
    {
        It pos = std::next(from, std::distance(from, to) / 2);

        if (*pos == val) return pos;
        if (c(*pos, val)) from = ++pos;
        else to = pos;
    }

    return end;
}

// Using the default less comparison
template <typename It>
It BinarySearch(It begin, It end, 
    const typename It::value_type& val)
{
    return BinarySearch(begin, end, val, 
        [](const typename It::value_type& v1, 
           const typename It::value_type& v2)
        {
            return v1 < v2;
        });
}

template <typename It>
It LinearSearch(It begin, It end, 
    const typename It::value_type& val)
{
    while (begin != end)
    {
        if (*begin == val) return begin;
    }

    return begin;
}