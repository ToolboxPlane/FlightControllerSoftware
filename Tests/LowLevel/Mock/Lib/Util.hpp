/**
 * @file Util.hpp
 * @author paul
 * @date 08.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_MOCKUTIL_HPP
#define FLIGHTCONTROLLER_MOCKUTIL_HPP

#include <tuple>
#include <type_traits>
#include <vector>

#include "FunctionInfo.hpp"

namespace mock::util {
    template<typename Tuple, typename Type, std::size_t i = 0>
    constexpr auto containsType() {
        auto currSame = std::is_same_v<Type, std::tuple_element_t<i, Tuple>>;
        if constexpr (i + 1 < std::tuple_size_v<Tuple>) {
            return currSame or containsType<Tuple, Type, i + 1>();
        } else {
            return currSame;
        }
    }

    template<typename T, typename... Ts>
    constexpr auto removeDuplicatesImpl(std::tuple<T, Ts...>) {
        if constexpr (sizeof...(Ts) > 0) {
            auto following = removeDuplicatesImpl<Ts...>(std::tuple<Ts...>{});
            if constexpr (containsType<decltype(following), T>()) {
                return following;
            } else {
                return std::tuple_cat(std::tuple<T>{}, following);
            }
        } else {
            return std::tuple<T>{};
        }
    }

    constexpr auto removeDuplicatesImpl(std::tuple<>) {
        return std::tuple<>{};
    }

    template<typename Tuple>
    using removeDuplicates = decltype(removeDuplicatesImpl(std::declval<Tuple>()));

    template<typename Ret, typename... Args>
    constexpr auto buildFunctionInfo(Ret (*ptr)(Args...)) {
        return FunctionInfo<Ret, Args...>{.ptr = ptr, .override = {}, .calls = {}};
    }

    template<auto mockedFunction, auto... mockedFunctions>
    constexpr auto insertFunctions(auto &functionInfos) {
        auto &currFunctionInfoSet = std::get<std::vector<GetFunctionInfo<mockedFunction>>>(functionInfos);
        currFunctionInfoSet.emplace_back(buildFunctionInfo(mockedFunction));

        if constexpr (sizeof...(mockedFunctions) > 0) {
            insertFunctions<mockedFunctions...>(functionInfos);
        }
    }

    template<auto... mockedFunctions>
    auto buildFunctions() {
        using FunctionInfos = std::tuple<std::vector<GetFunctionInfo<mockedFunctions>>...>;
        using UniqueFunctionInfos = removeDuplicates<FunctionInfos>;
        UniqueFunctionInfos functionInfos;
        if constexpr (sizeof...(mockedFunctions) > 0) {
            insertFunctions<mockedFunctions...>(functionInfos);
        }
        return functionInfos;
    }

    template<std::size_t i = 0>
    auto argsMatch(auto callTuple, auto arg, auto... args) {
        bool currentMatches = false;
        if constexpr (std::is_same_v<std::remove_cvref_t<decltype(arg)>, std::remove_cvref_t<decltype(std::ignore)>>) {
            currentMatches = true;
        } else {
            currentMatches = (std::get<i>(callTuple) == arg);
        }

        if constexpr (sizeof...(args) > 0) {
            return currentMatches and argsMatch<i + 1>(callTuple, args...);
        } else {
            return currentMatches;
        }
    }
} // namespace mock::util

#endif // FLIGHTCONTROLLER_MOCKUTIL_HPP
