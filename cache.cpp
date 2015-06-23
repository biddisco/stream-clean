#include <stdio.h>
#include <stdlib.h>

#include <chrono>
#include <iostream>
#include <sstream>
#include <vector>

using clock_type    = std::chrono::high_resolution_clock;
using duration_type = std::chrono::duration<double>;

using value_type = double;

void do_work(value_type* v, size_t n) {
    size_t i;
    __assume_aligned(v, 64);
    __assume(n%16 == 0);

    for(i=0; i<n; ++i) {
        v[i] += 1.2;
    }
}

std::vector<size_t> generate_range(size_t l, size_t u) {
    std::vector<size_t> range;
    auto i = l;
    while( i<u ) {
        auto next = 2*i;
        auto step = (next-i)/4;
        while(i<next) {
            range.push_back(i);
            i += step;
        }
    }

    return range;
}

std::string to_string(size_t n) {
    if(n<1024) {
        std::stringstream s;
        s << n << "  B";
        return s.str();
    }
    else if(n<1024*1024) {
        std::stringstream s;
        s << n/1024 << " kB";
        return s.str();
    }
    else {
        std::stringstream s;
        s << n/(1024*1024) << " MB";
        return s.str();
    }
    return "";
}

int main(int argc, char** argv) {
    // make buffer large enough for 64 MB (2^6)
    const auto buffer_len = 1024/sizeof(value_type)*1024*(1<<6);
    const auto buffer_size = sizeof(value_type)*buffer_len;

    value_type *buffer;
    posix_memalign((void**)&buffer, 64, buffer_size);
    std::fill(buffer, buffer+buffer_len, value_type{0});

    std::vector<double> times;
    std::vector<size_t> bytes;

    auto onek  = 1024ul/sizeof(value_type);
    auto onem  = onek * 1024ul;
    auto upper = buffer_len;
    auto sizes = generate_range(256/sizeof(value_type), upper);

    for(auto n: sizes) {
        auto num_iters = buffer_len * 16 / n;
        do_work(buffer, n);
        auto start = clock_type::now();
        for(auto i=0; i<num_iters; ++i) {
            do_work(buffer, n);
        }
        auto time_taken = duration_type(clock_type::now()-start).count();
        bytes.push_back(num_iters*n*sizeof(value_type));
        times.push_back(time_taken);
    }
    printf("%15s%15s%15s\n", "bytes", "human", "bandwidth GB/s");
    for(auto i=0; i<times.size(); ++i) {
        auto b = sizeof(value_type)*sizes[i];
        printf("%15ld%15s%15.2f\n",
                b,
                to_string(b).c_str(),
                bytes[i]/(float)times[i]*1e-9f);
    }

    free(buffer);
}

