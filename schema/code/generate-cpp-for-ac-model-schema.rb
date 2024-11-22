# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
require 'yaml'

CASE_SPLIT = /[ _\-]/

class String
  def pascal_case = split(CASE_SPLIT).map(&:capitalize).join
  def camel_case
    first, *rest = split(CASE_SPLIT)
    first + rest.map(&:capitalize).join
  end
end

CPP_TYPE = {
  string: 'std::string',
  integer: 'int',
  number: 'double',
  boolean: 'bool',
  binary: 'Blob'
}

def generate_read_from_val(pr)
  if pr[:type] == :object
    "#{pr[:cpp_val_type]}::fromDict(val)"
  elsif pr[:type] == :binary
    "std::move(val.get_binary())"
  else
    "std::move(val.get<#{pr[:cpp_val_type]}>())"
  end
end

def generate_write(pr)
  if pr[:type] == :object
    if (pr[:opt])
      "#{pr[:cpp_name]}->toDict()"
    else
      "#{pr[:cpp_name]}.toDict()"
    end
  elsif pr[:type] == :binary
    if (pr[:opt])
      "Dict::binary(std::move(*#{pr[:cpp_name]}))"
    else
      "Dict::binary(std::move(#{pr[:cpp_name]}))"
    end
  else
    if (pr[:opt])
      "std::move(*#{pr[:cpp_name]})"
    else
      "std::move(#{pr[:cpp_name]})"
    end
  end
end

def generate_struct(lines, name, data, indent)
  lines << "#{indent}// #{data['description']}" if data['description']
  lines << "#{indent}struct #{name.pascal_case} {"

  props = if data['properties']
    data['properties'].map do |key, value|
      type = value['type'].to_sym

      pr = {
        name: key,
        type: type,
        desc: value['description'],
        default: value['default'],
        cpp_name: key.camel_case,
        req: data['required']&.include?(key)
      }

      pr[:opt] = !pr[:req] && !pr[:default]

      if type == :array
        atype = value['items']['type'].to_sym
        elem = pr[:elem] = {}
        elem[:type] = atype
        elem[:cpp_name] = 'elem'
        if atype == :object
          lines << generate_struct(lines, key, value['items'], indent + '    ')
          elem[:cpp_val_type] = key.pascal_case
        else
          elem[:cpp_val_type] = CPP_TYPE[atype]
        end
        pr[:cpp_decl_type] = pr[:cpp_val_type] = "std::vector<#{elem[:cpp_val_type]}>"
      else
        if type == :object
          lines << generate_struct(lines, key, value, indent + '    ')
          pr[:cpp_val_type] = key.pascal_case
        else
          pr[:cpp_val_type] = CPP_TYPE[pr[:type]]
        end
        if pr[:opt]
          pr[:cpp_decl_type] = "std::optional<#{pr[:cpp_val_type]}>"
        else
          pr[:cpp_decl_type] = pr[:cpp_val_type]
        end
      end

      pr
    end
  else
    []
  end

  props.each do |pr|
    lines << "#{indent}    // #{pr[:desc]}" if pr[:desc]
    decl = "#{indent}    #{pr[:cpp_decl_type]} #{pr[:cpp_name]}"
    if pr[:default]
      if pr[:type] == :array
        decl += " = {#{pr[:default].map(&:inspect).join(', ')}}"
      else
        decl += " = #{pr[:default].inspect}"
      end
    end
    decl += ';'
    lines << decl
  end

  lines << ''

  lines << "#{indent}    static #{name.pascal_case} fromDict([[maybe_unused]] Dict& dict) {"
  lines << "#{indent}        #{name.pascal_case} ret;"
  props.each do |pr|
    lines << ''
    lines << "#{indent}        if (auto it = dict.find(\"#{pr[:name]}\"); it != dict.end()) {"
    if pr[:type] == :array
      lines << "#{indent}            for (auto& val : *it) {"
      lines << "#{indent}                ret.#{pr[:cpp_name]}.push_back(#{generate_read_from_val(pr[:elem])});"
      lines << "#{indent}            }"
    else
      lines << "#{indent}            auto& val = *it;"
      lines << "#{indent}            ret.#{pr[:cpp_name]} = #{generate_read_from_val(pr)};"
    end

    lines << "#{indent}        }"

    if pr[:req]
      lines << "#{indent}        else {"
      lines << "#{indent}            throw std::runtime_error(\"Missing required field #{pr[:name]}\");"
      lines << "#{indent}        }"
    end
  end
  lines << "#{indent}        return ret;"
  lines << "#{indent}    }"

  lines << "#{indent}    static #{name.pascal_case} fromDict(Dict&& dict) {"
  lines << "#{indent}        return fromDict(dict);"
  lines << "#{indent}    }"

  lines << ''

  lines << "#{indent}    Dict toDict() {"
  lines << "#{indent}        Dict dict;"
  props.each do |pr|
    lines << ''
    type = pr[:type]

    if type == :array
      lines << "#{indent}        if (!#{pr[:cpp_name]}.empty()) {"
      lines << "#{indent}            auto& val = dict[\"#{pr[:name]}\"];"
      lines << "#{indent}            for (auto& elem : #{pr[:cpp_name]}) {"
      lines << "#{indent}                val.push_back(#{generate_write(pr[:elem])});"
      lines << "#{indent}            }"
      lines << "#{indent}        }"
    elsif pr[:opt]
      lines << "#{indent}        if (#{pr[:cpp_name]}.has_value()) {"
      lines << "#{indent}            dict[\"#{pr[:name]}\"] = #{generate_write(pr)};"
      lines << "#{indent}        }"
    else
      lines << "#{indent}        dict[\"#{pr[:name]}\"] = #{generate_write(pr)};"
    end
  end
  lines << "#{indent}        return dict;"
  lines << "#{indent}    }"

  lines << "#{indent}};"
end

def generate_schema(lines, data)
  lines << "// #{data['description']}" if data['description']
  name = data['id']
  lines << "struct #{name.pascal_case} {"
  lines << "    static inline constexpr std::string_view id = \"#{name}\";"
  generate_struct(lines, 'params', data['params'], '    ')

  data['instances'].each do |inst_name, inst_data|
    lines << "    // #{inst_data['description']}" if inst_data['description']
    lines << "    struct Instance#{inst_name.pascal_case} {"
    lines << "        static inline constexpr std::string_view id = \"#{inst_name}\";"
    generate_struct(lines, 'params', inst_data['params'], '        ')
    inst_data['ops'].each do |op_name, op_data|
      lines << "        // #{op_data['description']}" if op_data['description']
      lines << "        struct Op#{op_name.pascal_case} {"
      lines << "            static inline constexpr std::string_view id = \"#{op_name}\";"
      generate_struct(lines, 'params', op_data['params'], '            ')
      generate_struct(lines, 'return', op_data['return'], '            ')
      lines << "        };"
    end

    lines << ''
    lines << "        using Ops = std::tuple<#{inst_data['ops'].keys.map { |op| "Op#{op.pascal_case}" }.join(', ')}>;"

    lines << '';
    lines << <<-UTIL
        static constexpr int getOpIndexById(std::string_view oid) {
            return impl::getTupleIndexByItemId<Ops>(oid);
        }

        template <typename Op>
        static inline constexpr int opIndex = astl::tuple::type_index_v<Op, Ops>;
    UTIL

    lines << "    };"
  end

  lines << ''
  lines << "    using Instances = std::tuple<#{data['instances'].keys.map { |inst| "Instance#{inst.pascal_case}" }.join(', ')}>;"
  lines << ''
  lines << <<-UTIL
    static constexpr int getInstanceById(std::string_view iid) {
        return impl::getTupleIndexByItemId<Instances>(iid);
    }

    template <typename Instance>
    static inline constexpr int instanceIndex = astl::tuple::type_index_v<Instance, Instances>;
  UTIL
  lines << "};"
end

if ARGV.size != 2
  STDERR.puts "Usage: ruby generate-cpp-for-ac-model-schema.rb <input-file> <output-file>"
  exit 1
end

INPUT_FILE = ARGV[0]
OUTPUT_FILE = ARGV[1]

lines = []
lines << <<~HEADER
// This file is generated by the Alpaca Core Local C++ schema generator
// Do not edit manually
#pragma once
#include <ac/Dict.hpp>
#include <ac/schema/TupleIndexByItemId.hpp>
#include <astl/tuple_util.hpp>

namespace ac::local::schema {
HEADER

generate_schema(lines, YAML.load_file(INPUT_FILE))

lines << "\n}\n"

File.write(OUTPUT_FILE, lines.join("\n"))
