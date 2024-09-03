/**
 * @file dict-c.cpp
 * @brief Implementation of the C wrapper for the C++ Dict class in Alpaca Core.
 *
 * This file provides a C API implementation that wraps the C++ Dict class,
 * allowing C programs to interact with the dictionary structures defined in Dict.hpp.
 * It includes operations for creating, manipulating, and querying dictionary structures,
 * bridging the gap between C and C++ implementations.
 */

// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "dict.h"
#include "DictCUtil.hpp"

#include <astl/move.hpp>
#include <splat/warnings.h>

#include <cstring>
#include <cassert>

DISABLE_MSVC_WARNING(4996) // posix name deprecated

/**
 * @struct ac_dict_root
 * @brief Root structure for the dictionary.
 *
 * This structure wraps the C++ ac::Dict object.
 */
struct ac_dict_root {
    ac::Dict dict;
};

/**
 * @struct ac_dict_iter
 * @brief Iterator structure for dictionary traversal.
 *
 * This structure holds the current iterator and the end iterator
 * for dictionary traversal operations.
 */
struct ac_dict_iter {
    ac::Dict::iterator it;
    ac::Dict::iterator end;
};

namespace ac::cutil {
/**
 * @brief Consumes a C dictionary root and returns the contained C++ Dict object.
 * @param d Pointer to the dictionary root.
 * @return The Dict object contained in the root.
 */
Dict Dict_from_dict_root_consume(ac_dict_root* d) {
    if (!d) return {};
    auto ret = std::move(d->dict);
    ac_dict_free_root(d);
    return ret;
}

/**
 * @brief Converts a C dictionary reference to a C++ Dict object reference.
 * @param ref The dictionary reference.
 * @return Reference to the Dict object.
 */
ac::Dict& Dict_from_dict_ref(ac_dict_ref ref) {
    assert(ref); // should never be null here
    return *reinterpret_cast<ac::Dict*>(ref);
}

/**
 * @brief Converts a C++ Dict object reference to a C dictionary reference.
 * @param dict Reference to the Dict object.
 * @return The dictionary reference.
 */
ac_dict_ref Dict_to_dict_ref(ac::Dict& dict) {
    return reinterpret_cast<ac_dict_ref>(&dict);
}

/**
 * @brief Parses a JSON string into a Dict object.
 * @param json The JSON string to parse.
 * @param json_end Pointer to the end of the JSON string (optional).
 * @return The parsed Dict object.
 */
ac::Dict Dict_parse(const char* json, const char* json_end) {
    if (!json_end) {
        json_end = json + strlen(json);
    }
    return ac::Dict::parse(json, json_end);
}
} // namespace ac::cutil

using namespace ac::cutil;

namespace {

thread_local std::string dict_last_error;

/**
 * @brief Executes a function and catches any exceptions, storing the error message.
 * @tparam F The type of the function to execute.
 * @param f The function to execute.
 * @return The result of the function execution, or a default-constructed value on error.
 */
template <typename F>
auto dict_try_catch(F&& f) noexcept -> decltype(f()) {
    dict_last_error.clear();
    try {
        return f();
    }
    catch (const std::exception& e) {
        dict_last_error = e.what();
    }
    catch (...) {
        dict_last_error = "Unknown error";
    }
    return {};
}
} // namespace

/**
 * @brief Retrieves the last error message.
 * @return The last error message, or nullptr if no error occurred.
 */
const char* ac_dict_get_last_error() {
    if (dict_last_error.empty()) return nullptr;
    return dict_last_error.c_str();
}

extern "C" {

/**
 * @brief Creates a new dictionary root.
 * @return Pointer to the new dictionary root, or nullptr on error.
 */
ac_dict_root* ac_dict_new_root() {
    return dict_try_catch([] {
        return new ac_dict_root();
    });
}

/**
 * @brief Frees a dictionary root.
 * @param d Pointer to the dictionary root to free.
 */
void ac_dict_free_root(ac_dict_root* d) {
    delete d;
}

/**
 * @brief Parses a JSON string into a dictionary.
 * @param target The target dictionary reference.
 * @param json The JSON string to parse.
 * @param json_end Pointer to the end of the JSON string (optional).
 * @return true if parsing was successful, false otherwise.
 */
bool ac_dict_parse_json(ac_dict_ref target, const char* json, const char* json_end) {
    return dict_try_catch([&] {
        Dict_from_dict_ref(target) = Dict_parse(json, json_end);
        return true;
    });
}

/**
 * @brief Copies a dictionary.
 * @param target The target dictionary reference.
 * @param source The source dictionary reference.
 */
void ac_dict_copy(ac_dict_ref target, ac_dict_ref source) {
    Dict_from_dict_ref(target) = Dict_from_dict_ref(source);
}

/**
 * @brief Moves a dictionary.
 * @param target The target dictionary reference.
 * @param source The source dictionary reference.
 */
void ac_dict_take(ac_dict_ref target, ac_dict_ref source) {
    Dict_from_dict_ref(target) = astl::move(Dict_from_dict_ref(source));
}

/**
 * @brief Creates a dictionary reference from a root.
 * @param d Pointer to the dictionary root.
 * @return The dictionary reference.
 */
ac_dict_ref ac_dict_make_ref(ac_dict_root* d) {
    return Dict_to_dict_ref(d->dict);
}

/**
 * @brief Retrieves a dictionary reference for a key.
 * @param d The dictionary reference.
 * @param key The key to look up.
 * @return The dictionary reference for the key, or nullptr on error.
 */
ac_dict_ref ac_dict_at_key(ac_dict_ref d, const char* key) {
    return dict_try_catch([&] {
        return Dict_to_dict_ref(Dict_from_dict_ref(d).at(key));
    });
}

/**
 * @brief Retrieves a dictionary reference for an index.
 * @param d The dictionary reference.
 * @param index The index to look up.
 * @return The dictionary reference for the index, or nullptr on error.
 */
ac_dict_ref ac_dict_at_index(ac_dict_ref d, int index) {
    return dict_try_catch([&] {
        return Dict_to_dict_ref(Dict_from_dict_ref(d).at(index));
    });
}

/**
 * @brief Get the type of a dictionary value.
 *
 * This function determines the type of the value stored in the given dictionary reference.
 *
 * @param d The dictionary reference to check.
 * @return The type of the dictionary value as an ac_dict_value_type enum.
 */
ac_dict_value_type ac_dict_get_type(ac_dict_ref d) {
    switch (Dict_from_dict_ref(d).type()) {
        case ac::Dict::value_t::null: return ac_dict_value_type_null;
        case ac::Dict::value_t::boolean: return ac_dict_value_type_bool;
        case ac::Dict::value_t::number_integer: return ac_dict_value_type_number_int;
        case ac::Dict::value_t::number_unsigned: return ac_dict_value_type_number_unsigned;
        case ac::Dict::value_t::number_float: return ac_dict_value_type_number_double;
        case ac::Dict::value_t::string: return ac_dict_value_type_string;
        case ac::Dict::value_t::array: return ac_dict_value_type_array;
        case ac::Dict::value_t::object: return ac_dict_value_type_object;
        case ac::Dict::value_t::binary: return ac_dict_value_type_binary;
        default:
            dict_last_error = "Unsupported type";
            return ac_dict_value_type_null;
    }
}

/**
 * @brief Get the size of a dictionary value.
 *
 * @param d The dictionary reference.
 * @return The size of the dictionary value (0 for null, 1 for scalar types, number of elements for arrays/objects).
 */
int ac_dict_get_size(ac_dict_ref d) {
    return int(Dict_from_dict_ref(d).size());
}

/**
 * @brief Get the boolean value from a dictionary.
 *
 * @param d The dictionary reference.
 * @return The boolean value (false on error).
 */
bool ac_dict_get_bool_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<bool>();
    });
}

/**
 * @brief Get the integer value from a dictionary.
 *
 * @param d The dictionary reference.
 * @return The integer value (0 on error).
 */
int ac_dict_get_int_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<int>();
    });
}

/**
 * @brief Get the unsigned integer value from a dictionary.
 *
 * @param d The dictionary reference.
 * @return The unsigned integer value (0 on error).
 */
unsigned ac_dict_get_unsigned_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<unsigned>();
    });
}

/**
 * @brief Get the double value from a dictionary.
 *
 * @param d The dictionary reference.
 * @return The double value (0.0 on error).
 */
double ac_dict_get_double_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<double>();
    });
}

/**
 * @brief Get the string value from a dictionary.
 *
 * @param d The dictionary reference.
 * @return The string value (null if not a string).
 */
const char* ac_dict_get_string_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        return Dict_from_dict_ref(d).get<std::string_view>().data();
    });
}

/**
 * @brief Get the binary value from a dictionary.
 *
 * @param d The dictionary reference.
 * @return An ac_dict_binary_buf structure containing the binary data and its size.
 */
ac_dict_binary_buf ac_dict_get_binary_value(ac_dict_ref d) {
    return dict_try_catch([&] {
        auto& bin = Dict_from_dict_ref(d).get_binary();
        return ac_dict_binary_buf{ bin.data(), uint32_t(bin.size()) };
    });
}

/**
 * @brief Create a new iterator for a dictionary.
 *
 * @param d The dictionary reference to iterate over.
 * @return A new ac_dict_iter pointer, or null on error.
 */
ac_dict_iter* ac_dict_new_iter(ac_dict_ref d) {
    return new ac_dict_iter{ Dict_from_dict_ref(d).begin(), Dict_from_dict_ref(d).end() };
}

/**
 * @brief Move the iterator to the next element.
 *
 * @param it The current iterator.
 * @return The updated iterator, or null if there are no more elements.
 */
ac_dict_iter* ac_dict_iter_next(ac_dict_iter* it) {
    ++it->it;
    if (it->it == it->end) {
        delete it;
        return nullptr;
    }
    return it;
}

/**
 * @brief Free the memory allocated for an iterator.
 *
 * @param it The iterator to free.
 */
void ac_dict_free_iter(ac_dict_iter* it) {
    delete it;
}

/**
 * @brief Get the key from the current iterator position.
 *
 * @param it The current iterator.
 * @return The key as a C string, or null if not applicable.
 */
const char* ac_dict_iter_get_key(ac_dict_iter* it) {
    return dict_try_catch([&] {
        return it->it.key().data();
    });
}

/**
 * @brief Get the value from the current iterator position.
 *
 * @param it The current iterator.
 * @return A dictionary reference to the value at the current position.
 */
ac_dict_ref ac_dict_iter_get_value(ac_dict_iter* it) {
    return Dict_to_dict_ref(it->it.value());
}

/**
 * @brief Set the value of a dictionary to null.
 *
 * @param parent The dictionary reference to set.
 */
void ac_dict_set_null(ac_dict_ref parent) {
    Dict_from_dict_ref(parent) = nullptr;
}

/**
 * @brief Set the value of a dictionary to a boolean.
 *
 * @param parent The dictionary reference to set.
 * @param value The boolean value to set.
 */
void ac_dict_set_bool(ac_dict_ref parent, bool value) {
    Dict_from_dict_ref(parent) = value;
}

/**
 * @brief Set the value of a dictionary to an integer.
 *
 * @param parent The dictionary reference to set.
 * @param value The integer value to set.
 */
void ac_dict_set_int(ac_dict_ref parent, int value) {
    Dict_from_dict_ref(parent) = value;
}

/**
 * @brief Set the value of a dictionary to an unsigned integer.
 *
 * @param parent The dictionary reference to set.
 * @param value The unsigned integer value to set.
 */
void ac_dict_set_unsigned(ac_dict_ref parent, unsigned value) {
    Dict_from_dict_ref(parent) = value;
}

/**
 * @brief Set the value of a dictionary to a double.
 *
 * @param parent The dictionary reference to set.
 * @param value The double value to set.
 */
void ac_dict_set_double(ac_dict_ref parent, double value) {
    Dict_from_dict_ref(parent) = value;
}

/**
 * @brief Set the value of a dictionary to a string.
 *
 * @param parent The dictionary reference to set.
 * @param value The string value to set.
 * @param end Pointer to the end of the string (null for zero-terminated).
 */
void ac_dict_set_string(ac_dict_ref parent, const char* value, const char* end) {
    if (!end) {
        end = value + strlen(value);
    }
    Dict_from_dict_ref(parent) = std::string(value, end);
}

/**
 * @brief Set the value of a dictionary to an empty array.
 *
 * @param parent The dictionary reference to set.
 */
void ac_dict_set_array(ac_dict_ref parent) {
    Dict_from_dict_ref(parent) = ac::Dict::array();
}

/**
 * @brief Set the value of a dictionary to an empty object.
 *
 * @param parent The dictionary reference to set.
 */
void ac_dict_set_object(ac_dict_ref parent) {
    Dict_from_dict_ref(parent) = ac::Dict::object();
}

/**
 * @brief Set the value of a dictionary to binary data.
 *
 * @param parent The dictionary reference to set.
 * @param data Pointer to the binary data.
 * @param size Size of the binary data.
 */
void ac_dict_set_binary(ac_dict_ref parent, const uint8_t* data, uint32_t size) {
    Dict_from_dict_ref(parent) = ac::Dict::binary(ac::Blob(data, data + size));
}

/**
 * @brief Add a child to a dictionary.
 *
 * @param parent The parent dictionary reference.
 * @param key The key for the new child (null for array elements).
 * @return A reference to the new child, or null on error.
 */
ac_dict_ref ac_dict_add_child(ac_dict_ref parent, const char* key) {
    return dict_try_catch([&] {
        if (key) {
            return Dict_to_dict_ref(Dict_from_dict_ref(parent).operator[](key));
        }
        else {
            return Dict_to_dict_ref(Dict_from_dict_ref(parent).emplace_back(ac::Dict()));
        }
    });
}

/**
 * @brief Dump the dictionary to a string.
 *
 * @param d The dictionary reference to dump.
 * @param indent The indentation level (-1 for compact, 0 for pretty, 1+ for pretty with indent).
 * @return A newly allocated string containing the dumped dictionary (caller must free).
 */
char* ac_dict_dump(ac_dict_ref d, int indent) {
    auto dump = Dict_from_dict_ref(d).dump(indent);
    return strdup(dump.c_str());
}

/**
 * @brief Dump the dictionary to a buffer.
 *
 * @param d The dictionary reference to dump.
 * @param indent The indentation level (-1 for compact, 0 for pretty, 1+ for pretty with indent).
 * @param buf The buffer to write to.
 * @param buf_size The size of the buffer.
 * @return The number of characters that would have been written (ignoring buf_size), or -1 on error.
 */
int ac_dict_dump_to(ac_dict_ref d, int indent, char* buf, int buf_size) {
    auto dump = Dict_from_dict_ref(d).dump(indent);
    const int s = int(dump.size());
    if (s < buf_size) {
        memcpy(buf, dump.c_str(), dump.size() + 1);
    }
    return s;
}

} // extern "C"
