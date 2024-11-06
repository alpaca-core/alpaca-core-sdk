# Building AC Local Plugins with Shared Libraries

Most AC Local plugins, when configured with `BUILD_SHARED_LIBS` set to `ON`, will emit a warning:

>  Building AC Local plugins with BUILD_SHARED_LIBS ON is dangerous!

If the plugin requires some shared libraries and other plugins require *other* versions of the same libraries, this may lead to a huge mess of conflicting dependencies, ODR violations, spurious crashes, and other undefined behavior.

It is true that with careful management of SONAME and RPATH on POSIX systems, and DLL versions and manifests on Windows, the potential mess can be avoided. However this is a complex and error-prone process, and it is easy to make mistakes. Not all developers are familiar with the intricacies of shared library management, and even those who are can make mistakes.

So, unless you are very sure of what you are doing, it is best to avoid building AC Local plugins with shared libraries. We recommend to statically link everything into the plugin.
