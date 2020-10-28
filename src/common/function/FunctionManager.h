/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_FUNCTION_FUNCTIONMANAGER_H_
#define COMMON_FUNCTION_FUNCTIONMANAGER_H_

#include "common/base/StatusOr.h"
#include "common/base/Status.h"
#include "common/datatypes/Value.h"
#include <folly/futures/Future.h>

/**
 * FunctionManager is for managing builtin and dynamic-loaded functions,
 * which users could use as function call expressions.
 *
 * TODO(dutor) To implement dynamic loading.
 */

namespace nebula {

struct TypeSignature {
    TypeSignature() = default;
    TypeSignature(std::vector<Value::Type> argsType, Value::Type returnType)
        :argsType_(std::move(argsType)), returnType_(returnType) {}
    std::vector<Value::Type> argsType_;
    Value::Type returnType_;
};

class FunctionManager final {
public:
    using Function = std::function<Value(const std::vector<Value>&)>;

    /**
     * To obtain a function named `func', with the actual arity.
     */
    static StatusOr<Function> get(const std::string &func, size_t arity);

    static StatusOr<bool> getStateless(const std::string &func, size_t arity);

    /**
     * To load a set of functions from a shared object dynamically.
     */
    static Status load(const std::string &soname, const std::vector<std::string> &funcs);

    /**
     * To unload a shared object.
     */
    static Status unload(const std::string &soname, const std::vector<std::string> &funcs);

    /**
     * To obtain the return value type according to the parameter type
     */
    static StatusOr<Value::Type> getReturnType(const std::string &funcName,
                                               const std::vector<Value::Type> &argsType);

private:
    struct FunctionAttributes final {
        size_t minArity_{0};
        size_t maxArity_{0};
        // Stateless means same input same result
        bool     stateless_{true};
        Function body_;
    };

    /**
     * FunctionManager functions as a singleton, since the dynamic loading is process-wide.
     */
    FunctionManager();

    static FunctionManager &instance();

    StatusOr<const FunctionAttributes>
    getInternal(const std::string &func, size_t arity) const;

    Status loadInternal(const std::string &soname, const std::vector<std::string> &funcs);

    Status unloadInternal(const std::string &soname, const std::vector<std::string> &funcs);

    static std::unordered_map<std::string, std::vector<TypeSignature>> typeSignature_;

    mutable folly::RWSpinLock lock_;
    std::unordered_map<std::string, FunctionAttributes> functions_;
};

}   // namespace nebula

#endif  // COMMON_FUNCTION_FUNCTIONMANAGER_H_
