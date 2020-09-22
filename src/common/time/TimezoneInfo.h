/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_TIME_TIMEZONEINFO_H_
#define COMMON_TIME_TIMEZONEINFO_H_

#include "common/base/Base.h"
#include "common/base/Status.h"

namespace nebula {
namespace time {

class Timezone {
public:
    explicit Timezone(const std::string &name) : name_(name) {}

    MUST_USE_RESULT Status load() {
        const auto &it = tzdb.find(name_);
        if (it == tzdb.end()) {
            return Status::Error("Not supported timezone `%s'.", name_.c_str());
        }
#if DCHECK_IS_ON()
        valid_ = true;
#endif
        return Status::OK();
    }

    const std::string &abbreviation() const {
#if DCHECK_IS_ON()
        CHECK(valid_);
#endif
        return info_.abbreviation;
    }

    int32_t utcOffset() const {
#if DCHECK_IS_ON()
        CHECK(valid_);
#endif
        return info_.utcOffset;
    }

private:
    struct TimezoneInfo {
        std::string abbreviation;
        int32_t utcOffset;   // utc/gmt offset in seconds
    };

    static const std::unordered_map<std::string, TimezoneInfo> tzdb;

#if DCHECK_IS_ON()
    bool valid_{false};
#endif

    std::string name_;
    TimezoneInfo info_;
};

}   // namespace time
}   // namespace nebula

#endif   // COMMON_TIME_TIMEZONEINFO_H_
