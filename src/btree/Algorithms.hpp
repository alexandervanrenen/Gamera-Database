#pragma once

namespace dbi {

template<typename Key, typename I, typename Compare>
I search(I first, I end, Compare& c, const Key& k) {
    I last = end; --last;
    if (c.less(k, *first) || c.less(*last, k)) return end;
    if (c.equal(k, *first)) return first;
    if (c.equal(k, *last)) return last;
    while (true) {
        int diff = last - first;
        if (diff <= 1)
            return end;
        I middle = first+diff/2;
        if (c.less(k, *middle))
            last = middle;
        else if (c.less(*middle, k))
            first = middle;
        else
            return middle;
    }
}


template<typename Key, typename I, typename Compare>
I upper_bound(I first, I end, Compare& c, const Key& k) {
    I last = end; --last;
    //if (c.less(k, *first)) return first;
    if (c.less(*last, k)) return end;
    int step;
    int count = end - first;
    I it;
    while (count>0) {
        it = first; step=count/2; it += step;
        if (!c.less(k, *it)) {
            first=++it; count-=step+1;
        }
        else count=step;
    }
    return first;
}

}
