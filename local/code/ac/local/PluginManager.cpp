// Copyright (c) Alpaca Core
// SPDX-License-Identifier: MIT
//
#include "PluginManager.hpp"
#include "PluginInterface.hpp"
#include "ModelLoaderRegistry.hpp"
#include "Logging.hpp"
#include "Version.hpp"

#include <astl/throw_stdex.hpp>
#include <astl/qalgorithm.hpp>
#include <astl/sentry.hpp>

#include <cstdlib>
#include <filesystem>
#include <type_traits>

// this is required to avoid warnings when returning PluginInterface from extern "C" functions
static_assert(std::is_trivial_v<ac::local::PluginInterface>, "PluginInterface must be trivial");

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

inline hplugin load_plugin(const char* filename) {
    return dlopen(filename, RTLD_NOW | RTLD_LOCAL);
}

#define unload_plugin dlclose
#define get_proc dlsym

#endif

namespace ac::local {

PluginManager::PluginManager(ModelLoaderRegistry& registry)
    : m_registry(registry)
{}

PluginManager::~PluginManager() = default;

std::string_view PluginManager::pluginPathToName(std::string_view path) {
    if (auto fnamePos = path.find_last_of("/\\"); fnamePos != std::string_view::npos) {
        path.remove_prefix(fnamePos + 1);
    }
    if (path.starts_with("aclp-")) {
        path.remove_prefix(5);
    }
    if (path.ends_with(".dll")) {
        path.remove_suffix(4);
    }
    else if (path.ends_with(".so")) {
        path.remove_suffix(3);
    }
    else if (path.ends_with(".dylib")) {
        path.remove_suffix(6);
    }
    return path;
}

void PluginManager::addPluginDir(std::string_view dir) {
    AC_LOCAL_LOG(Info, "Adding plugin directory ", dir);
    m_pluginDirs.emplace_back(dir);
}

void PluginManager::addPluginDirsFromEnvVar(std::string envVar) {
    AC_LOCAL_LOG(Info, "Adding plugin directories from $", envVar);
    auto pdirs = std::getenv(envVar.c_str());
    if (!pdirs) {
        AC_LOCAL_LOG(Info, "Environment variable ", envVar, " not set");
        return;
    }

    std::string_view dirs(pdirs);
#if defined(_WIN32)
    constexpr char sep = ';';
#else
    constexpr char sep = ':';
#endif

    while (!dirs.empty()) {
        auto end = dirs.find(sep);
        if (end == std::string_view::npos) {
            end = dirs.size();
        }

        addPluginDir(dirs.substr(0, end));
        dirs.remove_prefix(end + 1);
    }
}

const PluginInfo* PluginManager::loadPlugin(const std::string& path, LoadPluginCb cb) {
    return tryLoadPlugin(path, cb);
}

void PluginManager::loadPlugins(LoadPluginCb cb) {
    for (auto& dir : m_pluginDirs) {
        AC_LOCAL_LOG(Info, "Loading plugins from ", dir);
        for (auto& entry : std::filesystem::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue; // not a file
            const auto& path = entry.path();
            auto ext = path.extension().string();
            if (ext != ".dll" && ext != ".so" && ext != ".dylib") continue; // not a shared library
            auto fname = path.filename().string();
            if (!fname.starts_with("aclp-")) continue; // not our plugin
            tryLoadPlugin(path.string(), cb);
        }
    }
}

namespace {

const PluginInfo* findPluginByPath(const std::vector<PluginInfo>& plugins, std::string_view path) {
    return astl::pfind_if(plugins, [&path](const PluginInfo& p) { return p.fullPath == path; });
}

const char* GetAcLocalVersionFunc_name = "aclp_ac_local_version";
const char* PluginLoadFunc_name = "aclp_get_interface";

} // namespace

inline jalog::BasicStream& operator,(jalog::BasicStream& s, const astl::version& ver) {
    return s, ver.major, '.', ver.minor, '.', ver.patch;
}

const PluginInfo* PluginManager::tryLoadPlugin(const std::string& path, LoadPluginCb& cb) {
    if (cb.pathFilter && !cb.pathFilter(path)) {
        AC_LOCAL_LOG(Info, "User filtered plugin by path: ", path);
        return nullptr;
    }

    auto name = pluginPathToName(path);
    if (cb.nameFilter && !cb.nameFilter(name)) {
        AC_LOCAL_LOG(Info, "User filtered plugin by name: ", path);
        return nullptr;
    }

    if (auto p = findPluginByPath(m_plugins, path)) {
        AC_LOCAL_LOG(Info, "Plugin ", path, " already loaded");
        return p;
    }

    hplugin hplugin = load_plugin(path.c_str());
    if (!hplugin) {
        AC_LOCAL_LOG(Warning, "Failed to load plugin ", path);
        return nullptr;
    }
    astl::sentry close([&] {
        if (hplugin) unload_plugin(hplugin);
    });

    auto getVer = (PluginInterface::GetAcLocalVersionFunc)get_proc(hplugin, GetAcLocalVersionFunc_name);
    if (!getVer) {
        AC_LOCAL_LOG(Warning, "Failed to find ", GetAcLocalVersionFunc_name, " in ", path);
        return nullptr;
    }

    auto pluginVersion = astl::version::from_int(getVer());
    if (pluginVersion != Project_Version) {
        AC_LOCAL_LOG(Warning, "Plugin ", path, " was built with incompatible ac-local version: ", pluginVersion,
            ". Own version is ", Project_Version);
        return nullptr;
    }

    auto load = (PluginInterface::GetFunc)get_proc(hplugin, PluginLoadFunc_name);
    if (!load) {
        AC_LOCAL_LOG(Warning, "Failed to find ", PluginLoadFunc_name, " in ", path);
        return nullptr;
    }

    auto interface = load();
    AC_LOCAL_LOG(Info, "Loaded plugin ", path,
        "\n    name: ", name,
        "\n   label: ", interface.label,
        "\n     ver: ", interface.version,
        "\n  vendor: ", interface.vendor,
        "\n    desc: ", interface.desc
    );

    if (cb.interfaceFilter && !cb.interfaceFilter(interface)) {
        AC_LOCAL_LOG(Info, "User filtered plugin by interface", path);
        return nullptr;
    }

    void* pluginRawData= nullptr;
    if (interface.init) try {
        pluginRawData= interface.init();
    }
    catch (const std::exception& ex) {
        AC_LOCAL_LOG(Error, "Failed to init plugin: ", ex.what());
        return nullptr;
    }
    catch (...) {
        AC_LOCAL_LOG(Error, "Failed to init plugin: unknown exception");
        return nullptr;
    }

    PluginInfo& info = m_plugins.emplace_back();

    info.fullPath = path;
    info.name = name;
    info.nativeHandle = hplugin;
    hplugin = nullptr; // release sentry

    info.rawData = pluginRawData;
    info.loaders = interface.getLoaders();

    for (auto& loader : info.loaders) {
        m_registry.addLoader(*loader, &info);
    }

    cb.onPluginLoaded(info);

    return &info;
}

} // namespace ac::local
