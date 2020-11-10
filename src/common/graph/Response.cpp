/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#include "common/graph/Response.h"

namespace nebula {

bool PlanNodeDescription::operator==(const PlanNodeDescription &rhs) const {
    if (name != rhs.name) {
        return false;
    }
    if (id != rhs.id) {
        return false;
    }
    if (!checkPointer(description.get(), rhs.description.get())) {
        return false;
    }
    if (!checkPointer(profiles.get(), rhs.profiles.get())) {
        return false;
    }
    if (!checkPointer(branch_info.get(), rhs.branch_info.get())) {
        return false;
    }
    return checkPointer(dependencies.get(), rhs.dependencies.get());
}

}  // namespace nebula

