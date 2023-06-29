#include <iostream>
#include <random>
#include <chrono>
#include <utility>

std::mt19937_64 gen(42);
std::uniform_int_distribution<uint64_t> distrib(0, -1);

const std::size_t TESTS = 5;
const std::size_t N = 100'000;
const std::size_t M = 10'000'000;
const uint64_t MOD = 1e9 + 7;

struct binary_search_sln {
    std::vector<uint64_t> data;

    explicit binary_search_sln(std::vector<uint64_t> &data_) : data(data_) {
    }

    [[nodiscard]] int get(uint64_t x) const {
        return static_cast<int>(std::lower_bound(data.begin(), data.end(), x) - data.begin());
    }
};

struct seg_tree_sln {
    std::vector<uint64_t> data, tree;

    explicit seg_tree_sln(std::vector<uint64_t> data_) : data(std::move(data_)) {
        tree.assign(4 * N, 0);
        build_segtree(0, 0, N - 1);
    }

    void build_segtree(std::size_t v, std::size_t tl, std::size_t tr) {
        if (tl == tr) {
            tree[v] = data[tl];
        } else {
            std::size_t tm = (tl + tr) / 2;
            build_segtree(2 * v + 1, tl, tm);
            build_segtree(2 * v + 2, tm + 1, tr);
            tree[v] = data[tm];
        }
    }

    [[nodiscard]] std::size_t lower_bound(std::size_t v, std::size_t tl, std::size_t tr, uint64_t x) const {
        while (tl != tr) {
            std::size_t tm = (tl + tr) / 2;
            if (x <= tree[v]) {
                tr = tm;
                v = 2 * v + 1;
            } else {
                tl = tm + 1;
                v = 2 * v + 2;
            }
        }
        return tl;
        /*if (tl == tr) {
            return tl;
        } else {
            int tm = (tl + tr) / 2;
            if (x <= tree[v]) {
                return lower_bound(2 * v + 1, tl, tm, tree, x);
            } else {
                return lower_bound(2 * v + 2, tm + 1, tr, tree, x);
            }
        }*/
    }

    [[nodiscard]] std::size_t get(uint64_t x) const {
        if (x > data[N - 1]) {
            return N;
        } else {
            return lower_bound(0, 0, N - 1, x);
        }
    }
};

struct layers_sln {
    // количество слоев
    static inline const std::size_t L = 3;
    // шаг на каждом слое (пропускаем столько элементов)
    static inline const std::size_t P = 51;

    std::vector<std::vector<uint64_t>> layers;

    explicit layers_sln(const std::vector<uint64_t> &data) {
        layers.assign(L, {});
        layers[0] = data;
        for (std::size_t layer = 0; layer + 1 < L; layer++) {
            for (std::size_t index = 0; index < layers[layer].size(); index += P) {
                layers[layer + 1].push_back(layers[layer][index]);
            }
        }
    }

    [[nodiscard]] std::size_t get(uint64_t x) const {
        std::size_t index = 0;
        for (int layer = L - 1; layer >= 0; layer--) {
            index *= P;
            while (index + 1 < layers[layer].size() && layers[layer][index + 1] < x) {
                index++;
            }
        }
        if (index < N && layers[0][index] < x) {
            index++;
        }
        return index;
    }
};

// (data, requests)
std::pair<std::vector<uint64_t>, std::vector<uint64_t>> build_test() {
    std::vector<uint64_t> data(N), requests(M);
    for (std::size_t index = 0; index < N; index++) {
        data[index] = distrib(gen);
    }
    sort(data.begin(), data.end());
    for (std::size_t index = 0; index < M; index++) {
        requests[index] = distrib(gen);
    }
    return {data, requests};
}

template<typename Solution>
uint64_t solve(const Solution &sln, const std::vector<uint64_t> &requests) {
    uint64_t hash = 0;
    for (uint64_t x: requests) {
        std::size_t index = sln.get(x);
        hash *= 13;
        hash += index + 1;
        hash %= MOD;
    }
    return hash;
}

template<typename Solution>
long long test_case() {
    gen = std::mt19937_64(42);
    long long total_time = 0;
    uint64_t total_hash = 0;
    for (std::size_t test = 1; test <= TESTS; test++) {
        auto [data, requests] = build_test();

        Solution sln(data);

        auto time_start = std::chrono::steady_clock::now();

        uint64_t hash = solve(sln, requests);
        total_hash *= 1273;
        total_hash += hash;
        total_hash %= MOD;

        auto time_end = std::chrono::steady_clock::now();
        auto duration = time_end - time_start;
        auto time_ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        std::cout << "test: " << test << " time: " << time_ms.count() << "ms hash: " << hash << '\n';
        total_time += time_ms.count();
    }
    std::cout << "Total time: " << total_time << "ms\n";
    std::cout << "Total hash: " << total_hash << "\n";
    return total_time;
}

int main() {
    std::cout << "binary_search_sln\n";
    test_case<binary_search_sln>();
    std::cout << "seg_tree_sln\n";
    test_case<seg_tree_sln>();
    std::cout << "layers_sln\n";
    test_case<layers_sln>();
}

/*
=====================================

N = 100'000
M = 10'000'000

=====binary search=====
test: 1 time: 815ms hash: 261895180
test: 2 time: 810ms hash: 337713848
test: 3 time: 811ms hash: 831388475
test: 4 time: 811ms hash: 772999321
test: 5 time: 815ms hash: 44180255
Total time: 4062ms

=====seg tree=====
test: 1 time: 740ms hash: 261895180
test: 2 time: 726ms hash: 337713848
test: 3 time: 727ms hash: 831388475
test: 4 time: 725ms hash: 772999321
test: 5 time: 723ms hash: 44180255
Total time: 3641ms

=====layers===== L=3, P=51
test: 1 time: 564ms hash: 261895180
test: 2 time: 556ms hash: 337713848
test: 3 time: 551ms hash: 831388475
test: 4 time: 554ms hash: 772999321
test: 5 time: 552ms hash: 44180255
Total time: 2777ms

=====================================

N = 1'000'000
M = 100'000'000

=====binary search=====
test: 1 time: 13569ms hash: 354736359
test: 2 time: 13884ms hash: 589526183
test: 3 time: 13702ms hash: 359303248
test: 4 time: 13808ms hash: 228408545
test: 5 time: 13516ms hash: 372777787
Total time: 68479ms

=====seg tree=====
test: 1 time: 9952ms hash: 354736359
test: 2 time: 9738ms hash: 589526183
test: 3 time: 9947ms hash: 359303248
test: 4 time: 9888ms hash: 228408545
test: 5 time: 9882ms hash: 372777787
Total time: 49407ms

=====layers===== L=4, P=35
test: 1 time: 9385ms hash: 354736359
test: 2 time: 9305ms hash: 589526183
test: 3 time: 9255ms hash: 359303248
test: 4 time: 9483ms hash: 228408545
test: 5 time: 9399ms hash: 372777787
Total time: 46827ms
 */