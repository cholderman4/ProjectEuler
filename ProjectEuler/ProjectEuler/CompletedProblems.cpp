#include "CompletedProblems.h"

#include <vector>

#include "BigInt.h"
#include "PrimeSieve.h"
#include "UtilityHelpers.h"

namespace completed
{
    using namespace util;

    // Problem 19
    Int count_sundays()
    {
        int year = 1900;
        int month = 1;
        int day = 1;
        int weekDay = 1; // Sunday = 0
        int sundays = 0;

        while (year <= 2000 && month <= 12 && day <= 31)
        {
            // Check the problem conditions
            if (day == 1
                && weekDay == 0 // sunday
                && year >= 1901
                && year <= 2000)
            {
                ++sundays;
            }

            // Advance the weekday
            ++weekDay;
            if (weekDay == 7)
                weekDay = 0;

            // Advance the day
            ++day;
            if (day < 29) // no new month/year
                continue;

            if (month == 2)
            {
                // No leap year
                if (year % 4 != 0 || (year % 100 == 0 && year % 400 != 0))
                {
                    day = 1;
                    ++month;
                    continue;
                }
                else if (day == 30)
                {
                    day = 1;
                    ++month;
                    continue;
                }
            }

            // Check 30 day months
            if (day == 31
                && (month == 4
                    || month == 6
                    || month == 9
                    || month == 11))
            {
                day = 1;
                ++month;
                continue;
            }

            if (day == 32)
            {
                day = 1;
                ++month;
                if (month == 13)
                {
                    month = 1;
                    ++year;
                }
                continue;
            }
        }

        return sundays;
    }

    // Problem 20
    Int factorial_digit_sum()
    {
        // Condensed with BigInt class
        auto num = BigInt(1);

        constexpr int max = 100;
        for (uint32_t i = 1; i <= max; ++i)
            num *= i;

        return num.sum_digits();
    }

    // Problem 21
    Int sum_amicable_numbers()
    {
        constexpr Int MAX_N = 10000;
        auto foundDivisors = std::vector<bool>(MAX_N, false);
        Int sumAmicableNumbers = 0;
        for (auto n = 1; n <= MAX_N; ++n)
        {
            if (foundDivisors[n - 1] == true)
                continue;

            foundDivisors[n - 1] = true;

            const auto sum1 = sum_divisors(n);
            if (sum1 > MAX_N || foundDivisors[sum1 - 1] == true)
                continue;

            foundDivisors[sum1 - 1] = true;
            const auto sum2 = sum_divisors(sum1);
            if (sum2 == n)
            {
                sumAmicableNumbers += sum1;
                sumAmicableNumbers += sum2;
            }
        }

        return sumAmicableNumbers;
    }

    // Problem 23
    bool has_abundant_sum(const int n, const std::vector<int8_t>& isAbundant)
    {
        // Check pairs
        for (auto s1 = 1; s1 <= n / 2; ++s1)
        {
            const auto s2 = n - s1;
            if (isAbundant[s1 - 1] == 1 && isAbundant[s2 - 1] == 1)
                return true;
        }

        return false;
    }

    Int non_abundant_sums()
    {
        constexpr Int LIMIT = 28123;

        // -1 for non-abundant (deficient or abundant)
        // +1 for abundant
        auto isAbundant = std::vector<int8_t>(LIMIT, 0);

        // We know 12 is the smallest abundant number
        std::fill(isAbundant.begin(), isAbundant.begin() + 11, -1);
        isAbundant[11] = 1;

#pragma omp parallel for
        for (auto n = 13; n <= LIMIT; ++n)
        {
            isAbundant[n - 1] = (sum_divisors(n) > n) ? 1 : -1;
        }

        Int sumNonAbundantSum = 12 * 23;
#pragma omp parallel for reduction(+:sumNonAbundantSum)
        for (auto n = 25; n <= LIMIT; ++n)
        {
            if (!has_abundant_sum(n, isAbundant))
            {
                sumNonAbundantSum += n;
            }
        }

        return sumNonAbundantSum;
    }

    // Problem 25
    Int fibonacci_1000_digit()
    {
        constexpr auto MAX_DIGIT = 1000u;
        BigInt a = 1;
        BigInt b = 1;
        BigInt c = a + b;
        Int step = 3;
        while (c.digits10() < MAX_DIGIT)
        {
            a = b;
            b = c;
            c += a;
            ++step;
        }

        return step;
    }

    // Problem 26
    Int reciprocal_cycles_slow()
    {
        constexpr Int MAX_DIVISOR = 999;

        auto r2step = std::vector<uint64_t>(MAX_DIVISOR, 0);

        Int maxCycle = 0;
        Int maxDivisor = 0;
        for (auto d = 2; d <= MAX_DIVISOR; ++d)
        {
            uint64_t step = 1;
            Int r = 10 % d;
            Int cycle = 0;
            while (true)
            {
                if (r == 0) break;

                if (r2step[r] != 0)
                {
                    // Seen this remainder before
                    cycle = step - r2step[r];
                    break;
                }
                r2step[r] = step;
                ++step;
                r = (10 * r) % d;
            }

            if (cycle > maxCycle)
            {
                maxCycle = cycle;
                maxDivisor = d;
            }
            std::fill(r2step.begin(), r2step.end(), 0);
        }
        //std::cout << maxCycle << std::endl;
        return maxDivisor;
    }

    bool is_reptend_prime(const Int p)
    {
        // Replicate long division, but only keep the remainder.
        Int k = 1;
        Int r = 10 % p;
        while (r != 1 && k < p)
        {
            ++k;
            r = (10 * r) % p;
        }

        return (k == p - 1);
    }

    Int reciprocal_cycles_fast()
    {
        constexpr Int MAX_DIVISOR = 999;

        const auto sieve = PrimeSieve(MAX_DIVISOR);

        for (Int d = MAX_DIVISOR; d >= 2; --d)
        {
            if (!sieve.is_prime(d))
                continue;

            if (is_reptend_prime(d))
                return d;
        }

        return 0;
    }

    // Problem 27
    int64_t quadratic_primes()
    {
        constexpr auto LIMIT = 1000u;
        auto sieve = PrimeSieve(13 * LIMIT);
        Int max_n = 40;
        int64_t ab = 41;
        int64_t b = 1;

        // By solving for n = 0 and n = 1, we can set conditions on a and b
        while ((b = sieve.next_prime(b)) < LIMIT)
        {
            int64_t p = 1;
            while ((p = sieve.next_prime(p)) < LIMIT + 1 + b && b < LIMIT - 1 + p)
            {
                const int64_t a = p - b - 1;

                // Start at 2, since we already know 0 & 1.
                int64_t n = 2;
                while (true)
                {
                    const int64_t p_test = n * n + a * n + b;
                    if (p_test <= 1)
                        break;

                    // From previous tests, we know we won't need to resize
                    /*if (sieve.limit() < p_test)
                    {
                        sieve.rebuild_integer_limit(p_test);
                    }*/

                    if (!sieve.is_prime(p_test))
                        break;
                    ++n;
                }

                if (n > max_n)
                {
                    max_n = n;
                    ab = a * b;
                }
            }
        }

        return ab;
    }
}
