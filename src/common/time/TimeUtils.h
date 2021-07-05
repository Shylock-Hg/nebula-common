/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIME_H_
#define COMMON_TIME_TIME_H_

#include <iomanip>
#include <limits>
#include <vector>

#include "common/base/Status.h"
#include "common/base/StatusOr.h"
#include "common/datatypes/Date.h"
#include "common/datatypes/Map.h"
#include "common/fs/FileUtils.h"
#include "common/time/TimeParser.h"
#include "common/time/TimeConversion.h"
#include "common/time/TimezoneInfo.h"
#include "common/time/WallClock.h"

namespace nebula {
namespace time {

// In nebula only store UTC time, and the interpretion of time value based on the
// timezone configuration in current system.

class TimeUtils {
public:
    explicit TimeUtils(...) = delete;

    // check the validation of date
    // not check range limit in here
    // I.E. 2019-02-31
    template <typename D,
              typename = std::enable_if_t<std::is_same<D, Date>::value ||
                                          std::is_same<D, DateTime>::value>>
    static Status validateDate(const D &date) {
        const int64_t *p = TimeConversion::isLeapYear(date.year) ? kLeapDaysSoFar : kDaysSoFar;
        if ((p[date.month] - p[date.month - 1]) < date.day) {
            return Status::Error("`%s' is not a valid date.", date.toString().c_str());
        }
        return Status::OK();
    }

    static StatusOr<DateTime> parseDateTime(const std::string &str) {
        TimeParser p;
        auto result = p.parseDateTime(str);
        NG_RETURN_IF_ERROR(result);
        NG_RETURN_IF_ERROR(validateDate(result.value()));
        return result.value();
    }

    static StatusOr<DateTime> dateTimeFromMap(const Map &m);

    // utc + offset = local
    static DateTime dateTimeToUTC(const DateTime &dateTime) {
        return TimeConversion::dateTimeShift(dateTime,
                                             -Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static DateTime utcToDateTime(const DateTime &dateTime) {
        return TimeConversion::dateTimeShift(dateTime,
                                             Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static DateTime localDateTime() {
        auto time = unixTime();
        auto dt = TimeConversion::unixSecondsToDateTime(
            time.seconds - Timezone::getGlobalTimezone().utcOffsetSecs());
        dt.microsec = time.milliseconds * 1000;
        return dt;
    }

    static DateTime utcDateTime() {
        auto time = unixTime();
        auto dt = TimeConversion::unixSecondsToDateTime(time.seconds);
        dt.microsec = time.milliseconds * 1000;
        return dt;
    }

    static StatusOr<Date> dateFromMap(const Map &m);

    static StatusOr<Date> parseDate(const std::string &str) {
        TimeParser p;
        auto result = p.parseDate(str);
        NG_RETURN_IF_ERROR(result);
        NG_RETURN_IF_ERROR(validateDate(result.value()));
        return result.value();
    }

    static StatusOr<Date> localDate() {
        Date d;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToDate(unixTime -
                                                 Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static StatusOr<Date> utcDate() {
        Date d;
        time_t unixTime = std::time(NULL);
        if (unixTime == -1) {
            return Status::Error("Get unix time failed: %s.", std::strerror(errno));
        }
        return TimeConversion::unixSecondsToDate(unixTime);
    }

    static StatusOr<Time> timeFromMap(const Map &m);

    static StatusOr<Time> parseTime(const std::string &str) {
        TimeParser p;
        return p.parseTime(str);
    }

    // utc + offset = local
    static Time timeToUTC(const Time &time) {
        return TimeConversion::timeShift(time, -Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static Time utcToTime(const Time &time) {
        return TimeConversion::timeShift(time, Timezone::getGlobalTimezone().utcOffsetSecs());
    }

    static Time localTime() {
        auto time = unixTime();
        auto t = TimeConversion::unixSecondsToTime(time.seconds -
                                                   Timezone::getGlobalTimezone().utcOffsetSecs());
        t.microsec = time.milliseconds * 1000;
        return t;
    }

    static Time utcTime() {
        auto time = unixTime();
        auto t = TimeConversion::unixSecondsToTime(time.seconds);
        t.microsec = time.milliseconds * 1000;
        return t;
    }

    static StatusOr<Value> toTimestamp(const Value &val);

private:
    struct UnixTime {
        int64_t seconds{0};
        int64_t milliseconds{0};
    };

    // <seconds, milliseconds>
    static UnixTime unixTime() {
        auto ms = WallClock::fastNowInMilliSec();
        return UnixTime{ms / 1000, ms % 1000};
    }
};   // class TimeUtils

}   // namespace time
}   // namespace nebula

#endif   // COMMON_TIME_TIME_H_
