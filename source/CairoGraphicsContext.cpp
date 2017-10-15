// ===========================================================================

// ===========================================================================
#include "CairoGraphicsContext.h"
// ===========================================================================
 CairoGraphicsContext::CairoGraphicsContext (unsigned char* data, unsigned int width, unsigned int height, unsigned int stride, unsigned int dpi)
{
  cairo_debug_reset_static_data ();
  surface = cairo_image_surface_create_for_data (data, CAIRO_FORMAT_ARGB32, width, height, stride);
  cairo_surface_set_fallback_resolution (surface, dpi, dpi);
  cr = cairo_create (surface);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL);
}
// ===========================================================================
CairoGraphicsContext::CairoGraphicsContext (CairoGraphicsContext::CAIRO_OUTPUT output, const char *filename, unsigned int width,  unsigned int height, unsigned int dpi)
{
  cairo_debug_reset_static_data ();
  switch (output)
  {
    case CAIRO_OUTPUT_PDF:
      surface = cairo_pdf_surface_create (filename, width, height);
      break;
    case CAIRO_OUTPUT_PS:
      surface = cairo_ps_surface_create (filename, width, height);
      break;
    case CAIRO_OUTPUT_SVG:
      surface = cairo_svg_surface_create (filename, width, height);
      break;
    default:
      surface = NULL;
      break;
  }
  cairo_surface_set_fallback_resolution (surface, dpi, dpi);
  cr = cairo_create (surface);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL);
}
//=============================================================================
#ifdef JUCE_WIN32
//=============================================================================
 CairoGraphicsContext::CairoGraphicsContext (HDC printerHandle, unsigned int /*width*/,  unsigned int /*height*/, unsigned int dpi)
{
  cairo_debug_reset_static_data ();
  surface = cairo_win32_printing_surface_create (printerHandle);
  cairo_surface_set_fallback_resolution (surface, dpi, dpi);
  cr = cairo_create (surface);
  cairo_set_antialias (cr, CAIRO_ANTIALIAS_SUBPIXEL);
}
//=============================================================================
#elif JUCE_MAC
//=============================================================================
 CairoGraphicsContext::CairoGraphicsContext (CGContextRef quartzHandle, unsigned int width, unsigned int height, unsigned int dpi)
{
  cairo_debug_reset_static_data ();
  surface = cairo_quartz_surface_create_for_cg_context (quartzHandle, width, height);
  cairo_surface_set_fallback_resolution (surface, dpi, dpi);
  cr = cairo_create (surface);
  cairo_set_antialias(cr, CAIRO_ANTIALIAS_SUBPIXEL);
}
//=============================================================================
#endif
//=============================================================================
 CairoGraphicsContext::~CairoGraphicsContext ()
{
  cairo_destroy (cr);
  cairo_surface_flush (surface);
  cairo_surface_destroy (surface);
}
//=============================================================================
void CairoGraphicsContext::nextPage (bool clearSurface)
{
  if (clearSurface)
    cairo_show_page (cr);
  else
    cairo_copy_page (cr);
}

static cairo_matrix_t* getMatrix (const AffineTransform& at)
{
  cairo_matrix_t *matrix = nullptr;
  cairo_matrix_init (matrix, at.mat00, at.mat10, at.mat01, at.mat11, at.mat02, at.mat12);
  return matrix;
}

static void drawPath (cairo_t *cr, const Path& path, const AffineTransform& transform)
{
  Path mpath (path);
  mpath.applyTransform (transform);
  Path::Iterator i (mpath);
  while (i.next())
  {
    switch (i.elementType)
    {
      case Path::Iterator::startNewSubPath:
        cairo_new_sub_path (cr);
        cairo_move_to (cr, i.x1, i.y1);
        break;
      case Path::Iterator::lineTo:
        cairo_line_to (cr, i.x1, i.y1);
        break;
      case Path::Iterator::quadraticTo:
        cairo_curve_to (cr, i.x1, i.y1, i.x2, i.y2, i.x2, i.y2);
        break;
      case Path::Iterator::cubicTo:
        cairo_curve_to (cr, i.x1, i.y1, i.x2, i.y2, i.x3, i.y3);
        break;
      case Path::Iterator::closePath:
        cairo_close_path (cr);
        break;
      default:
        jassertfalse;
        break;
    }
  }
}
//=============================================================================
void CairoGraphicsContext::setOrigin (Point<int> o)
{
  cairo_surface_set_device_offset (surface, o.x, o.y);
}

void CairoGraphicsContext::addTransform (const AffineTransform& transform)
{
  cairo_transform (cr, getMatrix (transform));
}

float  CairoGraphicsContext::getPhysicalPixelScaleFactor()
{
  return 1.0f;
}

static juce::Rectangle<int> getBounds (cairo_t *cr)
{
  RectangleList<int> bounds;
  cairo_rectangle_list_t* list = cairo_copy_clip_rectangle_list (cr);
  for (int i=0; i < list->num_rectangles; ++i)
  {
    cairo_rectangle_t rect = list->rectangles[i];
    bounds.add (juce::Rectangle<int>(roundToIntAccurate(rect.x),
                     roundToIntAccurate(rect.y),
                     roundToIntAccurate(rect.width),
                     roundToIntAccurate(rect.height)));
  }
  return bounds.getBounds ();
}

bool  CairoGraphicsContext::clipToRectangle (const juce::Rectangle<int>& r)
{
  cairo_reset_clip (cr);
  cairo_rectangle (cr, r.getX(), r.getY(), r.getWidth(), r.getHeight());
  cairo_clip (cr);
  return (cairo_status (cr) == CAIRO_STATUS_SUCCESS) ? true : false;
}

bool  CairoGraphicsContext::clipToRectangleList (const RectangleList<int>&)
{
  return true;
}

void CairoGraphicsContext::excludeClipRectangle (const juce::Rectangle<int>& r)
{
}

void CairoGraphicsContext::clipToPath (const Path& path, const AffineTransform& transform)
{
  cairo_new_path (cr);
  drawPath (cr, path, transform);
  cairo_clip (cr);
}

void CairoGraphicsContext::clipToImageAlpha (const Image& sourceImage, const AffineTransform& transform)
{
  int width = sourceImage.getWidth();
  int height = sourceImage.getHeight();
  Image::BitmapData datas (sourceImage, 0, 0, width, height);
  unsigned char* buffer = (unsigned char*)datas.getPixelPointer (0, 0);
  cairo_surface_t *image = cairo_image_surface_create_for_data (buffer, CAIRO_FORMAT_ARGB32, width, height, width*4);
  cairo_set_matrix (cr, getMatrix(transform));
  cairo_mask_surface (cr, image, 0, 0);
}

bool  CairoGraphicsContext::clipRegionIntersects (const juce::Rectangle<int>& r)
{
  return true;
}

juce::Rectangle<int> CairoGraphicsContext::getClipBounds() const
{
  return getBounds(cr);
}

bool  CairoGraphicsContext::isClipEmpty() const
{
  return getBounds(cr).isEmpty();
}

void CairoGraphicsContext::saveState()
{
  cairo_save (cr);
}

void CairoGraphicsContext::restoreState()
{
  cairo_restore (cr);
}

void CairoGraphicsContext::beginTransparencyLayer (float opacity)
{
  cairo_push_group_with_content (cr, CAIRO_CONTENT_COLOR_ALPHA);
  setOpacity (opacity);
}

void CairoGraphicsContext::endTransparencyLayer()
{
  cairo_pop_group_to_source (cr);
}

void CairoGraphicsContext::setFill (const FillType& fillType)
{
  if (fillType.isColour())
  {
    Colour colour = fillType.colour;
    cairo_set_source_rgba (cr, (colour.getRed()/255.),
                 (colour.getGreen()/255.),
                  (colour.getBlue()/255.),
                 (colour.getAlpha()/255.));
  }
  else
  if (fillType.isGradient())
  {
//    ColourGradient *gradient = fillType.gradient;
//    Point lt, float gt,  p1 = gradient-& gt, point1;
//    Point lt, float gt,  p2 = gradient-& gt, point2;
//    float radius = 10.0f;
//    cairo_pattern_t *pattern;
//    if (gradient-& gt, isRadial)
//      pattern = cairo_pattern_create_radial (p1.getX(), p1.getY(), radius,
//                           p2.getX(), p2.getY(), radius);
//    else
//      pattern = cairo_pattern_create_linear (p1.getX(), p1.getY(),
//                           p2.getX(), p2.getY());
//    cairo_pattern_set_filter (pattern, filter);
  }
  else
  if (fillType.isTiledImage())
  {
    int width = fillType.image.getWidth();
    int height = fillType.image.getHeight();
    Image::BitmapData datas (fillType.image, 0, 0, width, height);
    unsigned char* buffer = (unsigned char*)datas.getPixelPointer (0, 0);
    cairo_surface_t *image = cairo_image_surface_create_for_data 
              (buffer, CAIRO_FORMAT_ARGB32, width, height, width*4);
    cairo_pattern_t *pattern = cairo_pattern_create_for_surface (image);
    cairo_pattern_set_filter (pattern, filter);
    cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
    cairo_pattern_set_matrix (pattern, getMatrix (fillType.transform));
  }
  else
  if (fillType.isInvisible())
  {
  }
}

void CairoGraphicsContext::setOpacity (float newOpacity)
{
  opacity = newOpacity;
}

void CairoGraphicsContext::setInterpolationQuality (Graphics::ResamplingQuality quality)
{
  if (quality == Graphics::lowResamplingQuality)
    filter = CAIRO_FILTER_FAST;
  else
  if (quality == Graphics::mediumResamplingQuality)
    filter = CAIRO_FILTER_GOOD;
  else
  if (quality == Graphics::highResamplingQuality)
    filter = CAIRO_FILTER_BEST;
  else
    filter = CAIRO_FILTER_GOOD;
}

void CairoGraphicsContext::fillRect (const Rectangle<int>& r, bool replaceExistingContents)
{
//  if (replaceExistingContents)
//  {
    cairo_rectangle (cr, r.getX(), r.getY(), r.getWidth(), r.getHeight());
    cairo_fill (cr);
//  }
}

void CairoGraphicsContext::fillRect (const Rectangle<float>&)
{
  
}

void CairoGraphicsContext::fillRectList (const RectangleList<float>&)
{
  
}

void CairoGraphicsContext::fillPath (const Path& path, const AffineTransform& transform)
{
  cairo_new_path (cr);
  drawPath (cr, path, transform);
  cairo_fill (cr);
}

void CairoGraphicsContext::drawImage (const Image& sourceImage, const AffineTransform& transform)
{
//  if (fillEntireClipAsTiles)
//  {
//    setFill(FillType(sourceImage, transform));
//    cairo_fill (cr);
//  }
//  else
//  {
    int width = sourceImage.getWidth();
    int height = sourceImage.getHeight();
    Image::BitmapData datas (sourceImage, 0, 0, width, height);
    unsigned char* buffer = (unsigned char*)datas.getPixelPointer (0, 0);
    cairo_surface_t *image = cairo_image_surface_create_for_data 
              (buffer, CAIRO_FORMAT_ARGB32, width, height, width*4);
    cairo_set_source_surface (cr, image, 0, 0);
    cairo_paint_with_alpha (cr, opacity);
//  }
}

void CairoGraphicsContext::drawLine (const Line<float>& line)
{
  cairo_set_line_width (cr, 1.0);
  cairo_move_to (cr, line.getStartX(), line.getStartY());
  cairo_line_to (cr, line.getEndX(), line.getEndY());
  cairo_stroke (cr);
}

void CairoGraphicsContext::setFont (const Font& newFont)
{
  font = newFont;
  cairo_font_slant_t slant = font.isItalic() ? CAIRO_FONT_SLANT_NORMAL : CAIRO_FONT_SLANT_ITALIC;
  cairo_font_weight_t weight = font.isBold() ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL;
  cairo_select_font_face (cr, (const char *)font.getTypefaceName().toUTF8(), slant, weight);
  cairo_set_font_size (cr, font.getHeight());
  cairo_glyph_extents (cr, glyphs, 10, extents);
}

const juce::Font&  CairoGraphicsContext::getFont()
{
  return font;
}

void CairoGraphicsContext::drawGlyph (int glyphNumber, const AffineTransform& transform)
{
  int num_glyphs = 1;
  cairo_set_font_matrix (cr, getMatrix(transform));
  cairo_show_glyphs (cr, glyphs + glyphNumber, num_glyphs);
}

