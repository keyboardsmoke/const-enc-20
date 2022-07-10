#pragma once

#include <cstdint>
#include <ctime>
#include <limits>

namespace constant
{
    namespace mktime
    {
        struct TimeConstants
        {
            static constexpr int SecondsPerMin = 60;
            static constexpr int SecondsPerHour = 3600;
            static constexpr int SecondsPerDay = 86400;
            static constexpr int DaysPerWeek = 7;
            static constexpr int MonthsPerYear = 12;
            static constexpr int DaysPerNonLeapYear = 365;
            static constexpr int DaysPerLeapYear = 366;
            static constexpr int TimeYearBase = 1900;
            static constexpr int EpochYear = 1970;
            static constexpr int EpochWeekDay = 4;
            static constexpr int NumberOfSecondsInLeapYear =
                (DaysPerNonLeapYear + 1) * SecondsPerDay;
            // For asctime the behavior is undefined if struct tm's tm_wday or tm_mon are
            // not within the normal ranges as defined in <time.h>, or if struct tm's
            // tm_year exceeds {INT_MAX}-1990, or if the below asctime_internal algorithm
            // would attempt to generate more than 26 bytes of output (including the
            // terminating null).
            static constexpr int AsctimeBufferSize = 256;
            static constexpr int AsctimeMaxBytes = 26;
            /* 2000-03-01 (mod 400 year, immediately after feb29 */
            static constexpr int64_t SecondsUntil2000MarchFirst =
                (946684800LL + SecondsPerDay * (31 + 29));
            static constexpr int WeekDayOf2000MarchFirst = 3;
            static constexpr int DaysPer400Years =
                (DaysPerNonLeapYear * 400 + (400 / 4) - 3);
            static constexpr int DaysPer100Years =
                (DaysPerNonLeapYear * 100 + (100 / 4) - 1);
            static constexpr int DaysPer4Years = (DaysPerNonLeapYear * 4 + 1);
            // The latest time that can be represented in this form is 03:14:07 UTC on
            // Tuesday, 19 January 2038 (corresponding to 2,147,483,647 seconds since the
            // start of the epoch). This means that systems using a 32-bit time_t type are
            // susceptible to the Year 2038 problem.
            static constexpr int EndOf32BitEpochYear = 2038;

            static constexpr time_t InvalidTime = -1;
        };

        static constexpr const char DaysInMonth[] = { 31 /* Mar */, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31, 29 };
        static constexpr int NonLeapYearDaysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        static consteval int64_t getNumOfLeapYearsBefore(int64_t year)
        {
            return (year / 4) - (year / 100) + (year / 400);
        }

        static consteval bool isLeapYear(const int64_t year)
        {
            return (((year) % 4) == 0 && (((year) % 100) != 0 || ((year) % 400) == 0));
        }

        static consteval int64_t computeRemainingYears(int64_t daysPerYears,
            int64_t quotientYears,
            int64_t* remainingDays) {
            int64_t years = *remainingDays / daysPerYears;
            if (years == quotientYears)
                years--;
            *remainingDays -= years * daysPerYears;
            return years;
        }

        consteval time_t from_tm(
            int tm_sec,   // seconds after the minute - [0, 60] including leap second
            int tm_min,   // minutes after the hour - [0, 59]
            int tm_hour,  // hours since midnight - [0, 23]
            int tm_mday,  // day of the month - [1, 31]
            int tm_mon,   // months since January - [0, 11]
            int tm_year,  // years since 1900
            int tm_wday,  // days since Sunday - [0, 6]
            int tm_yday,  // days since January 1 - [0, 365]
            int tm_isdst  // daylight savings time flag
        )
        {
            // Unlike most C Library functions, mktime doesn't just die on bad input.
            // TODO(rtenneti); Handle leap seconds.
            int64_t tmYearFromBase = tm_year + TimeConstants::TimeYearBase;
            // 32-bit end-of-the-world is 03:14:07 UTC on 19 January 2038.
            if (sizeof(time_t) == 4 &&
                tmYearFromBase >= TimeConstants::EndOf32BitEpochYear) {
                if (tmYearFromBase > TimeConstants::EndOf32BitEpochYear)
                    return TimeConstants::InvalidTime;
                if (tm_mon > 0)
                    return TimeConstants::InvalidTime;
                if (tm_mday > 19)
                    return TimeConstants::InvalidTime;
                if (tm_hour > 3)
                    return TimeConstants::InvalidTime;
                if (tm_min > 14)
                    return TimeConstants::InvalidTime;
                if (tm_sec > 7)
                    return TimeConstants::InvalidTime;
            }

            // Years are ints.  A 32-bit year will fit into a 64-bit time_t
            // A 64-bit year will not.
            static_assert(sizeof(int) == 4,
                "ILP64 is unimplemented.  This implementation requires "
                "32-bit integers.");
            // Calculate number of months and years from tm_mon.
            int64_t month = tm_mon;
            if (month < 0 || month >= TimeConstants::MonthsPerYear - 1) {
                int64_t years = month / 12;
                month %= 12;
                if (month < 0) {
                    years--;
                    month += 12;
                }
                tmYearFromBase += years;
            }
            bool tmYearIsLeap = isLeapYear(tmYearFromBase);
            // Calculate total number of days based on the month and the day (tm_mday).
            int64_t totalDays = tm_mday - 1;
            for (int64_t i = 0; i < month; ++i)
                totalDays += NonLeapYearDaysInMonth[i];
            // Add one day if it is a leap year and the month is after February.
            if (tmYearIsLeap && month > 1)
                totalDays++;
            // Calculate total numbers of days based on the year.
            totalDays += (tmYearFromBase - TimeConstants::EpochYear) *
                TimeConstants::DaysPerNonLeapYear;
            if (tmYearFromBase >= TimeConstants::EpochYear) {
                totalDays += getNumOfLeapYearsBefore(tmYearFromBase - 1) -
                    getNumOfLeapYearsBefore(TimeConstants::EpochYear);
            }
            else if (tmYearFromBase >= 1) {
                totalDays -= getNumOfLeapYearsBefore(TimeConstants::EpochYear) -
                    getNumOfLeapYearsBefore(tmYearFromBase - 1);
            }
            else {
                // Calculate number of leap years until 0th year.
                totalDays -= getNumOfLeapYearsBefore(TimeConstants::EpochYear) -
                    getNumOfLeapYearsBefore(0);
                if (tmYearFromBase <= 0) {
                    totalDays -= 1; // Subtract 1 for 0th year.
                    // Calculate number of leap years until -1 year
                    if (tmYearFromBase < 0) {
                        totalDays -= getNumOfLeapYearsBefore(-tmYearFromBase) -
                            getNumOfLeapYearsBefore(1);
                    }
                }
            }
            // TODO(rtenneti): Need to handle timezone and update of tm_isdst.
            int64_t seconds = tm_sec +
                tm_min * TimeConstants::SecondsPerMin +
                tm_hour * TimeConstants::SecondsPerHour +
                totalDays * TimeConstants::SecondsPerDay;

            return static_cast<time_t>(seconds);
        }
    }
}