template <typename I> // I models RandomAccessIterator
void sort_subrange(I f, I l, I sf, I sl) {
    if (sf == sl) return;
    if (sf != f) {
        std::nth_element(f, sf, l);
        ++sf;
    }
    std::partial_sort(sf, sl, l);
}