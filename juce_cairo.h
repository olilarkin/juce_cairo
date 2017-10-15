/*
  This file is part of the juce_cairo JUCE module
  Written by Oliver Larkin
  www.olilarkin.co.uk
*/

/*******************************************************************************

 BEGIN_JUCE_MODULE_DECLARATION

  ID:               juce_cairo
  vendor:           olilarkin
  version:          0.1
  name:             Cairo for JUCE
  description:      Use Cairo in JUCE Projects.
  website:          https://github.com/olilarkin/juce_cairo
  license:          BSD

  dependencies:     juce_core, juce_graphics
  
  OSXLibs:          cairo
  
 END_JUCE_MODULE_DECLARATION

*******************************************************************************/

#if __APPLE__
#include <cairo/cairo.h>
#include <cairo/cairo-quartz.h>
#include <cairo/cairo-pdf.h>
#include <cairo/cairo-ps.h>
#include <cairo/cairo-svg.h>
#endif
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>

#include "source/CairoGraphicsContext.h"
