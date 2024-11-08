# New Build Notes

Notes on the build processs with plugins.

## Dep types

* hard: always fetched and assumed present, always the same (typically add_subdirectory) regardless of flavor. This would be llama.cpp for our llama wrapper: always a subdir, tests and examples never built
* soft: may be a subdir, may be find_package, may come from "above". This would be ac-local for a plugin, or a plugin for an app

## Build types and invariants:

* CI standalone
    * Soft fetched: yes
    * Soft may be subdirs: yes
    * Soft built: yes
    * Soft tests: no
    * Own tests: yes
* Mono root
    * Soft fetched: no (paths)
    * Soft may be subdirs: yes
    * Soft built: yes
    * Soft tests: yes
    * Own tests: yes
* Mono component
    * Soft fetched: no (paths)
    * Soft may be subdirs: no (find_package)
    * Soft built: no (built by mono standalone project)
    * Soft tests: n/a (not built)
    * Own tests: yes
* Subdir
    * Soft fetched: yes if not present
    * Soft may be subdirs: yes if fetched
    * Soft built: yes if fetched
    * Soft tests: auto no if fetched
    * Own tests: default no, but may be requested
* Deploy root:
    * Soft fetched: yes
    * Soft may be subdirs: yes, but mind install
    * Soft built: yes (and installed)
    * Soft tests: yes
    * Own tests: yes, but no examples
* Deploy component:
    * Soft fetched: no (installed)
    * Soft may be subdirs: no (find_package)
    * Soft built: no (built by root)
    * Soft tests: n/a (not built)
    * Own tests: yes

## Use cases

### ac-local

No soft deps

* CI standalone: own tests
* Mono root: equivalent to ci
* Mono component: equivalent to ci
* Subdir: own tests per request
* Deploy root: equivalent to ci
* Deploy component: equivalent to ci

### inference lib

Soft dep is ac-local. May have wrappers as soft deps for own examples.

* CI standalone
    * Soft fetch: CPM by version
    * Soft add: subdir (CPM)
    * Soft build: auto (CPM)
    * Soft tests: auto no (subdir default)
* Mono root
    * Soft fetch: paths
    * Soft add: subdir
    * Soft build: auto (subdir)
    * Soft tests: request when adding subdir
* Mono component
    * Soft fetch: paths
    * Soft add: target-guard find_package (`_ROOT` must be configured from above)
    * Soft build: no
    * Soft tests: n/a
* Subdir
    * Soft fetch: target-guard CPM by version (gurads fetch)
    * Soft add: auto subdir (CPM)
    * Soft build: auto (CPM)
    * Soft tests: only if own tests are requested
* Deploy root
    * Soft fetch: CPM by version, but long form so SYSTEM is OFF. Thus soft deps would be installable
    * Soft add: subdir (CPM)
    * Soft build: auto (CPM)
    * Soft tests: request when adding subdir
    * No own examples!
* Deploy component
    * Soft fetch: assume installed
    * Soft add: find_package by prefix path
    * Soft build: assume built
    * Soft tests: n/a


### wrapper

Soft dep is ac-local. May have inference libs as soft deps for own examples

* CI standalone
    * Soft fetch: CPM by version, `DOWNLOAD_ONLY` ilibs
    * Soft add: ac-local is subdir, ilibs cross-configure + find_package
    * Soft build: ac-local auto (subdir), ilibs cross-build
    * Soft tests: ac-local auto no (subdir default), ilibs cross-cofigure with no
* Mono root
    * Soft fetch: paths
    * Soft add: ac-local is subdir, ilibs cross-configure + find_package
    * Soft build: ac-local auto (subdir), ilibs cross-build
    * Soft tests: request when configuring
* Mono component:
    * Soft fetch: paths
    * Soft add: target-guard find_package (`_ROOT` must be configured from above)
    * Soft build: no
    * Soft tests: n/a
* Subdir
    * Soft fetch: target-guard CPM by version (gurads fetch)
    * Soft add: auto subdir (CPM)
    * Soft build: auto (CPM)
    * Soft tests: only if own tests are requested
* Deploy root
    * Soft fetch: CPM by version, but long form so SYSTEM is OFF
    * Soft add: subdir
    * Soft build: auto (CPM)
    * Soft tests: request when adding subdir
    * No own examples!
* Deploy component
    * Soft fetch: assume installed
    * Soft add: find_package by prefix path
    * Soft build: assume built
    * Soft tests: n/a

### app

Soft dep is ac-local, Infernece libs and wrappers as soft deps

* CI standalone
    * Soft fetch: CPM by version, `DOWNLOAD_ONLY` ilibs
    * Soft add: subdir, ilibs cross-configure + find_package
    * Soft build: auto (subdir), ilibs cross-build
    * Soft tests: auto no (subdir default), ilibs cross-cofigure with no
* Mono root
    * Soft fetch: paths
    * Soft add: subdirs, ilibs cross-configure + find_package
    * Soft build: auto (subdir), ilibs cross-build
    * Soft tests: request when configuring
* Mono component: never
* Subdir: never
* Deploy root
    * Soft fetch: CPM by version, but long form so SYSTEM is OFF, `DOWNLOAD_ONLY` ilibs
    * Soft add: subdir, ilibs cross-configure + find_package
    * Soft build: auto (subdir), ilibs cross-build
    * Soft tests: request when adding subdir
* Deploy component
    * Soft fetch: assume installed
    * Soft add: find_package by prefix path
    * Soft build: assume built
    * Soft tests: n/a

### External build (like a mobile app)

All soft deps are deploy components - install

### Direct inference lib user APP

CPM (add subdir) ilib plugin is not built by default
