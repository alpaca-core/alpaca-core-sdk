#include <vector>

struct throw_ex {
    [[noreturn]] ~throw_ex() noexcept(false);
};

using T = std::vector<int>;

T make_vec(T& rc);

// build error: C4715
T bad_warning(int a) {
    T x(a + 5);
    if (a == 0) return make_vec(x);
    throw_ex{};
}
