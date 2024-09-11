# Updating Submodules

Throughout the process, document changes made in the relevant issue, as a comment.

* Update all forks to the latest version of the upstream master branch.
    * If there are active branches on our side, update them as well.
* Redirect the submodules to the latest commit of the fork's relevant branch.
* Make sure everything builds 
    * On build errors in ggml, look for the problem in our configuration
    * On build errors in other submodules, it may be the case that they are not compatible with the ggml version. If this is the case, move to a commit which is. It may also be configuration.
    * On build errors in our code, search for the relevant changes in the commit history of the submodule and apply fixes
* Make sure tests pass and examples work. Fix if necessary.
* And now, the tedious part...
* Go through every commit in the submodule's history and check if it is relevant to our project. Add issues or immediately apply changes to reflect:
    * New features
    * Deprecations and feature removals
    * Implementations of features we already have implemented ourselves
    * Code quality improvements which can be applied to our code
    * As always, use judgement
* Once everything is done and documented, close the update issue and you're done!
