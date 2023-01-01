/**
 * @file FunctionInfo.hpp
 * @author paul
 * @date 08.11.22
 * Description here TODO
 */
#ifndef MOCK_FUNCTIONINFO_HPP
#define MOCK_FUNCTIONINFO_HPP

#include <functional>
#include <optional>
#include <tuple>
#include <vector>

namespace mock {
    template<typename Ret_, typename... Args_>
    struct FunctionInfo {
        using Ret = Ret_;
        using Args = std::tuple<Args_...>;
        static constexpr auto NUM_ARGS = sizeof...(Args_);

        // Pointer to the function
        Ret_ (*ptr)(Args_...);

        // Overriden implementation
        std::optional<std::function<auto(Args_...)->Ret_>> override;

        // All calls of the function
        std::vector<Args> calls;
    };


    template<typename Ret, typename... Args>
    constexpr auto getFunctionInfoImpl(Ret (* /*ptr*/)(Args...)) {
        return FunctionInfo<Ret, Args...>{};
    }

    template<auto fun>
    using GetFunctionInfo = decltype(getFunctionInfoImpl(fun));
} // namespace mock

#endif // MOCK_FUNCTIONINFO_HPP
