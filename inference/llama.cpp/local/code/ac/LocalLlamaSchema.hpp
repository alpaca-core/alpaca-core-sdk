// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <string>
#include <vector>
#include "ac/SchemaDefinitions.hpp"

namespace ac {

BEGIN_SCHEMA(LlamaSchema)

BEGIN_PARAMS()
DEFINE_PARAM(prompt, std::string, "The input prompt for the model", "")
DEFINE_PARAM(max_tokens, uint32_t, "Maximum number of tokens to generate", 2000u)
DEFINE_PARAM(antiprompts, std::vector<std::string>, "List of strings to stop generation", {})
END_PARAMS()

BEGIN_RESULT()
DEFINE_RESULT(result, std::string, "The generated text")
END_RESULT()

END_SCHEMA()

}
