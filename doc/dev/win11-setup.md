# WIN 11 Development environment setup

## First steps

* Install [VS code](https://code.visualstudio.com/docs/setup/windows)
* Install [Git for Win](https://git-scm.com/download/win)
* `Optional:` Install [Git Desktop](https://desktop.github.com/download/) \simplifies clone ops from GitHub, especially if you haven't setup SSH keys. Also setups local git user & mail.\
* Install [CMake](https://cmake.org/download/) 
* Install [Ninja](https://github.com/ninja-build/ninja/wiki/Pre-built-Ninja-packages) 
    * **notice: system restart may be required**
* Install [SDL2](https://github.com/libsdl-org/SDL/releases) 
    * for MinGW use version similar to SDL2-devel-2.30.6-mingw.zip
    * extract to `some directory of your choice`
    * add the dir to local ENV var `name: CMAKE_PREFIX_PATH`: `value: <the extracted dir>`


* Clone needed repositories e.g. alpca-core 

## Prepare VS code for development

* Install **CMake** extensions (*CMake*, *CMake Tools*, *CMake Language Support*) - directly from VS code -> extensions
* Install **C/C++** compiler of your choice - in my case *MinGW (gcc)* -> follow this [tutorial](https://code.visualstudio.com/docs/cpp/config-mingw)
    * Verify installation - you shall be able to execute `gcc --version` from CMD/PS 
        * **notice: update PATH variable**
* Add C/C++ extension pack from VS code 

* Configure CMake toolchains - from VS code command palette `F1 or Ctrl + Shift + P` run `CMake: Configure` - and select your toolchain.

* Once configured, you shall be able to build the project's targets 


--- 

On any issues or suggestions - contact person: [Ivan Filipov](https://github.com/IvanFilipov)
