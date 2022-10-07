#pragma once
#include <cstdint>
#include <string>

struct GfxConfiguration {
    /// Inline all-elements constructor.
    /// \param[in] r the red color depth in bits
    /// \param[in] g the green color depth in bits
    /// \param[in] b the blue color depth in bits
    /// \param[in] a the alpha color depth in bits
    /// \param[in] d the depth buffer depth in bits
    /// \param[in] s the stencil buffer depth in bits
    /// \param[in] msaa the msaa sample count
    /// \param[in] width the screen width in pixel
    /// \param[in] height the screen height in pixel
    /// \param app_name
    /// \param app_class_name
    GfxConfiguration(uint32_t r = 8, uint32_t g = 8,
        uint32_t b = 8, uint32_t a = 8,
        uint32_t d = 24, uint32_t s = 0, uint32_t msaa = 0,
        int32_t width = 1280, int32_t height = 720, bool fullscreen = false,
        std::string app_name = "GameEngine",
        const char* app_class_name = "WindowsClass") :
        redBits(r), greenBits(g), blueBits(b), alphaBits(a),
        depthBits(d), stencilBits(s), msaaSamples(msaa),
        screenWidth(width), screenHeight(height), fullscreen(fullscreen),
        appName(std::move(app_name)), winClassName(app_class_name) {}

    uint32_t redBits; ///< red color channel depth in bits
    uint32_t greenBits; ///< green color channel depth in bits
    uint32_t blueBits; ///< blue color channel depth in bits
    uint32_t alphaBits; ///< alpha color channel depth in bits
    uint32_t depthBits; ///< depth buffer depth in bits
    uint32_t stencilBits; ///< stencil buffer depth in bits
    uint32_t msaaSamples; ///< MSAA samples
    int32_t screenWidth;
    int32_t screenHeight;
    bool fullscreen;
    std::string appName;
    const char* winClassName;
};
