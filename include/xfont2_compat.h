#ifdef XFONT2
#include <X11/fonts/fontstruct.h>
#include <X11/fonts/libxfont2.h>
#endif

/* for Xext/xf86bigfont.c */
#ifdef XFONT2
#define AllocateFontPrivateIndex xfont2_allocate_font_private_index
#endif

/* for dix/dispatch.c */
#ifdef XFONT2
#define QueryTextExtents xfont2_query_text_extents
#endif

/* for dix/main.c */
#ifdef XFONT2
#define InitGlyphCaching xfont2_init_glyph_caching
#endif

/* for mi/miglbt.c and miext/damage/damage.c */
#ifdef XFONT2
#define QueryGlyphExtents xfont2_query_glyph_extents
#endif

/* for os/utils.c */
#ifdef XFONT2
#define ParseGlyphCachingMode xfont2_parse_glyph_caching_mode
#endif

/* for dix/dixfonts.c */
#ifdef XFONT2
#define CacheFontPattern xfont2_cache_font_pattern
#define FindCachedFontPattern xfont2_find_cached_font_pattern
#define RemoveCachedFontPattern xfont2_remove_cached_font_pattern
#define AddFontNamesName xfont2_add_font_names_name
#define FreeFontNames xfont2_free_font_names
#define MakeFontNamesRecord xfont2_make_font_names_record
#define EmptyFontPatternCache xfont2_empty_font_pattern_cache
#define FreeFontPatternCache xfont2_free_font_pattern_cache
#endif
