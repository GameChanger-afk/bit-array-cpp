#include "bitarray.h"
#include <gtest/gtest.h>
#include <string>

// --------------------------
// Конструкторы и деструктор
// --------------------------

TEST(BitArrayTest, DefaultConstructor) {
    BitArray ba;
    EXPECT_EQ(ba.size(), 0);
    EXPECT_TRUE(ba.empty());
}

TEST(BitArrayTest, ConstructorWithValue) {
    BitArray ba(5, 0b10110UL);
    EXPECT_EQ(ba.size(), 5);
    EXPECT_EQ(ba[0], false);
    EXPECT_EQ(ba[1], true);
    EXPECT_EQ(ba[2], true);
    EXPECT_EQ(ba[3], false);
    EXPECT_EQ(ba[4], true);
}

TEST(BitArrayTest, ConstructorZeroSize) {
    BitArray ba(0);
    EXPECT_TRUE(ba.empty());
}

TEST(BitArrayTest, ConstructorNegativeThrows) {
    EXPECT_THROW(BitArray(-1), std::invalid_argument);
}

// --------------------------
// Копирование и присваивание
// --------------------------

TEST(BitArrayTest, CopyConstructor) {
    BitArray ba(6, 0b110011);
    BitArray ba2(ba);
    EXPECT_EQ(ba2, ba);
}

TEST(BitArrayTest, AssignmentOperator) {
    BitArray ba(4, 0b1101);
    BitArray ba2;
    ba2 = ba;
    EXPECT_EQ(ba2, ba);
}

TEST(BitArrayTest, SelfAssignment) {
    BitArray ba(3, 0b101);
    ba = ba;
    EXPECT_EQ(ba.to_string(), "101");
}

// --------------------------
// Изменение размера и содержимого
// --------------------------

TEST(BitArrayTest, ResizeGrowWithFalse) {
    BitArray ba(2, 0b11);
    ba.resize(5, false);
    EXPECT_EQ(ba.to_string(), "00011");
}

TEST(BitArrayTest, ResizeGrowWithTrue) {
    BitArray ba(2, 0b01);
    ba.resize(5, true);
    EXPECT_EQ(ba.to_string(), "11101");
}

TEST(BitArrayTest, ResizeShrink) {
    BitArray ba(6, 0b111100);
    ba.resize(3);
    EXPECT_EQ(ba.to_string(), "100"); // only lowest 3 bits
}

TEST(BitArrayTest, Clear) {
    BitArray ba(5, 0b11111);
    ba.clear();
    EXPECT_TRUE(ba.empty());
}

TEST(BitArrayTest, PushBack) {
    BitArray ba;
    ba.push_back(true);
    ba.push_back(false);
    ba.push_back(true);
    EXPECT_EQ(ba.to_string(), "101");
}

// --------------------------
// Установка и сброс битов
// --------------------------

TEST(BitArrayTest, SetSingleBit) {
    BitArray ba(4);
    ba.set(2, true);
    EXPECT_EQ(ba.to_string(), "0100");
    ba.set(2, false);
    EXPECT_EQ(ba.to_string(), "0000");
}

TEST(BitArrayTest, SetAllBits) {
    BitArray ba(5);
    ba.set();
    EXPECT_EQ(ba.to_string(), "11111");
}

TEST(BitArrayTest, ResetAllBits) {
    BitArray ba(4, 0b1111);
    ba.reset();
    EXPECT_EQ(ba.to_string(), "0000");
}

TEST(BitArrayTest, ResetSingleBit) {
    BitArray ba(3, 0b111);
    ba.reset(1);
    EXPECT_EQ(ba.to_string(), "101");
}

// --------------------------
// Битовые операции
// --------------------------

TEST(BitArrayTest, BitwiseAND) {
    BitArray a(4, 0b1100);
    BitArray b(4, 0b1010);
    BitArray c = a & b;
    EXPECT_EQ(c.to_string(), "1000");
    a &= b;
    EXPECT_EQ(a.to_string(), "1000");
}

TEST(BitArrayTest, BitwiseOR) {
    BitArray a(3, 0b101);
    BitArray b(3, 0b011);
    EXPECT_EQ((a | b).to_string(), "111");
    a |= b;
    EXPECT_EQ(a.to_string(), "111");
}

TEST(BitArrayTest, BitwiseXOR) {
    BitArray a(4, 0b1100);
    BitArray b(4, 0b1010);
    EXPECT_EQ((a ^ b).to_string(), "0110");
}

TEST(BitArrayTest, NOT) {
    BitArray a(4, 0b1010);
    BitArray b = ~a;
    EXPECT_EQ(b.to_string(), "0101");
}

// --------------------------
// Сдвиги
// --------------------------

TEST(BitArrayTest, LeftShift) {
    BitArray a(5, 0b10011);
    BitArray b = a << 2;
    EXPECT_EQ(b.to_string(), "01100");
    a <<= 2;
    EXPECT_EQ(a.to_string(), "01100");
}

TEST(BitArrayTest, RightShift) {
    BitArray a(5, 0b10011);
    BitArray b = a >> 2;
    EXPECT_EQ(b.to_string(), "00100");
    a >>= 2;
    EXPECT_EQ(a.to_string(), "00100");
}

// --------------------------
// Запросы состояния
// --------------------------

TEST(BitArrayTest, AnyAndNone) {
    BitArray a(3);
    EXPECT_FALSE(a.any());
    EXPECT_TRUE(a.none());

    a.set(1);
    EXPECT_TRUE(a.any());
    EXPECT_FALSE(a.none());
}

TEST(BitArrayTest, Count) {
    BitArray a(6, 0b101101);
    EXPECT_EQ(a.count(), 4);
}

TEST(BitArrayTest, OperatorBracket) {
    BitArray a(4, 0b1101);
    EXPECT_EQ(a[0], true);
    EXPECT_EQ(a[1], false);
    EXPECT_EQ(a[2], true);
    EXPECT_EQ(a[3], true);
}

TEST(BitArrayTest, SizeAndEmpty) {
    BitArray a;
    EXPECT_EQ(a.size(), 0);
    EXPECT_TRUE(a.empty());

    a.resize(10);
    EXPECT_EQ(a.size(), 10);
    EXPECT_FALSE(a.empty());
}

// --------------------------
// Преобразование в строку
// --------------------------

TEST(BitArrayTest, ToString) {
    BitArray a(5, 0b11001);
    EXPECT_EQ(a.to_string(), "11001");
}

// --------------------------
// Операторы сравнения
// --------------------------

TEST(BitArrayTest, Equality) {
    BitArray a(4, 0b1010);
    BitArray b(4, 0b1010);
    BitArray c(4, 0b0101);
    EXPECT_TRUE(a == b);
    EXPECT_FALSE(a == c);
    EXPECT_TRUE(a != c);
}

// --------------------------
// Границы и исключения
// --------------------------

TEST(BitArrayTest, OutOfRangeAccessThrows) {
    BitArray a(3);
    EXPECT_THROW(a[3], std::out_of_range);
    EXPECT_THROW(a[-1], std::out_of_range);
}

TEST(BitArrayTest, BitwiseMismatchSizeThrows) {
    BitArray a(3);
    BitArray b(4);
    EXPECT_THROW(a &= b, std::invalid_argument);
    EXPECT_THROW(a |= b, std::invalid_argument);
    EXPECT_THROW(a ^= b, std::invalid_argument);
}