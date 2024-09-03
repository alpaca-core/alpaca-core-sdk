# Mac OS Development environment setup

## First steps

* Install [VS code](https://code.visualstudio.com/docs/setup/mac)
* Install [Brew package manager](https://brew.sh/)
    * **notice: follow the instructions after installation, in order to add brew to the PATH variable**
* Install [Xcode](https://developer.apple.com/xcode/) and developer tools for both desktop & iOS targets
* Install [Git for MacOS](https://git-scm.com/download/mac)
* Add git extension [Git LFS](https://git-lfs.com/)
* `Optional:` Install [Git Desktop](https://desktop.github.com/download/) \simplifies clone ops from GitHub, especially if you haven't setup SSH keys. Also setups local git user & mail.\
* Install __CMake__ - using `brew install cmake`
* Install __Ninja__ - using `brew install ninja`
* Install __SDL2__ - command `brew install sdl2`
* Install __Doxygen__ - command `brew install doxygen`
* Clone needed repositories e.g. alpca-core

## Prepare VS code for development

* Install **CMake** extensions (*CMake*, *CMake Tools*, *CMake Language Support*) - directly from VS code -> extensions
* **C/C++** compiler - `clang`, shall be available from the XCode installation
* Add C/C++ extension pack from VS code
* Configure CMake toolchains - from VS code command palette `F1 or Ctrl + Shift + P` run `CMake: Configure` - and select your toolchain.
* Once configured, you shall be able to build the project's targets


---
