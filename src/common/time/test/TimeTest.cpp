/* Copyright (c) 2018 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include <gtest/gtest.h>
#include <folly/Random.h>

#include "common/time/Time.h"

namespace nebula {

TEST(Time, secondsTimeCovertion) {
    // DateTime
    {
        std::vector<DateTime> values;
        // TODO(shylock) support before Epoch
        // values.emplace_back(DateTime(1969, 1, 1, 0, 0, 0, 0));
        for (std::size_t i = 0; i < 4; ++i) {
            values.emplace_back(DateTime(
                                    folly::Random::rand32(1970,
                                                          std::numeric_limits<int16_t>::max()),
                                    folly::Random::rand32(1, 13),
                                    folly::Random::rand32(1, 29),
                                    folly::Random::rand32(0, 24),
                                    folly::Random::rand32(0, 60),
                                    folly::Random::rand32(0, 60),
                                    0));
        }
        for (const auto &dt : values) {
            EXPECT_EQ(dt, time::secondsToDateTime(time::dateTimeToSeconds(dt)));
        }
    }
    {
        std::vector<std::pair<DateTime, int64_t>> dateTimeSeconds {
            {{DateTime(1970, 1, 1, 0, 0, 0, 0), 0},
            {DateTime(2020, 9, 17, 1, 35, 18, 0), 1600306518}},
        };
        for (std::size_t i = 0; i < dateTimeSeconds.size(); ++i) {
            EXPECT_EQ(dateTimeSeconds[i].second, time::dateTimeToSeconds(dateTimeSeconds[i].first));
            EXPECT_EQ(time::secondsToDateTime(dateTimeSeconds[i].second), dateTimeSeconds[i].first);
        }
    }
    // Date
    {
        std::vector<Date> values;
        // TODO(shylock) support before Epoch
        // values.emplace_back(DateTime(1969, 1, 1));
        for (std::size_t i = 0; i < 4; ++i) {
            values.emplace_back(Date(
                                    folly::Random::rand32(1970,
                                                          std::numeric_limits<int16_t>::max()),
                                    folly::Random::rand32(1, 13),
                                    folly::Random::rand32(1, 29)));
        }
        for (const auto &d : values) {
            EXPECT_EQ(d, time::secondsToDate(time::dateToSeconds(d)));
        }
    }
    {
        std::vector<std::pair<Date, int64_t>> dateSeconds {
            {{Date(1970, 1, 1), 0},
            {Date(2003, 3, 4), 1046736000}}
        };
        for (std::size_t i = 0; i < dateSeconds.size(); ++i) {
            EXPECT_EQ(dateSeconds[i].second, time::dateToSeconds(dateSeconds[i].first));
            EXPECT_EQ(time::secondsToDate(dateSeconds[i].second), dateSeconds[i].first);
        }
    }
    // Time
    {
        std::vector<Time> values;
        for (std::size_t i = 0; i < 4; ++i) {
            values.emplace_back(Time(folly::Random::rand32(0, 24),
                                         folly::Random::rand32(0, 60),
                                         folly::Random::rand32(0, 60),
                                         0));
        }
        for (const auto &t : values) {
            EXPECT_EQ(t, time::secondsToTime(time::timeToSeconds(t)));
        }
    }
    {
        std::vector<std::pair<Time, int64_t>> timeSeconds {
            {{Time(0, 0, 0, 0), 0},
            {Time(14, 2, 4, 0), 50524}}
        };
        for (std::size_t i = 0; i < timeSeconds.size(); ++i) {
            EXPECT_EQ(timeSeconds[i].second, time::timeToSeconds(timeSeconds[i].first));
            EXPECT_EQ(time::secondsToTime(timeSeconds[i].second), timeSeconds[i].first);
        }
    }
}

}  // namespace nebula

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    folly::init(&argc, &argv, true);
    google::SetStderrLogging(google::INFO);
    FLAGS_timezone_name = "Asia/Shanghai";
    auto result = nebula::time::initializeGlobalTimezone();
    if (!result.ok()) {
        LOG(FATAL) << result;
    }

    return RUN_ALL_TESTS();
}
