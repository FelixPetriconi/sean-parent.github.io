template <typename T>
class correct_cow {
    struct object_t {
        explicit object_t(const T& x) : data_m(x) {}
        atomic<int> count_m{1};
        T           data_m; };
    object_t* object_m;
 public:
    explicit correct_cow(const T& x) : object_m(new object_t(x)) { }
    ~correct_cow() { if (0 == --object_m->count_m) delete object_m; }
    correct_cow(const correct_cow& x) : object_m(x.object_m) { ++object_m->count_m; }

    correct_cow& operator=(const T& x) {
        if (object_m->count_m == 1) object_m->data_m = x;
        else {
            object_t* tmp = new object_t(x);
            if (0 == --object_m->count_m) delete object_m;    // position 3
            object_m = tmp;
        }
        return *this;
    }
};