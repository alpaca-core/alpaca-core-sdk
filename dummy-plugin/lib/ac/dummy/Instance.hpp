// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include <astl/generator.hpp>
#include <span>
#include <vector>
#include <string>

namespace ac::dummy {
class Model;

class Instance {
public:
    struct InitParams {
        int cutoff = -1; // cut off model data to n-th element (or don't cut if -1)
    };

    Instance(Model& model, InitParams params);
    ~Instance();

    struct SessionParams {
        bool splice = true; // splice input with model data (otherwise append model data to input)
        int throwOn = -1; // throw exception on n-th token (or don't throw if -1)
    };

    astl::generator<const std::string&> newSession(std::vector<std::string> input, SessionParams params);

    const Model& model() const noexcept { return m_model; }
private:
    const InitParams m_params;
    Model& m_model;
    std::span<const std::string> m_data;
};

} // namespace ac::dummy
