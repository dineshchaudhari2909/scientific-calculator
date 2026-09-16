# 🧮 Scientific Calculator — Arbitrary Precision C++ Engine

A high-performance **Scientific Calculator** built with a custom C++ backend designed to handle **very large numbers** beyond the limits of standard integer data types.

The project was inspired by the classic DSA problem **“Factorials of Large Numbers”**, where the result must be represented as a list of digits because the factorial can quickly exceed the range of built-in numeric types.

Instead of relying on `BigInteger` or external big-number libraries, this project implements arithmetic using **strings and `vector<int>`**.

---

## 🚀 Project Highlights

* 🔢 Arbitrary-precision integer arithmetic
* ⚡ C++ backend exposed through a REST API
* 🌐 HTML/CSS/JavaScript frontend
* 🧮 Scientific calculator operations
* 📦 Custom `string` / `vector<int>` number representation
* 🚫 No `BigInteger`
* 🚫 No GMP / Boost multiprecision
* 🚫 No external big-number libraries
* 🎯 Current version can accurately produce results of **up to 2,568 digits**
* 🔬 Designed for future high-performance arbitrary-precision algorithms

---

## 💡 Inspiration

The project started after encountering the DSA problem:

### Factorials of Large Numbers

Given an integer `n`, find its factorial and return the digits that make up the factorial.

Example:

```text
Input:
n = 5

Output:
[1, 2, 0]

Explanation:
5! = 1 × 2 × 3 × 4 × 5
   = 120
```

The problem raised an interesting question:

> **How can we perform arithmetic when the result becomes much larger than the limits of standard integer types?**

That question led to the development of this calculator.

---

## 🏗️ Architecture

```text
                    ┌─────────────────────────┐
                    │       Web Browser       │
                    │                         │
                    │  HTML + CSS + JavaScript│
                    └────────────┬────────────┘
                                 │
                                 │ HTTP REST API
                                 ▼
                    ┌─────────────────────────┐
                    │      C++ Backend        │
                    │                         │
                    │     REST API Server      │
                    └────────────┬────────────┘
                                 │
                                 ▼
                    ┌─────────────────────────┐
                    │  Arbitrary Precision    │
                    │       Engine            │
                    │                         │
                    │ string / vector<int>    │
                    │                         │
                    │ + Addition              │
                    │ + Subtraction           │
                    │ + Multiplication        │
                    │ + Division              │
                    │ + Modulo                │
                    │ + Power                 │
                    └─────────────────────────┘
```

---

## 🛠️ Tech Stack

### Frontend

* HTML5
* CSS3
* JavaScript

### Backend

* C++
* C++17
* REST API

### Compiler

```text
GCC 11.4.0
```

### Core Data Structures

```text
string
vector<int>
```

---

## 📁 Project Structure

```text
scientific-calculator/
│
├── backend/
│   ├── main.cpp
│   ├── arithmetic.h
│   └── arithmetic.cpp
│
└── frontend/
    ├── index.html
    ├── style.css
    └── script.js
```

---

# 🔢 Arbitrary Precision Arithmetic

The main goal of the backend is to avoid depending on fixed-size integer types for large-number calculations.

For example, instead of storing:

```text
123456789012345678901234567890
```

inside a built-in integer type, the number can be represented using:

```text
vector<int>
```

or:

```text
string
```

This allows the arithmetic engine to work with numbers containing thousands or potentially millions of digits, subject to memory and algorithmic limits.

---

# ⚙️ Current Operations

The calculator is being developed around a custom arbitrary-precision arithmetic engine.

| Operation      | Current / Planned Algorithm         |
| -------------- | ----------------------------------- |
| Comparison     | Length + digit/chunk comparison     |
| Addition       | Digit/chunk addition with carry     |
| Subtraction    | Digit/chunk subtraction with borrow |
| Multiplication | Fast multiplication algorithms      |
| Division       | Advanced division engine            |
| Modulo         | Division/remainder engine           |
| Power          | Exponentiation by squaring          |
| GCD            | Fast GCD algorithms                 |
| LCM            | `a / gcd(a,b) × b`                  |
| Factorial      | Large-number factorial algorithms   |

---

# 🚀 Advanced Algorithm Roadmap

The next stage of the project focuses on improving the performance of operations involving extremely large operands.

## 1. Division

Planned algorithms:

```text
Newton Reciprocal
        +
Fast Multiplication
        +
Burnikel-Ziegler Division
```

These algorithms are intended for very large operands where traditional long division becomes inefficient.

---

## 2. Modulo

Modulo will reuse the division engine:

```text
a % b
```

Conceptually:

```text
Fast Division
      ↓
Remainder
      ↓
Modulo
```

---

## 3. Power

For:

```text
a^n
```

the calculator will use:

### Exponentiation by Squaring

Instead of performing `n` multiplications:

```text
a × a × a × a × ...
```

the algorithm reduces the number of multiplications to approximately:

```text
O(log n)
```

multiplications.

Combined with fast multiplication, this allows very large powers to be calculated much more efficiently.

---

## 4. GCD

Planned algorithm:

### Half-GCD

The goal is to support GCD calculations efficiently for extremely large integers.

```text
gcd(a, b)
```

will eventually use advanced divide-and-conquer techniques rather than only the basic Euclidean algorithm.

---

## 5. LCM

LCM can be calculated using:

```text
LCM(a,b) = |a / GCD(a,b)| × |b|
```

The implementation can therefore reuse the optimized GCD and division engines.

---

# 📊 Current Capability

The current version has been tested with large-number calculations and can accurately produce results containing **up to 2,568 digits**.

For example:

```text
1000!
```

contains:

```text
2,568 digits
```

This makes factorial calculations a useful benchmark for the arbitrary-precision engine.

---

# 🔬 Why Build This Instead of Using BigInteger?

Libraries such as BigInteger and GMP already solve arbitrary-precision arithmetic.

However, the purpose of this project is different.

The objective is to understand and implement the algorithms behind large-number computation:

```text
Number Representation
        ↓
Additio
```
