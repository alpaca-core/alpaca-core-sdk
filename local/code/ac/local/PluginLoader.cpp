// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "PluginLoader.hpp"
#include <astl/throw_stdex.hpp>

#if defined (_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef HMODULE hplugin;

inline hplugin load_plugin(const std::string& fname) {
    return LoadLibraryA(fname.c_str());
}

#define unload_plugin FreeLibrary
#define get_proc GetProcAddress

#else

#include <dlfcn.h>

typedef void* hplugin;

inline hplugin load_plugin(std::string_view dir, std::string_view lib) {
    std::string l{dir};
    l += "/lib";
    l += lib;
//#if defined(__APPLE__)
    // l += ".dylib";
//#else
    // cmake MODULE libraries have a .so extension on both linux and mac
    l += ".so";
// #endif
    return dlopen(l.c_str(), RTLD_NOW | RTLD_LOCAL);
}
#define unload_plugin dlclose
#define get_proc dlsym

#endif

namespace ac::local {

static const char* PluginLoadFunc_name = "acLocalPluginLoad";

PluginInterface PluginLoader::loadPlugin(std::string_view path, std::string_view lib) {
    std::string fname{path};
    fname += '/';
#if defined(__GNUC__)
    fname += "lib";
#endif
    fname += lib;
    fname += ".dll";
    return loadPlugin(fname);
}

PluginInterface PluginLoader::loadPlugin(const std::string& filename) {
    hplugin h = load_plugin(filename);
    if (!h) {
        throw_ex{} << "Failed to load plugin " << filename;
    }


    auto f = (PluginInterface::PluginLoadFunc)get_proc(h, PluginLoadFunc_name);
    if (!f) {
        unload_plugin(h);
        throw_ex{} << "Failed to find " << PluginLoadFunc_name << " in " << filename;
    }

    auto iface = f();

    if (iface.acLocalVersion != Project_Version) {
        unload_plugin(h);
        throw_ex{} << "Plugin " << filename << " was built with incompatible ac-local version";
    }

    return iface;
}

} // namespace ac::local
