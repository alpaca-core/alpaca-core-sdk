# Tips

Some tips for developers.

* Make use of the [personal sandbox](../../poc/README.md#personal-sandbox).
* Make use of the gitignored `CMakeUserPresets.json` for your personal personal presets that you use often.
* Use the gitignored `<repo>/tmp` for experimental and test i/o
    * C and C++ access to `<repo>/tmp` can be acquired through the [`ac-dev::repo-root` target](../../common/ac-repo-root/README.md).
    * Especially relevant for new packages and, most importantly, models, to avoid excessive traffic and potential multiple copies on your machine
    
