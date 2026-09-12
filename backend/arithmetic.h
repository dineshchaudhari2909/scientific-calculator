#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <string>
#include <vector>

using namespace std;

vector<int> toDigits(string s);
string toString(vector<int> digits);
void removeLeadingZeros(vector<int>& digits);

string add(string a, string b);
string subtract(string a, string b);
string multiply(string a, string b);
string divide(string a, string b);
string modulo(string a, string b);
string power(string base, string exponent);
string factorial(string n);
string gcd(string a, string b);
string lcm(string a, string b);

#endif