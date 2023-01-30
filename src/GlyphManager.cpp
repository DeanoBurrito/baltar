#include <GlyphManager.h>
#include <Debug.h>
#include <3rdparty/Glad.h>
#include <vector>
#include <algorithm>

namespace Baltar
{
    void GlyphManager::Init()
    {
        ASSERT(FT_Init_FreeType(&ftLib) == FT_Err_Ok, "FreeType init failed");
    }

    void GlyphManager::Deinit()
    {
        if (ftFace != nullptr)
            FT_Done_Face(ftFace);
        //TODO: delete any attached textures
        
        FT_Done_FreeType(ftLib);
        ftLib = nullptr;
    }
    
    void GlyphManager::SetActiveFace(const std::string_view& name, size_t size)
    {
        ASSERT(ftLib != nullptr, StrUninitialized);
        if (name == currentName && size == currentSize)
            return;
        
        FT_Face newFace;
        VERIFY(!FT_New_Face(ftLib, name.data(), 0, &newFace),, "FT_New_Face()");
        VERIFY(!FT_Set_Pixel_Sizes(newFace, 0, size),, "FT_Set_Pixel_Sizes()");
        
        Log("Loaded new font: size=%lu, file=%s", LogLevel::Info, size, name.data());
        currentName = name;
        currentSize = size;
        if (ftFace != nullptr)
            FT_Done_Face(ftFace);
        ftFace = newFace;

        ClearCache();
        maxVerticalBearing = GetGlyph('|').bearing.y;

        //TODO: pre-cache common glyphs
    }

    Glyph GlyphManager::GetGlyph(uint32_t codepoint)
    {
        ASSERT(ftLib != nullptr, StrUninitialized);
        if (auto found = glyphCache.find(codepoint); found != glyphCache.end())
            return found->second;
        
        //cache miss, render the char bitmap and create oGL texture
        ASSERT(!FT_Load_Char(ftFace, codepoint, FT_LOAD_RENDER), "FT_Load_Char()");
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLuint textureId;
        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);

        auto slot = ftFace->glyph;
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, slot->bitmap.width, slot->bitmap.rows, 0, 
            GL_RED, GL_UNSIGNED_BYTE, slot->bitmap.buffer);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        return glyphCache[codepoint] = Glyph 
        { 
            textureId, 
            { slot->bitmap.width, slot->bitmap.rows }, 
            { slot->bitmap_left, slot->bitmap_top }, 
            static_cast<size_t>(slot->advance.x)
        };
    }

    size_t GlyphManager::MaxVerticalBearing() const
    {
        return maxVerticalBearing;
    }

    void GlyphManager::ClearCache()
    {
        Log("Clearing %lu cached glyphs.", LogLevel::Info, glyphCache.size());

        std::vector<GLuint> texIds;
        texIds.reserve(glyphCache.size());
        for (const auto it : glyphCache)
            texIds.push_back(it.second.texId);
        
        glyphCache.clear();
        glDeleteTextures(texIds.size(), texIds.data());
    }
}
