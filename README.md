dansSuperMetroidLibrary
=======================
This is a work in progress.

The goal is to create a Super Metroid ROM reader, editor, and writer. By editor, I do not mean a human interface, but functions that ease editing. For example, putDoorHere(where, destinationRoom, etc) might be a function eventually.

The entirety of the library exists in sm.hpp and sm.cpp.

viewer.cpp uses the library and SFML 2.0 RC to create a Super Metroid viewer. Right click on a door to enter it.
