/*********************************************************************************

  *FileName: Token.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/15

**********************************************************************************/

#ifndef _OBJ_LOADER_H_
#define _OBJ_LOADER_H_

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"Token.h"

struct stObjModel
{
    float* vertices;
    float* normals;
    float* texCoords;
    int numFaces;
};

stObjModel* LoadOBJModel(char* fileName);
void FreeOBJModel(stObjModel* model);

#endif