dansSuperMetroidLibrary
=======================
Dan's Super Metroid Library aims to be a Super Metroid ROM reader, editor, and writer. By editor, I do not mean a human interface, but functions that ease editing. For example, putDoorHere(where, destinationRoom, etc) might be a function eventually.

The entirety of the library exists in sm.hpp and sm.cpp.

viewer.cpp uses the library and SFML to create a Super Metroid viewer. Copy an SFML source release to neighbor CMakeLists.txt, and rename it from SFML-some-version to SFML.
