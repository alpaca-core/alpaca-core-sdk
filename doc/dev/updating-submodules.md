# Updating Submodules

Throughout the process, document changes made in the relevant issue, as a comment.

* Update all forks to the latest version of the upstream master branch.
    * If there are active branches on our side, update them as well.
* Redirect the submodules to the latest commit of the fork's relevant branch 
* Make sure everything builds 
    * On build errors in ggml, look for the problem in our configuration
    * On build errors in other submodules, it may be the case that they are not compatible with the ggml version. If this is the case, move to a commit which is. It may also be configuration.
    * On build errors in our code, search for the relevant changes in the commit history of the submodule