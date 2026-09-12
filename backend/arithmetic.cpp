#include "arithmetic.h"

#include <algorithm>
#include <stdexcept>
#include <cctype>

using namespace std;

static bool isNegative(const string& s)
{
    return !s.empty() && s[0] == '-';
}

static string stripSign(const string& s)
{
    if (isNegative(s))
    {
        return s.substr(1);
    }
    return s;
}

static string applySign(bool negative, const string& magnitude)
{
    if (negative && magnitude != "0")
    {
        return "-" + magnitude;
    }
    return magnitude;
}

static int compareDigits(const vector<int>& a, const vector<int>& b)
{
    if (a.size() != b.size())
    {
        return a.size() < b.size() ? -1 : 1;
    }

    for (int i = static_cast<int>(a.size()) - 1; i >= 0; --i)
    {
        if (a[i] != b[i])
        {
            return a[i] < b[i] ? -1 : 1;
        }
    }

    return 0;
}

vector<int> toDigits(string s)
{
    string stripped = stripSign(s);

    if (stripped.empty())
    {
        throw invalid_argument("Invalid number");
    }

    vector<int> digits;

    for (int i = static_cast<int>(stripped.size()) - 1; i >= 0; --i)
    {
        if (!isdigit(static_cast<unsigned char>(stripped[i])))
        {
            throw invalid_argument("Invalid number");
        }

        digits.push_back(stripped[i] - '0');
    }

    removeLeadingZeros(digits);

    return digits;
}

string toString(vector<int> digits)
{
    removeLeadingZeros(digits);

    string result;

    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i)
    {
        result += static_cast<char>('0' + digits[i]);
    }

    return result;
}

void removeLeadingZeros(vector<int>& digits)
{
    while (digits.size() > 1 && digits.back() == 0)
    {
        digits.pop_back();
    }

    if (digits.empty())
    {
        digits.push_back(0);
    }
}

string add(string a, string b)
{
    bool aNegative = isNegative(a);
    bool bNegative = isNegative(b);

    string aMag = stripSign(a);
    string bMag = stripSign(b);

    if (aNegative == bNegative)
    {
        vector<int> x = toDigits(aMag);
        vector<int> y = toDigits(bMag);

        vector<int> result;

        size_t maxSize = max(x.size(), y.size());

        int carry = 0;

        for (size_t i = 0; i < maxSize; ++i)
        {
            int digitA = (i < x.size()) ? x[i] : 0;
            int digitB = (i < y.size()) ? y[i] : 0;

            int sum = digitA + digitB + carry;

            result.push_back(sum % 10);

            carry = sum / 10;
        }

        if (carry > 0)
        {
            result.push_back(carry);
        }

        return applySign(aNegative, toString(result));
    }
    else
    {
        if (aNegative)
        {
            return subtract(bMag, aMag);
        }
        else
        {
            return subtract(aMag, bMag);
        }
    }
}

static vector<int> subtractDigits(
    const vector<int>& a,
    const vector<int>& b)
{
    vector<int> result;
    int borrow = 0;

    for (size_t i = 0; i < a.size(); ++i)
    {
        int digitA = a[i];
        int digitB = (i < b.size()) ? b[i] : 0;

        int diff = digitA - digitB - borrow;

        if (diff < 0)
        {
            diff += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        result.push_back(diff);
    }

    removeLeadingZeros(result);

    return result;
}

string subtract(string a, string b)
{
    bool aNegative = isNegative(a);
    bool bNegative = isNegative(b);

    string aMag = stripSign(a);
    string bMag = stripSign(b);

    if (aNegative == bNegative)
    {
        vector<int> x = toDigits(aMag);
        vector<int> y = toDigits(bMag);

        int cmp = compareDigits(x, y);

        if (cmp == 0)
        {
            return "0";
        }

        bool resultNegative = (cmp < 0);
        if (resultNegative)
        {
            swap(x, y);
        }

        vector<int> result = subtractDigits(x, y);

        return applySign(aNegative ^ resultNegative, toString(result));
    }
    else
    {
        if (aNegative)
        {
            return applySign(true, add(aMag, bMag));
        }
        else
        {
            return applySign(false, add(aMag, bMag));
        }
    }
}

static vector<int> multiplyDigits(
    const vector<int>& a,
    const vector<int>& b)
{
    vector<int> result(a.size() + b.size(), 0);

    for (size_t i = 0; i < a.size(); ++i)
    {
        int carry = 0;

        for (size_t j = 0; j < b.size(); ++j)
        {
            int value =
                result[i + j]
                + a[i] * b[j]
                + carry;

            result[i + j] = value % 10;
            carry = value / 10;
        }

        size_t position = i + b.size();

        while (carry > 0)
        {
            int value = result[position] + carry;

            result[position] = value % 10;
            carry = value / 10;

            ++position;

            if (position == result.size() && carry > 0)
            {
                result.push_back(0);
            }
        }
    }

    removeLeadingZeros(result);

    return result;
}

static void increment(vector<int>& digits)
{
    int carry = 1;

    for (size_t i = 0; i < digits.size() && carry; ++i)
    {
        int value = digits[i] + carry;

        digits[i] = value % 10;
        carry = value / 10;
    }

    if (carry)
    {
        digits.push_back(carry);
    }
}

static void decrement(vector<int>& digits)
{
    if (digits.size() == 1 && digits[0] == 0)
    {
        return;
    }

    int borrow = 1;

    for (size_t i = 0; i < digits.size() && borrow; ++i)
    {
        int value = digits[i] - borrow;

        if (value < 0)
        {
            value += 10;
            borrow = 1;
        }
        else
        {
            borrow = 0;
        }

        digits[i] = value;
    }

    removeLeadingZeros(digits);
}

static bool isGreaterThanOne(const vector<int>& digits)
{
    return digits.size() > 1 ||
           (digits.size() == 1 && digits[0] > 1);
}

static bool isOdd(const vector<int>& digits)
{
    return (digits[0] % 2) == 1;
}

string multiply(string a, string b)
{
    bool aNegative = isNegative(a);
    bool bNegative = isNegative(b);

    string aMag = stripSign(a);
    string bMag = stripSign(b);

    vector<int> x = toDigits(aMag);
    vector<int> y = toDigits(bMag);

    if (x.size() == 1 && x[0] == 0 ||
        y.size() == 1 && y[0] == 0)
    {
        return "0";
    }

    vector<int> result = multiplyDigits(x, y);

    return applySign(aNegative ^ bNegative, toString(result));
}

string factorial(string n)
{
    if (isNegative(n))
    {
        throw invalid_argument("Factorial requires a non-negative integer");
    }

    vector<int> number = toDigits(n);

    if (number.size() == 1 &&
        number[0] == 0)
    {
        return "1";
    }

    if (number.size() == 1 &&
        number[0] == 1)
    {
        return "1";
    }

    vector<int> counter;
    counter.push_back(2);

    vector<int> result;
    result.push_back(1);

    while (true)
    {
        if (counter.size() > number.size())
        {
            break;
        }

        if (counter.size() == number.size())
        {
            bool greater = false;

            for (int i = static_cast<int>(counter.size()) - 1;
                 i >= 0;
                 --i)
            {
                if (counter[i] > number[i])
                {
                    greater = true;
                    break;
                }

                if (counter[i] < number[i])
                {
                    break;
                }
            }

            if (greater)
            {
                break;
            }
        }

        result = multiplyDigits(result, counter);

        increment(counter);
    }

    return toString(result);
}

static vector<int> divideDigits(
    const vector<int>& dividend,
    const vector<int>& divisor,
    vector<int>& remainder)
{
    if (divisor.size() == 1 && divisor[0] == 0)
    {
        throw invalid_argument("Cannot divide by zero");
    }

    vector<int> quotient;
    remainder.clear();

    int cmp = compareDigits(dividend, divisor);

    if (cmp < 0)
    {
        remainder = dividend;
        removeLeadingZeros(remainder);
        quotient.push_back(0);
        return quotient;
    }

    if (cmp == 0)
    {
        remainder.push_back(0);
        quotient.push_back(1);
        return quotient;
    }

    vector<int> current;

    for (int i = static_cast<int>(dividend.size()) - 1; i >= 0; --i)
    {
        current.insert(current.begin(), dividend[i]);

        removeLeadingZeros(current);

        if (compareDigits(current, divisor) < 0)
        {
            if (!quotient.empty())
            {
                quotient.insert(quotient.begin(), 0);
            }
            continue;
        }

        int low = 0;
        int high = 9;
        int bestDigit = 0;

        while (low <= high)
        {
            int mid = (low + high) / 2;

            vector<int> tempDivisor = divisor;
            for (size_t j = 0; j < tempDivisor.size(); ++j)
            {
                tempDivisor[j] *= mid;
            }

            int carry = 0;
            for (size_t j = 0; j < tempDivisor.size(); ++j)
            {
                int val = tempDivisor[j] + carry;
                tempDivisor[j] = val % 10;
                carry = val / 10;
            }

            while (carry > 0)
            {
                tempDivisor.push_back(carry % 10);
                carry /= 10;
            }

            removeLeadingZeros(tempDivisor);

            if (compareDigits(tempDivisor, current) <= 0)
            {
                bestDigit = mid;
                low = mid + 1;
            }
            else
            {
                high = mid - 1;
            }
        }

        vector<int> tempDivisor = divisor;
        for (size_t j = 0; j < tempDivisor.size(); ++j)
        {
            tempDivisor[j] *= bestDigit;
        }

        int carry = 0;
        for (size_t j = 0; j < tempDivisor.size(); ++j)
        {
            int val = tempDivisor[j] + carry;
            tempDivisor[j] = val % 10;
            carry = val / 10;
        }

        while (carry > 0)
        {
            tempDivisor.push_back(carry % 10);
            carry /= 10;
        }

        removeLeadingZeros(tempDivisor);

        current = subtractDigits(current, tempDivisor);

        quotient.insert(quotient.begin(), bestDigit);
    }

    removeLeadingZeros(quotient);
    remainder = current;

    return quotient;
}

string divide(string a, string b)
{
    bool aNegative = isNegative(a);
    bool bNegative = isNegative(b);

    string aMag = stripSign(a);
    string bMag = stripSign(b);

    vector<int> dividend = toDigits(aMag);
    vector<int> divisor = toDigits(bMag);

    if (divisor.size() == 1 && divisor[0] == 0)
    {
        throw invalid_argument("Cannot divide by zero");
    }

    vector<int> remainder;
    vector<int> quotient = divideDigits(dividend, divisor, remainder);

    return applySign(aNegative ^ bNegative, toString(quotient));
}

string modulo(string a, string b)
{
    bool aNegative = isNegative(a);
    bool bNegative = isNegative(b);

    string aMag = stripSign(a);
    string bMag = stripSign(b);

    vector<int> dividend = toDigits(aMag);
    vector<int> divisor = toDigits(bMag);

    if (divisor.size() == 1 && divisor[0] == 0)
    {
        throw invalid_argument("Cannot divide by zero");
    }

    vector<int> remainder;
    divideDigits(dividend, divisor, remainder);

    return applySign(aNegative, toString(remainder));
}

string power(string base, string exponent)
{
    if (isNegative(exponent))
    {
        throw invalid_argument("Negative exponent not supported");
    }

    string baseMag = stripSign(base);
    bool baseNegative = isNegative(base);

    vector<int> exp = toDigits(exponent);

    if (exp.size() == 1 && exp[0] == 0)
    {
        return "1";
    }

    if (baseMag == "0")
    {
        return "0";
    }

    bool originalExponentOdd = (exp[0] % 2 == 1);

    vector<int> result;
    result.push_back(1);

    vector<int> current = toDigits(baseMag);

    while (true)
    {
        if (exp.size() == 1 && exp[0] == 0)
        {
            break;
        }

        result = multiplyDigits(result, current);

        decrement(exp);
    }

    string resultStr = toString(result);

    if (baseNegative && originalExponentOdd && resultStr != "0")
    {
        return "-" + resultStr;
    }

    return resultStr;
}

string gcd(string a, string b)
{
    string aMag = stripSign(a);
    string bMag = stripSign(b);

    if (aMag == "0")
    {
        return bMag;
    }

    if (bMag == "0")
    {
        return aMag;
    }

    vector<int> x = toDigits(aMag);
    vector<int> y = toDigits(bMag);

    while (true)
    {
        int cmp = compareDigits(x, y);

        if (cmp == 0)
        {
            break;
        }

        if (cmp < 0)
        {
            swap(x, y);
        }

        vector<int> remainder;
        divideDigits(x, y, remainder);

        if (remainder.size() == 1 && remainder[0] == 0)
        {
            break;
        }

        x = remainder;
    }

    return toString(y);
}

string lcm(string a, string b)
{
    string aMag = stripSign(a);
    string bMag = stripSign(b);

    if (aMag == "0" || bMag == "0")
    {
        return "0";
    }

    string gcdValue = gcd(aMag, bMag);
    string product = multiply(aMag, bMag);
    string result = divide(product, gcdValue);

    return result;
}