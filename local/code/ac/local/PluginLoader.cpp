// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "PluginLoader.hpp"
#include "Version.hpp"
#include <astl/throw_stdex.hpp>

#if defined (_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef HMODULE hplugin;

#define load_plugin LoadLibraryA
#define unload_plugin FreeLibrary
#define get_proc GetProcAddress

#else

#include <dlfcn.h>

typedef void* hplugin;

hplugin load_plugin(const char* filename) {
    return dlopen(filename, RTLD_NOW | RTLD_LOCAL);
}

#define unload_plugin dlclose
#define get_proc dlsym

#endif

namespace ac::local {

static const char* GetAcLocalVersionFunc = "aclp_ac_local_version";
static const char* PluginLoadFunc_name = "aclp_get_interface";

PluginInterface PluginLoader::loadPlugin(const std::string& filename) {
    hplugin h = load_plugin(filename.c_str());
    if (!h) {
        throw_ex{} << "Failed to load plugin " << filename;
    }

    auto getVer = (PluginInterface::GetAcLocalVersionFunc)get_proc(h, GetAcLocalVersionFunc);
    if (!getVer) {
        unload_plugin(h);
        throw_ex{} << "Failed to find " << GetAcLocalVersionFunc << " in " << filename;
    }

    if (getVer() != Project_Version.to_int()) {
        unload_plugin(h);
        throw_ex{} << "Plugin " << filename << " was built with incompatible ac-local version";
    }

    auto f = (PluginInterface::GetFunc)get_proc(h, PluginLoadFunc_name);
    if (!f) {
        unload_plugin(h);
        throw_ex{} << "Failed to find " << PluginLoadFunc_name << " in " << filename;
    }

    auto iface = f();

    return iface;
}

} // namespace ac::local
