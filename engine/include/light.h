/*********************************************************************************

  *FileName: light.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/
#pragma once
#include "math/Vector.h"

enum class LightType
{
    POINT,
    DIRECTIONAL,
    SPOT
};
struct stLight
{
    void InitDirectionalLight(const Vector3f& direction, Vector4f& color)
    {
        Type = LightType::DIRECTIONAL;
        Ambient = color * 0.4f;
        Diffuse = color;
        Specular = color * 0.6f;
        Direction = direction;
    }

    void InitPointLight(const Vector3f& pos, Vector4f& color)
    {
        Type = LightType::POINT;
        Ambient = color * 0.4f;
        Diffuse = color;
        Specular = color * 0.6f;
        Position = pos;
        Range = 1000.0f;
        Falloff = 1.0f;
        Attenuation0 = 1.0f;
        Attenuation1 = 0.0f;
        Attenuation2 = 0.0f;
    }

    void InitSpotLight(const Vector3f& pos, const Vector3f& direction, Vector4f& color)
    {
        Type = LightType::SPOT;
        Ambient = color * 0.4f;
        Diffuse = color;
        Specular = color * 0.6f;
        Position = pos;
        Direction = *direction;
        Range = 1000.0f;
        Falloff = 1.0f;
        Attenuation0 = 1.0f;
        Attenuation1 = 0.0f;
        Attenuation2 = 0.0f;
        Theta = 0.5f;
        Phi = 0.7f;
    }

    LightType Type;//光源の種類
    Vector4f Diffuse{ 0 };//光によって放射される拡散色
    Vector4f Specular{ 0 };//ライトによって出力される反射色
    Vector4f Ambient{ 0 };//ライトによって放射されるアンビエント カラー
    Vector3f Position{ 0 };//ワールド空間内のライトの位置
    Vector3f Direction{ 0 };//ワールド空間でライトが指している方向
    float         Range = 0;//ライトが影響を与えない距離
    //スポットライトの内部円錐 (Theta で指定された角度) と外側コーンの外縁 (Phi で指定された角度) の間の照明の減少
    float         Falloff = 0;
    float         Attenuation0 = 0;//距離に対する光の強度の変化を指定する値0
    float         Attenuation1 = 0;//距離に対する光の強度の変化を指定する値1
    float         Attenuation2 = 0;//距離に対する光の強度の変化を指定する値2
    float         Theta = 0;//スポットライトの内部円錐の角度 (ラジアン単位)
    float         Phi = 0;//スポットライトの外側の円錐の外縁を定義する角度 (ラジアン単位)
};
