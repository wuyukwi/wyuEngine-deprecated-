/*********************************************************************************

  *FileName: material.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/

#ifndef _MATERIAL_H_
#define _MATERIAL_H_

struct stMaterial
{
    stMaterial()
    {
        emissiveR = emissiveG = emissiveB = emissiveA = 0.0f;
        ambientR = ambientG = ambientB = ambientA = 1.0f;
        diffuseR = diffuseG = diffuseB = diffuseA = 1.0f;
        specularR = specularG = specularB = specularA = 0.0f;
        power = 0.0f;
    }

    float emissiveR, emissiveG, emissiveB, emissiveA;
    float ambientR, ambientG, ambientB, ambientA;
    float diffuseR, diffuseG, diffuseB, diffuseA;
    float specularR, specularG, specularB, specularA;
    float power;
};

#endif // _MATERIAL_H_