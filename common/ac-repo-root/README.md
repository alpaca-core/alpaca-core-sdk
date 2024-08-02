# ac-repo-root

Provides access to to the repository root directory to C and C++ code.

## Usage

**CMake**

```cmake
target_link_libraries(<your-target> PRIVATE ac-dev::repo-root)
```

**C/C++**

```c
#include "ac-repo-root.h"

const char* tmp_dir = AC_REPO_ROOT "/tmp";
```
