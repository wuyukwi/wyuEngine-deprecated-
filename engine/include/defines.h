/*********************************************************************************

  *FileName: defines.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/

#ifndef _DEFINES_H_
#define _DEFINES_H_

#include <Windows.h>
#include <cstdio>
#include <cmath>

#include "math/Vector.h"

#define SAFE_DELETE( p )       { if( p ) { delete ( p );     ( p ) = nullptr; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );   ( p ) = nullptr; } }
#define SAFE_RELEASE( p )      { if( p ) { ( p )->Release(); ( p ) = nullptr; } }

#define KEYS_SIZE (256)

// Boolean values.
#define UGP_INFINITE -1
#define UGP_INVALID -1
#define UGP_OK       1
#define UGP_FAIL     0

// Vertex formats.
#define FVF_RHW     2
#define FVF_2D      3
#define FVF_3D      4

// Types of controls we support.
#define UGP_GUI_STATICTEXT 1
#define UGP_GUI_BUTTON     2
#define UGP_GUI_BACKDROP   3

// Mouse button states.
#define UGP_BUTTON_UP      1
#define UGP_BUTTON_OVER    2
#define UGP_BUTTON_DOWN    3

#define MOUSE_LEFT_BUTTON  0
#define MOUSE_RIGHT_BUTTON 1

// ライトタイプ
#define LIGHT_POINT        1
#define LIGHT_DIRECTIONAL  2
#define LIGHT_SPOT         3

// Window handle (need new way if porting to Mac and OpenGL).
#define WinHWND HWND

// Inline関数
//template <typename T>
//inline void SAFE_DELETE(T& p)
//{
//    if (p)
//    {
//        delete(p);
//
//        p = nullptr;
//    }
//}
//template <typename T>
//inline void SAFE_DELETE_ARRAY(T& p)
//{
//    if (p)
//    {
//        delete[](p);
//        p = nullptr;
//    }
//}
//template <typename T>
//inline void SAFE_RELEASE(T& p)
//{
//    if (p)
//    {
//        p->Release();
//        p = nullptr;
//    }
//}

//template <typename T>
//inline unsigned long COLOR_ARGB(float a, float r, float g, T& b)
//{
//    return static_cast<unsigned long>(((a) & 0xff) << 24) | (((r) & 0xff) << 16) | (((b) & 0xff) << 8) | ((b) & 0xff);
//}
#define COLOR_ARGB(a,r,g,b) \
    ((unsigned long)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

inline unsigned long COLOR_RGBA(float r, float g, float b, float a)
{
    const unsigned long dwR = r >= 1.0f ? 0xff : r <= 0.0f ? 0x00 : static_cast<unsigned long>(r * 255.0f + 0.5f);
    const unsigned long dwG = g >= 1.0f ? 0xff : g <= 0.0f ? 0x00 : static_cast<unsigned long>(g * 255.0f + 0.5f);
    const unsigned long dwB = b >= 1.0f ? 0xff : b <= 0.0f ? 0x00 : static_cast<unsigned long>(b * 255.0f + 0.5f);
    const unsigned long dwA = a >= 1.0f ? 0xff : a <= 0.0f ? 0x00 : static_cast<unsigned long>(a * 255.0f + 0.5f);


    return (dwA << 24) | (dwR << 16) | (dwG << 8) | dwB;
}

constexpr float ColorScalar = 1.0f / 255.0f;
inline Color ColorIntTransFloat(int32_t r, int32_t g, int32_t b, int32_t a)
{
    return Color(float(r * ColorScalar), float(g * ColorScalar), float(b * ColorScalar), float(a * ColorScalar));
}

const Color      WHITE(ColorIntTransFloat(255, 255, 255, 0));
const Color      BLACK(ColorIntTransFloat(0, 0, 0, 0));
const Color        RED(ColorIntTransFloat(255, 0, 0, 0));
const Color      GREEN(ColorIntTransFloat(0, 255, 0, 0));
const Color       BLUE(ColorIntTransFloat(0, 0, 255, 0));
const Color     YELLOW(ColorIntTransFloat(255, 255, 0, 0));
const Color       CYAN(ColorIntTransFloat(0, 255, 255, 0));
const Color    MAGENTA(ColorIntTransFloat(255, 0, 255, 0));

// Typedefs and enumerations.
typedef long VertexType;
// プリミティブタイプ
enum class PrimType
{
    NULL_TYPE,
    POINT_LIST,
    TRIANGLE_LIST,
    TRIANGLE_STRIP,
    TRIANGLE_FAN,
    LINE_LIST,
    LINE_STRIP
};

enum class RenderState
{
    CULL_NONE,
    CULL_CW,
    CULL_CCW,
    DEPTH_NONE,
    DEPTH_READONLY,
    DEPTH_READWRITE,
    SHADE_POINTS,
    SHADE_SOLIDTRI,
    SHADE_WIRETRI,
    SHADE_WIREPOLY,
    TRANSPARENCY_NONE,
    TRANSPARENCY_ENABLE
};

enum class TransperentState
{
    ZERO = 1,
    ONE,
    SRCCOLOR,
    INVSRCCOLOR,
    SRCALPHA,
    INVSRCALPHA,
    DSTALPHA,
    INVDSTALPHA,
    DSTCOLOR,
    INVDSTCOLOR,
    SRCALPHASAT,
    BOTHSRCALPHA,
    INVBOTHSRCALPHA,
    BLENDFACTOR,
    INVBLENDFACTOR
};

enum TextureState
{
    MIN_FILTER,
    MAG_FILTER,
    MIP_FILTER
};

enum FilterType
{
    POINT_TYPE,
    LINEAR_TYPE,
    ANISOTROPIC_TYPE
};

enum MS_TYPE
{
    MS_NONE,
    MS_SAMPLES_2,
    MS_SAMPLES_4,
    MS_SAMPLES_8,
    MS_SAMPLES_16
};

enum FOG_TYPE
{
    VERTEX_FOR,
    PIXEL_FOG
};
#endif