/**
 * @file Handler.hpp
 * @author paul
 * @date 08.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_HANDLER_HPP
#define FLIGHTCONTROLLER_HANDLER_HPP

#include <cassert>
#include <utility>

#include "FunctionInfo.hpp"
#include "Util.hpp"


namespace mock {
    template<auto... mockedFunctions>
    class Handler {
        using FunctionInfos = decltype(util::buildFunctions<mockedFunctions...>());
        using ReleaseFunc = std::function<void()>;

      public:
        Handler(FunctionInfos &functionInfos, ReleaseFunc releaseFunc) :
            functions{functionInfos},
            releaseFunc{std::move(releaseFunc)} {
        }

        Handler(const Handler<mockedFunctions...> &) = delete;

        Handler(Handler<mockedFunctions...> &&) = delete;

        auto operator=(const Handler<mockedFunctions...> &) = delete;

        auto operator=(Handler<mockedFunctions...> &&) = delete;

        ~Handler() {
            releaseFunc();
        }

        template<auto func>
        auto overrideFunc(auto override) {
            getFunctionInfo<func>().override = override;
        }

        /**
         * Check whether a function got called with specific arguments.
         * @tparam func the pointer to the relevant function
         * @param args can be empty if any call is ok, otherwise number of arguments need to be correct, std::ignore can
         * be used to ignore arguments.
         * @return true if the function got called, false otherwise.
         */
        template<auto func>
        auto functionGotCalled(auto... args) {
            auto &funcInfo = getFunctionInfo<func>();
            if constexpr (sizeof...(args) > 0) {
                static_assert(sizeof...(args) == std::remove_reference_t<decltype(funcInfo)>::NUM_ARGS);
                for (auto it = funcInfo.calls.begin(); it != funcInfo.calls.end(); ++it) {
                    if (util::argsMatch(*it, args...)) {
                        funcInfo.calls.erase(it);
                        return true;
                    }
                }
                return false;
            } else {
                return not funcInfo.calls.empty();
            }
        }

      private:
        /**
         * Get the function info corresponding to a function pointer.
         * @tparam func a pointer to the function (needs to be part of mockedFunctions)
         * @return a reference to the function info object.
         */
        template<auto func>
        auto getFunctionInfo() -> auto & {
            auto &currFunctionInfoSet = std::get<std::vector<GetFunctionInfo<func>>>(functions);
            for (auto &funcInfo : currFunctionInfoSet) {
                if (funcInfo.ptr == func) {
                    return funcInfo;
                }
            }
            throw std::runtime_error{"Function not part of the argument list!"};
        }

        /**
         * Functions is a tuple with an element for each function signature type. Each element is a vector of functions
         * with this signature (to handle different functions with the same signature). Each of the vector elements
         * is a function info object for this function.
         */
        FunctionInfos &functions;

        ReleaseFunc releaseFunc;
    };
} // namespace mock

#endif // FLIGHTCONTROLLER_HANDLER_HPP
