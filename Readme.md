GRUTinizer
==========

A generic unpacker and analysis package for gamma-ray spectroscopy.

The doxygen documentation can be found [here](https://pcbend.github.io/GRUTinizer).

Installation depends on choice of ROOT version.

If you are still using ROOT5, you should compile using the master branch.

If you are using ROOT6, you should compile on the root6 branch. Note that this has been tested on ROOT 6.06.08 and 6.08.02. If using ROOT 6.06.08, you must be using a GCC version earlier than GCC5.2, because GCC introduced a new ABI that is incompatible with the version of clang used in 6.06.08. 

ROOT should be installed from source, with python and mathmore features enabled.

Your mileage may vary on other ROOT versions. ROOT versions from beyond ROOT6.08 should compile (up to ROOT6.12) , but will not behave exactly as expected. Particularly, if you compile on ROOT6.12, you can still use GRUTinizer but the GUI will not function correctly.
