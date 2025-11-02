#include "bitarray.h"
#include <cstring>
#include <stdexcept>
#include <string>
#include <climits>

const int BITS_PER_WORD = sizeof(unsigned long) * CHAR_BIT;

BitArray::BitArray()
    : data(nullptr), num_bits(0), capacity(0)
{
}

BitArray::~BitArray()
{
    deallocate();
}

BitArray::BitArray(int num_bits, unsigned long value)
    : data(nullptr), num_bits(0), capacity(0)
{
    if (num_bits < 0)
        throw std::invalid_argument("num_bits must be non-negative");
    allocate(num_bits);
    if (num_bits > 0) {
        int init_bits = (num_bits < BITS_PER_WORD) ? num_bits : BITS_PER_WORD;
        unsigned long mask = (init_bits == BITS_PER_WORD) ? ~0UL : ((1UL << init_bits) - 1);
        data[0] = value & mask;   
    }
}

BitArray::BitArray(const BitArray& b)
    : data(nullptr), num_bits(0), capacity(0)
{
    allocate(b.num_bits);
    if (b.num_bits > 0) {
        std::memcpy(data, b.data, words_needed(b.num_bits) * sizeof(unsigned long));
    }
}

void BitArray::swap(BitArray& b)
{
    unsigned long* tmp_data = data;
    int tmp_bits = num_bits;
    int tmp_cap = capacity;

    data = b.data;
    num_bits = b.num_bits;
    capacity = b.capacity;

    b.data = tmp_data;
    b.num_bits = tmp_bits;
    b.capacity = tmp_cap;
}

BitArray& BitArray::operator=(const BitArray& b)
{
    if (this != &b) {
        BitArray tmp(b);
        swap(tmp);
    }
    return *this;
}

void BitArray::allocate(int bits)
{
    if (bits < 0)
        throw std::invalid_argument("bits must be non-negative");
    int words = words_needed(bits);
    data = new unsigned long[words]();
    num_bits = bits;
    capacity = words * BITS_PER_WORD;
}

void BitArray::deallocate()
{
    delete[] data;
    data = nullptr;
    num_bits = 0;
    capacity = 0;
}

int BitArray::words_needed(int bits) const
{
    return (bits + BITS_PER_WORD - 1) / BITS_PER_WORD;
}

void BitArray::resize(int num_bits, bool value)
{
    if (num_bits < 0)
        throw std::invalid_argument("num_bits must be non-negative");
    if (num_bits == this->num_bits)
        return;

    // Сохраняем старое состояние
    int old_size = this->num_bits;
    unsigned long* old_data = data;
    int old_words = words_needed(old_size);

    // Создаём новый массив (заполнен нулями)
    allocate(num_bits);

    // Копируем старые данные
    if (old_size > 0) {
        int words_to_copy = std::min(old_words, words_needed(num_bits));
        std::memcpy(data, old_data, words_to_copy * sizeof(unsigned long));
        // Очищаем возможный "мусор" в последнем слове при уменьшении
        if (num_bits < old_size) {
            int excess = capacity - num_bits;
            if (excess > 0) {
                unsigned long mask = (1UL << (BITS_PER_WORD - (excess % BITS_PER_WORD))) - 1;
                if (excess % BITS_PER_WORD == 0) mask = ~0UL;
                data[words_needed(num_bits) - 1] &= mask;
            }
        }
    }

    // Если увеличиваем и нужно заполнить единицами — ставим 1 в новые позиции
    if (value && num_bits > old_size) {
        for (int i = old_size; i < num_bits; ++i) {
            set(i, true);
        }
    }

    delete[] old_data;
}

void BitArray::clear()
{
    deallocate();
}

void BitArray::push_back(bool bit)
{
    resize(num_bits + 1, false);
    if (bit)
        set(num_bits - 1, true);
}

BitArray& BitArray::operator&=(const BitArray& b)
{
    if (num_bits != b.num_bits)
        throw std::invalid_argument("BitArray sizes must match for bitwise operations");
    int words = words_needed(num_bits);
    for (int i = 0; i < words; ++i)
        data[i] &= b.data[i];
    return *this;
}

BitArray& BitArray::operator|=(const BitArray& b)
{
    if (num_bits != b.num_bits)
        throw std::invalid_argument("BitArray sizes must match for bitwise operations");
    int words = words_needed(num_bits);
    for (int i = 0; i < words; ++i)
        data[i] |= b.data[i];
    return *this;
}

BitArray& BitArray::operator^=(const BitArray& b)
{
    if (num_bits != b.num_bits)
        throw std::invalid_argument("BitArray sizes must match for bitwise operations");
    int words = words_needed(num_bits);
    for (int i = 0; i < words; ++i)
        data[i] ^= b.data[i];
    return *this;
}

BitArray& BitArray::operator<<=(int n)
{
    if (n < 0) return *this;
    if (n >= num_bits) {
        std::memset(data, 0, words_needed(num_bits) * sizeof(unsigned long));
        return *this;
    }
    if (n == 0) return *this;

    int word_shift = n / BITS_PER_WORD;
    int bit_shift = n % BITS_PER_WORD;

    int total_words = words_needed(num_bits);
    // Shift words
    for (int i = total_words - 1; i >= word_shift; --i) {
        data[i] = data[i - word_shift] << bit_shift;
        if (bit_shift != 0 && i - word_shift - 1 >= 0)
            data[i] |= data[i - word_shift - 1] >> (BITS_PER_WORD - bit_shift);
    }
    // Zero out lower words
    for (int i = 0; i < word_shift; ++i)
        data[i] = 0;

    // Clear bits beyond size
    int excess = total_words * BITS_PER_WORD - num_bits;
    if (excess > 0) {
        unsigned long mask = (1UL << (BITS_PER_WORD - excess)) - 1;
        data[total_words - 1] &= mask;
    }

    return *this;
}

BitArray& BitArray::operator>>=(int n)
{
    if (n < 0) return *this;
    if (n >= num_bits) {
        std::memset(data, 0, words_needed(num_bits) * sizeof(unsigned long));
        return *this;
    }
    if (n == 0) return *this;

    int word_shift = n / BITS_PER_WORD;
    int bit_shift = n % BITS_PER_WORD;

    int total_words = words_needed(num_bits);
    // Shift words
    for (int i = 0; i < total_words - word_shift; ++i) {
        data[i] = data[i + word_shift] >> bit_shift;
        if (bit_shift != 0 && i + word_shift + 1 < total_words)
            data[i] |= data[i + word_shift + 1] << (BITS_PER_WORD - bit_shift);
    }
    // Zero out upper words
    for (int i = total_words - word_shift; i < total_words; ++i)
        data[i] = 0;

    return *this;
}

BitArray BitArray::operator<<(int n) const
{
    BitArray tmp(*this);
    tmp <<= n;
    return tmp;
}

BitArray BitArray::operator>>(int n) const
{
    BitArray tmp(*this);
    tmp >>= n;
    return tmp;
}

BitArray& BitArray::set(int n, bool val)
{
    if (n < 0 || n >= num_bits)
        throw std::out_of_range("Bit index out of range");
    int word = n / BITS_PER_WORD;
    int bit = n % BITS_PER_WORD;
    if (val)
        data[word] |= (1UL << bit);
    else
        data[word] &= ~(1UL << bit);
    return *this;
}

BitArray& BitArray::set()
{
    std::memset(data, 0xFF, words_needed(num_bits) * sizeof(unsigned long));
    // Clear excess bits
    int excess = capacity - num_bits;
    if (excess > 0) {
        unsigned long mask = (1UL << (BITS_PER_WORD - excess % BITS_PER_WORD)) - 1;
        if (excess % BITS_PER_WORD == 0)
            mask = ~0UL;
        else
            mask = (1UL << (BITS_PER_WORD - (excess % BITS_PER_WORD))) - 1;
        data[words_needed(num_bits) - 1] &= mask;
    }
    return *this;
}

BitArray& BitArray::reset(int n)
{
    return set(n, false);
}

BitArray& BitArray::reset()
{
    std::memset(data, 0, words_needed(num_bits) * sizeof(unsigned long));
    return *this;
}

bool BitArray::any() const
{
    int words = words_needed(num_bits);
    for (int i = 0; i < words; ++i)
        if (data[i] != 0)
            return true;
    return false;
}

bool BitArray::none() const
{
    return !any();
}

BitArray BitArray::operator~() const
{
    BitArray tmp(*this);
    int words = words_needed(num_bits);
    for (int i = 0; i < words; ++i)
        tmp.data[i] = ~data[i];
    // Clear excess bits
    int excess = tmp.capacity - tmp.num_bits;
    if (excess > 0) {
        int last_word = words - 1;
        unsigned long mask = (1UL << (BITS_PER_WORD - (excess % BITS_PER_WORD))) - 1;
        if (excess % BITS_PER_WORD == 0)
            mask = ~0UL;
        else
            mask = (1UL << (BITS_PER_WORD - (excess % BITS_PER_WORD))) - 1;
        tmp.data[last_word] &= mask;
    }
    return tmp;
}

int BitArray::count() const {
    if (num_bits == 0) return 0;

    int cnt = 0;
    int words = words_needed(num_bits);
    int full_words = num_bits / BITS_PER_WORD;
    int last_bits = num_bits % BITS_PER_WORD;

    for (int i = 0; i < full_words; ++i) {
        unsigned long w = data[i];
        while (w) {
            cnt += w & 1;
            w >>= 1;
        }
    }

    if (last_bits > 0) {
        unsigned long w = data[full_words];
        for (int i = 0; i < last_bits; ++i) {
            cnt += (w >> i) & 1;
        }
    }

    return cnt;
}

bool BitArray::operator[](int i) const
{
    if (i < 0 || i >= num_bits)
        throw std::out_of_range("Bit index out of range");
    int word = i / BITS_PER_WORD;
    int bit = i % BITS_PER_WORD;
    return (data[word] >> bit) & 1;
}

int BitArray::size() const
{
    return num_bits;
}

bool BitArray::empty() const
{
    return num_bits == 0;
}

std::string BitArray::to_string() const
{
    std::string s;
    s.reserve(num_bits);
    for (int i = num_bits - 1; i >= 0; --i) {
        s += ((*this)[i] ? '1' : '0');
    }
    return s;
}

// Free functions

bool operator==(const BitArray& a, const BitArray& b)
{
    if (a.size() != b.size())
        return false;
    int words = a.words_needed(a.size());
    for (int i = 0; i < words; ++i)
        if (a.data[i] != b.data[i])
            return false;
    return true;
}

bool operator!=(const BitArray& a, const BitArray& b)
{
    return !(a == b);
}

BitArray operator&(const BitArray& b1, const BitArray& b2)
{
    BitArray tmp(b1);
    tmp &= b2;
    return tmp;
}

BitArray operator|(const BitArray& b1, const BitArray& b2)
{
    BitArray tmp(b1);
    tmp |= b2;
    return tmp;
}

BitArray operator^(const BitArray& b1, const BitArray& b2)
{
    BitArray tmp(b1);
    tmp ^= b2;
    return tmp;
}