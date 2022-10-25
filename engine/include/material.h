/*********************************************************************************

  *FileName: material.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/
#pragma once
#include "math/Vector.h"
#include "defines.h"

struct Material
{
    Material()
    {
        Ambient = 1.0f;
        Diffuse = 1.0f;
        Specular = 0.0f;
        Emissive = 0.0f;
        power = 0.0f;
    }

    Material(Color ambient, Color diffuse, Color specular, Color emissive, float power)
    {
        this->Ambient = ambient;
        this->Diffuse = diffuse;
        this->Specular = specular;
        this->Emissive = emissive;
        this->power = power;
    }


    Color Diffuse; // マテリアルの拡散色 
    Color Ambient; // マテリアルのアンビエント カラーを指定する値
    Color Specular;// マテリアルの反射色を指定する値
    Color Emissive; // マテリアルの放射色を指定する値
    float power; // 反射ハイライトのシャープネスを指定する浮動小数点値。 値が大きいほど、ハイライトがシャープになります。
};


const Material WHITE_MTRL = Material(WHITE, WHITE, WHITE, BLACK, 2.0f);
const Material RED_MTRL = Material(RED, RED, RED, BLACK, 2.0f);
const Material GREEN_MTRL = Material(GREEN, GREEN, GREEN, BLACK, 2.0f);
const Material BLUE_MTRL = Material(BLUE, BLUE, BLUE, BLACK, 2.0f);
const Material YELLOW_MTRL = Material(YELLOW, YELLOW, YELLOW, BLACK, 2.0f);
