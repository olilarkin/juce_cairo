# Cairo Module for JUCE

This is a JUCE Module for drawing using cairo

Only works on macOS for now, but eventually should work on Linux and Windows

See example CLI project https://github.com/olilarkin/juce_cairo_test

## Building:

Instructions for building on various platforms:

---
### macOS

**Mac Ports**

https://www.macports.org/

Mac Ports is used to build many of the dependencies. Ports should be built with the ``+universal`` variant and macosx_deployment_target should be set in ```/opt/local/etc/macports/macports.conf``` to match the OSX Deployment Target setting in the .jucer project. e.g.

```macosx_deployment_target 10.9```

the .jucer project expects ports to be installed in certain locations. If you have customized macports configuration, you may have to adjust the .jucer project in order to build.

even on a fast machine the following will take a long time to build! Be prepared to leave your computer for over an hour!

To do everything in one go you can use the following command:

```sudo port install cairo +universal```

**cairo**

Install via Macports e.g. ```sudo port install cairo +universal```

The .jucer projects expects the libraries to be in ```/opt/local/lib```

#### Projucer settings for projects using the module:

When using the juce_cairo module you will need to add some things to your main .jucer project:

**Extra Linker Flags:**

In order to force static linking of the libraries, the full path to the the static libraries can be added in the .jucer "extra linker flags section", without a -l in front of the library name.

```  
/opt/local/lib/libcairo.a  
```

**Header search paths:**

```
/opt/local/include
```

**Extra Library search paths:**

```
/opt/local/lib
```

---
### Windows

TODO

---

### Linux

TODO

---

# License

The core of this module is heavily based on work posted to the JUCE forum by maxprod

https://forum.juce.com/t/how-to-work-with-3rd-party-lowlevelgraphicscontext-cairo-agg/12469/3

Written by Oliver Larkin    
License: BSD
www.olilarkin.co.uk
