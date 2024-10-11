// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#pragma once
#include <ac/Dict.hpp>
#include <ac/schema/SchemaItem.hpp>
#include <astl/tuple_util.hpp>

// here we use tuples to define instances within a model and operations within an instance
// an arguably better implementation would have been to define an enum and specialize an empty template
// for each value of the enum, as in:
/*
class ModelSchema {
    enum class Instances { A, B, Count };
    template <> class Instance;
    template <> struct Instance<Instances::A> { ... };
};*/
// then we could switch by the enum and... you know the drill
// however gcc does not allow us to specialize inside a class https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
// so, tuples it is

namespace ac::local::schema {

using namespace ac::schema;

class Binary : public SchemaItem {
    void doDescribeSelf(OrderedDict& d) const override {
        d["type"] = "blob";
    }
public:
    using SchemaItem::SchemaItem;

    Blob* getValue() const {
        if (!m_self || m_self->is_null()) return nullptr;
        return &m_self->get_binary();
    }

    void setValue(Blob value) {
        materializeSelf();
        *m_self = Dict::binary(std::move(value));
    }
};

namespace impl {

template <typename Tuple>
static constexpr int getTupleIndexByItemId(std::string_view id) {
    Tuple items;
    return std::apply([&](auto&&... item) -> int {
        int i = 0;
        std::array ids = { item.id... };
        for (auto n : ids) {
            if (n == id) {
                return i;
            }
            ++i;
        }
        return -1;
    }, items);
}

} // namespace impl

template <typename Instance>
struct InstanceHelper {
    static constexpr int getOpIndexById(std::string_view id) {
        return impl::getTupleIndexByItemId<typename Instance::Ops>(id);
    }

    template <typename Op>
    static inline constexpr int opIndex = astl::tuple::type_index_v<Op, typename Instance::Ops>;

    template <typename Op>
    static void describeOp(OrderedDict& d, Op) {
        auto& body = d[Op::id];
        body["description"] = Op::description;
        typename Op::Params{}.describeSelf(body["params"]);
        typename Op::Return{}.describeSelf(body["return"]);
    }

    static void describe(OrderedDict& d) {
        d["description"] = Instance::description;
        typename Instance::Params{}.describeSelf(d["params"]);
        auto& opsBody = d["ops"];
        typename Instance::Ops ops;
        std::apply([&](auto&&... op) {
            (describeOp(opsBody, op), ...);
        }, ops);
    }
};

template <typename Model>
struct ModelHelper {
    static constexpr int getInstanceById(std::string_view id) {
        return impl::getTupleIndexByItemId<typename Model::Instances>(id);
    }

    template <typename Instance>
    static inline constexpr int instanceIndex = astl::tuple::type_index_v<Instance, typename Model::Instances>;

    template <typename Instance>
    static void describeInstance(OrderedDict& d, Instance) {
        auto& body = d[Instance::id];
        Instance{}.describe(body);
    }

    static void describe(OrderedDict& d) {
        d["description"] = Model::description;
        typename Model::Params{}.describeSelf(d["params"]);
        auto& instancesBody = d["instances"];
        typename Model::Instances instances;
        std::apply([&](auto&&... instance) {
            (describeInstance(instancesBody, instance), ...);
        }, instances);
    }
};

} // namespace ac::local::schema
