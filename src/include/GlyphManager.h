#pragma once

#include <string>
#include <unordered_map>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include <glm/vec2.hpp>

namespace Baltar
{
    struct Glyph
    {
        glm::uint texId;
        glm::ivec2 size;
        glm::ivec2 bearing;
        size_t advance;
    };
    
    class GlyphManager
    {
    private:
        FT_Library ftLib = nullptr;
        FT_Face ftFace = nullptr;

        std::unordered_map<uint32_t, Glyph> glyphCache;

        std::string currentName;
        size_t currentSize;
        size_t maxVerticalBearing;

    public:
        GlyphManager() = default;

        void Init();
        void Deinit();

        GlyphManager(const GlyphManager&) = delete;
        GlyphManager& operator=(const GlyphManager&) = delete;
        GlyphManager(GlyphManager&&) = delete;
        GlyphManager& operator=(GlyphManager&&) = delete;

        void SetActiveFace(const std::string_view& name, size_t size);
        Glyph GetGlyph(uint32_t codepoint);
        size_t MaxVerticalBearing() const;
        void ClearCache();
    };
}
