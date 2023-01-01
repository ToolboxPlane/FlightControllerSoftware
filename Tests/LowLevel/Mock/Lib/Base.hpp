/**
 * @file Base.hpp
 * @author paul
 * @date 08.11.22
 * Description here TODO
 */
#ifndef MOCK_BASE_HPP
#define MOCK_BASE_HPP

#include <cassert>
#include <stdexcept>

#include "Handler.hpp"

namespace mock {
    template<auto... mockedFunctions>
    class Base {
        using Handle = Handler<mockedFunctions...>;
        using FunctionInfos = decltype(util::buildFunctions<mockedFunctions...>());

      public:
        auto getHandle() -> Handle {
            assert(not functions.has_value());
            functions.emplace(util::buildFunctions<mockedFunctions...>());
            return Handle{functions.value(), [this]() { releaseHandle(); }};
        }

        /**
         * Function to be called by all mocked functions, not part of the external interface
         * @tparam func function pointer to the function
         * @param args arguments passed to the function
         * @return
         */
        template<auto func>
        auto functionCallDelegate(auto... args) {
            auto &funcInfo = getFunctionInfo<func>();

            // Mark function as called
            funcInfo.calls.emplace_back(args...);

            if constexpr (std::is_same_v<typename std::remove_cvref_t<decltype(funcInfo)>::Ret, void>) {
                // Check if function is overriden
                if (funcInfo.override.has_value()) {
                    funcInfo.override.value()(args...);
                }
            } else {
                // If a function needs to return something it needs to be overriden
                assert(funcInfo.override.has_value());
                return funcInfo.override.value()(args...);
            }
        }

      private:
        void releaseHandle() {
            functions.reset();
        }

        /**
         * Get the function info corresponding to a function pointer.
         * @tparam func a pointer to the function (needs to be part of mockedFunctions)
         * @return a reference to the function info object.
         */
        template<auto func>
        auto getFunctionInfo() -> auto & {
            assert(functions.has_value());
            auto &currFunctionInfoSet = std::get<std::vector<GetFunctionInfo<func>>>(functions.value());
            for (auto &funcInfo : currFunctionInfoSet) {
                if (funcInfo.ptr == func) {
                    return funcInfo;
                }
            }
            throw std::runtime_error{"Invalid function!"};
        }

        /**
         * Functions is a tuple with an element for each function signature type. Each element is a vector of functions
         * with this signature (to handle different functions with the same signature). Each of the vector elements
         * is a function info object for this function.
         */
        std::optional<FunctionInfos> functions;
    };
} // namespace mock

#endif // MOCK_BASE_HPP
