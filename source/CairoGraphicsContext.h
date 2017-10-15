#pragma once

#if JUCE_MAC
#include <CoreGraphics/CoreGraphics.h>
#elif JUCE_WIN32
#elif JUCE_LINUX
#endif

using namespace juce;

class  CairoGraphicsContext : public LowLevelGraphicsContext
{
public:
  enum CAIRO_OUTPUT
  {
    CAIRO_OUTPUT_PDF = 0,
    CAIRO_OUTPUT_PS,
    CAIRO_OUTPUT_SVG
  };
  
public:
  CairoGraphicsContext (CAIRO_OUTPUT output, const char* filename, unsigned int width, unsigned int height, unsigned int dpi = 300);
  CairoGraphicsContext (unsigned char* data, unsigned int width, unsigned int height, unsigned int stride, unsigned int dpi = 300);
  #ifdef JUCE_WIN32
  CairoGraphicsContext (HDC printerHandle, unsigned int width, unsigned int height, unsigned int dpi = 300);
  #elif  JUCE_MAC
  CairoGraphicsContext (CGContextRef quartzHandle, unsigned int width, unsigned int height, unsigned int dpi = 300);
  #endif
  ~CairoGraphicsContext();
  void nextPage (bool clearSurface);
  bool isVectorDevice() const override { return true; }

  void setOrigin (Point<int>) override;
  void addTransform (const AffineTransform&) override;
  float getPhysicalPixelScaleFactor() override;
  
  bool clipToRectangle (const Rectangle<int>&) override;
  bool clipToRectangleList (const RectangleList<int>&) override;
  void excludeClipRectangle (const Rectangle<int>&) override;
  void clipToPath (const Path&, const AffineTransform&) override;
  void clipToImageAlpha (const Image&, const AffineTransform&) override;
  
  bool clipRegionIntersects (const Rectangle<int>&) override;
  Rectangle<int> getClipBounds() const override;
  bool isClipEmpty() const override;
  
  void saveState() override;
  void restoreState() override;
  
  void beginTransparencyLayer (float opacity) override;
  void endTransparencyLayer() override;
  
  //==============================================================================
  void setFill (const FillType&) override;
  void setOpacity (float) override;
  void setInterpolationQuality (Graphics::ResamplingQuality) override;
  
  //==============================================================================
  void fillRect (const Rectangle<int>&, bool replaceExistingContents) override;
  void fillRect (const Rectangle<float>&) override;
  void fillRectList (const RectangleList<float>&) override;
  void fillPath (const Path&, const AffineTransform&) override;
  void drawImage (const Image&, const AffineTransform&) override;
  void drawLine (const Line<float>&) override;
  
  void setFont (const Font&) override;
  const Font& getFont() override;
  void drawGlyph (int glyphNumber, const AffineTransform&) override;
  bool drawTextLayout (const AttributedString&, const Rectangle<float>&) override { return false; }

private:
  cairo_t *cr;
  cairo_surface_t *surface;
  cairo_matrix_t *matrix;
  cairo_filter_t filter;
  cairo_glyph_t *glyphs;
  cairo_text_extents_t *extents;
  Font font;
  float opacity;
  // ====================================================================
  CairoGraphicsContext (const CairoGraphicsContext& other);
  const  CairoGraphicsContext& operator= (const CairoGraphicsContext&);
  // ====================================================================
};
