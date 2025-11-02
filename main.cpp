#include "bitarray.h"
#include <iostream>

int main() {
    BitArray a(10, 0b1010101010UL);
    std::cout << "a: " << a.to_string() << " (size=" << a.size() << ")\n";

    BitArray b = a;
    b <<= 2;
    std::cout << "a << 2: " << b.to_string() << "\n";

    BitArray c(10);
    c.set(0).set(2).set(4);
    std::cout << "c: " << c.to_string() << "\n";

    BitArray d = a & c;
    std::cout << "a & c: " << d.to_string() << "\n";

    std::cout << "count(a): " << a.count() << "\n";
    std::cout << "any(c): " << c.any() << "\n";
    std::cout << "none(empty): " << BitArray().none() << "\n";

    return 0;
}