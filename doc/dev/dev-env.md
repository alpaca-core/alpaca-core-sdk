# Developer Environments

On the various systems below have recommended IDEs and text editors. However, as long as you have a C++ and CMake workflow set up, you should be fine with whatever is most comfortable for you.

## Linux

VS Code is the recommended IDE here.

### Ubuntu

We use Ubuntu 24.04, but the setup will likely work for other versions as well.

* `$ sudo apt install git`
* `$ sudo apt install git-lfs`
* `$ git lfs install` - do not forget this!
* `$ sudo apt install cmake`
* `$ sudo apt install ninja-build`
* `$ sudo apt install libssl-dev` (optional for HTTPS downloads)
* `$ sudo apt install libsdl2-dev` (optional for GUI demos)
* Optionally for the java wrapper: `$ sudo apt install openjdk-17-jdk`
* Compute backends (may or may not be applicable to your system):
    * Cuda: `$ sudo apt install nvidia-cuda-toolkit`
    * Vulkan: `$ sudo apt install libvulkan-dev`

### Arch Linux

We use Manjaro so some packages might have different names in pure AUR Arch Linux.

* `$ sudo pacman -S git`
* `$ sudo pacman -S git-lfs`
* `$ git lfs install` - do not forget this!
* `$ sudo pacman -S cmake`
* `$ sudo pacman -S gcc`
* `$ sudo pacman -S ninja`
* `$ sudo pacman -S openssl` (optional for HTTPS downloads)
* `$ sudo pacman -S sdl2` (optional for GUI demos)
* Optionally for the java wrapper: `$ sudo pacman -S jdk-openjdk`
* Optionally for the swift wrapper: `$ pamac build swift-bin`
    * Note that to build the swift wrapper, you will have to configure with `-DCMAKE_CXX_COMPILER=/usr/lib/swift/bin/clang++ -DCMAKE_C_COMPILER=/usr/lib/swift/bin/clang`
* Compute backends (may or may not be applicable to your system):
    * Cuda: `$ sudo pacman -S cuda`
    * Vulkan: `$ sudo pacman -Sg vulkan-devel`

### Visual Studio Code Setup

## Windows

We use Windows 11, but the setup will likely work for Windows 10 as well.

On Windows we recommend using Visual Studio or VS Code.

Install the following software (we recommend using Chocolatey, Scoop, or WinGet, but manually is also fine):

* [git](https://git-scm.com/downloads/win)
* [CMake](https://cmake.org/download/) 3.24 or later
* OpenSSL 3 (Optional for HTTPS downloads). Windows binary builds available [here](https://slproweb.com/products/Win32OpenSSL.html) (Install the Win64 non-light version))
    * set the `OPEN_SSL_ROOT_DIR` environment variable to the OpenSSL installation directory
* SDL2 (Optional, for GUI demos and tools). If you also install the Vulkan SDK, follow the steps for it below. They also provide SDL2. Alternatively:
    * Binary builds available [here](https://github.com/libsdl-org/SDL/releases) (note that first entries there will be for SDL3, scroll to find the latest SDL2 release).         
    * Add the SDL2 installation directory to the `CMAKE_PREFIX_PATH` environment variable 
    * It's a good idea to add it to the system path as well
* For Android builds (optional):
    * Android Studio with its SDK manager is recommended, but not required
    * Install SDK with API Level at least 32 and NDK
    * [Android gradle plugin](https://developer.android.com/build/releases/gradle-plugin)
    * Set the `ANDROID_HOME` environment variable to the Android SDK installation directory
    * Set the `ANDROID_NDK_ROOT` environment variable to the Android NDK installation directory
* For the Java wrapper (optional):
    * JDK. [Microsoft OpenJDK 17](https://learn.microsoft.com/en-us/java/openjdk/download#openjdk-17) works, but any other JDK 17 or later should be fine as well
    * Set the `JAVA_HOME` environment variable to the JDK installation directory 
* For the Swift wrapper (optional): 
    * Install Swift in any of the ways described [here](https://www.swift.org/install/windows)
    * Note that on you need to use the clang compiler shipped with your swift installation
    * Also note that installing the Swift will add clang++ to your system path. If you also use other clang compilers, you will have to juggle the system path around accorting to what you currently need.
* Optional compute backends which may or may not be applicable to your system:
    * [CUDA 12](https://developer.nvidia.com/cuda-downloads)
    * Vulkan SDK
        * [LunarG Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
        * The installer will set most environment variables for you, but you need to add `%VULKAN_SDK%\cmake` to `CMAKE_PREFIX_PATH`
        * The step above also provides SDL2

Enable git LFS: `> git lfs install`

### Visual Studio Setup

* Install Visual Studio 2022 with the following components:
    * Desktop development with C++
    * C++ CMake tools for Windows
* Even if you want to target Android, you do *not* need the Android development components. 
    * If you already have them, it's fine. You do not need to uninstall them.
    * We don't support android builds through Visual Studio
* Use "open folder" in Visual Studio to open the project directory, configure, and build.
    * Alternatively configuring with a Visual Studio generator will likely work, but it's not supported

### VS Code Setup

* Install [ninja](https://ninja-build.org/)
    * Add the installation directory to your system path
* Install a C++20 compiler
    * If you also have Visual Studio 2022 installed, you can use the MSVC compiler that comes with it
    * Portable versions of MSVC are also available in many places. For example, [here](https://github.com/Data-Oriented-House/PortableBuildTools)
    * [MSYS2](https://www.msys2.org/) with MinGW-w64 11 or later is also supported
    * [Clang](https://github.com/llvm/llvm-project/releases) 14 or later is also supported
* If you have not installed Visual Studio 2022, you'll need to add the compiler binaries to your system path
* Add the following extensions to VS Code:
    * CMake
    * CMake Tools
    * CMake Language Support
    * C/C++
* Open the project directory in VS Code, configure, and build.

## macOS

We use macOS Sequoia, but the setup will likely work for earlier versions as well.

Note, however, that we do not support x86 macOS. We only support Apple Silicon.

On macOS we recommend using VS Code or Xcode.

Technically you can setup the requirements manually, but, please, just use [Homebrew](https://brew.sh/).

* `$ brew install git`
* `$ brew install git-lfs`
* `$ git lfs install` - do not forget this!
* `$ brew install cmake`
* `$ brew install ninja`
* Install Xcode and developer tools
    * `$ xcode-select --install`
    * Optionally for iOS development: install the iOS SDK and tools
* `$ brew install openssl@3` (optional for HTTPS downloads)    
* Optionally for GUI demos: `$ brew install sdl2`
* Optionally for Android development:
    * Install Android Studio
    * Install SDK with API Level at least 32 and NDK
    * Set the `ANDROID_NDK_ROOT` environment variable to the Android NDK installation directory
* Optionally for the Java wrapper: `$ brew install openjdk` or `$ brew install openjdk@17` (both should work)

### Visual Studio Code Setup

* Add the following extensions to VS Code:
    * CMake
    * CMake Tools
    * CMake Language Support
    * C/C++
* Open the project directory in VS Code, configure, and build.

### Xcode Setup
