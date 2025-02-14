// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <doctest/doctest.h>

#include "TestSink.hpp"

#include <ac/jalog/Log.hpp>
#include <ac/jalog/LogPrintf.hpp>
#include <ac/jalog/PrintfWrap.hpp>
#include <ac/jalog/DefaultLogger.hpp>
#include <ac/jalog/Instance.hpp>

TEST_SUITE_BEGIN("jalog");

ac::jalog::Scope gscope("g", 10, 20);

AC_JALOG_DEFINE_PRINTF_FUNC(log_gscope, gscope)

TEST_CASE("default logger/scope")
{
    auto sink = std::make_shared<TestSink>();
    auto& es = sink->entries;
    ac::jalog::Instance i;
    i.setup()
        .defaultLevel(ac::jalog::Level::Info)
        .add(sink);

    AC_JALOG(Debug, "dbg", 1);
    AC_JALOG_PRINTF(Info, "info%d", 1);
    AC_JALOG(Error, "error", 1);

    ac::jalog::Scope scope("s1", 1, 2);
    AC_JALOG_SCOPE(scope, Debug, "dbg", 2);
    AC_JALOG_SCOPE(scope, Info, "info", 2);
    AC_JALOG_PRINTF_SCOPE(scope, Critical, "crit%d", 2);

    AC_JALOG_PRINTF_SCOPE(gscope, Debug, "dbg%d", 3);
    AC_JALOG_SCOPE(gscope, Error, "err", 3);

    log_gscope<ac::jalog::Level::Warning>("wrn%d", 11);

    REQUIRE(es.size() == 6);

    {
        auto& e = es[0];
        CHECK(e.scope.label().empty());
        CHECK(e.scope.id() == 0);
        CHECK(e.scope.userData == -1);
        CHECK(e.level == ac::jalog::Level::Info);
        CHECK(e.text == "info1");
    }
    {
        auto& e = es[1];
        CHECK(e.scope.label().empty());
        CHECK(e.scope.id() == 0);
        CHECK(e.scope.userData == -1);
        CHECK(e.level == ac::jalog::Level::Error);
        CHECK(e.text == "error1");
    }
    {
        auto& e = es[2];
        CHECK(e.scope.label() == "s1");
        CHECK(e.scope.id() == 1);
        CHECK(e.scope.userData == 2);
        CHECK(e.level == ac::jalog::Level::Info);
        CHECK(e.text == "info2");
    }
    {
        auto& e = es[3];
        CHECK(e.scope.label() == "s1");
        CHECK(e.scope.id() == 1);
        CHECK(e.scope.userData == 2);
        CHECK(e.level == ac::jalog::Level::Critical);
        CHECK(e.text == "crit2");
    }
    {
        auto& e = es[4];
        CHECK(e.scope.label() == "g");
        CHECK(e.scope.id() == 10);
        CHECK(e.scope.userData == 20);
        CHECK(e.level == ac::jalog::Level::Error);
        CHECK(e.text == "err3");
    }
    {
        auto& e = es[5];
        CHECK(e.scope.label() == "g");
        CHECK(e.scope.id() == 10);
        CHECK(e.scope.userData == 20);
        CHECK(e.level == ac::jalog::Level::Warning);
        CHECK(e.text == "wrn11");
    }
}
