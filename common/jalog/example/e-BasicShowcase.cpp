// Copyright (c) Borislav Stanimirov
// SPDX-License-Identifier: MIT
//
#include <ac/jalog/Instance.hpp>
#include <ac/jalog/sinks/DefaultSink.hpp>
#include <ac/jalog/Log.hpp>
#include <ac/jalog/LogPrintf.hpp>

#include <string>
#include <string_view>

struct Person
{
    std::string name;
    int age;
};

std::ostream& operator<<(std::ostream& o, const Person& p)
{
    return o << p.name << '(' << p.age << ')';
}

int main()
{
    ac::jalog::Instance jl;
    jl.setup().add<ac::jalog::sinks::DefaultSink>();

    AC_JALOG(Debug, "Log integers: ", 34, ", or in a custom base: ", ac::jalog::base<16>(255));
    AC_JALOG(Info, "Log floating point numbers with no precision loss: ", 12.4356631);

    std::string str = "my string";
    std::string_view sv = std::string_view(str).substr(0, 6);
    AC_JALOG(Warning, "Log strings: '", str, "' and string views '", sv, "'");

    Person alice = {"Alice", 34};
    AC_JALOG(Error, "Log types with custom ostream output: ", alice);

    AC_JALOG_PRINTF(Critical, "Log printf style: %d, %.3f, %s", 43, 3.14159, str.c_str());

    return 0;
}
