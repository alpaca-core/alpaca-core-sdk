# Copyright (c) Alpaca Core
# SPDX-License-Identifier: MIT
#
macro(asset_mgr_test test)
    add_doctest_lib_test(${test} ac-asset-mgr t-${test}.cpp
        LIBRARIES
            ac-dev::test-assets
            ac-dev::test-util
    )
endmacro()

asset_mgr_test(FsUtil)
asset_mgr_test(AssetSourceLocalDir)
asset_mgr_test(AssetSourceLocalFiles)
asset_mgr_test(AssetManager)
