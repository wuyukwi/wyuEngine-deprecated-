/*
   Demo Name:  Game Project 11
      Author:  Allen Sherrod
     Chapter:  Chapter 12
*/

#include"objLoader.h"

stObjModel* LoadOBJModel(char* fileName)
{
    FILE* file;
    char* data = NULL;
    CToken lexer, tempLex;
    char tempLine[512];
    char token[512];

    // ファイルを開きます。
    file = fopen(fileName, "r");
    if (!file) return NULL;

    // ファイルの長さを取得します。
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);

    // ファイルからすべてのデータを読み込みます
    data = new char[(length + 1) * sizeof(char)];
    if (!data) return NULL;
    fread(data, length, 1, file);
    data[length] = '\0';

    // 完了したらファイルを閉じます。
    fclose(file);

    // ファイルをTokenStreamに設定します。
    lexer.SetTokenStream(data);

    // もう必要ありません。
    delete[] data; data = NULL;

    bool validFile = false;

    // ファイル内のどこかでWavefrontという単語を探して、この.objが互換性があるかどうかを判断します。
    // これは、モデラーがわずかに異なる形式にエクスポートするためです。
    while (lexer.GetNextToken(token))
    {
        if (strcmp(token, "Wavefront") == 0)
        {
            validFile = true;
            break;
        }
    }

    if (!validFile) return NULL;

    // リセットします。
    lexer.Reset();

    // ファイルで宣言されたそれぞれの総数を取得するために使用されます。
    // facesはインデックスを使用するため、これらの数は異なる可能性があります。
    int totalVertices = 0, totalNormals = 0,
        totalTexC = 0, totalFaces = 0;

    // 最初の（または次の）行を取得します。
    while (lexer.MoveToNextLine(tempLine))
    {
        // //行をTokenStreamに設定します。
        tempLex.SetTokenStream(tempLine);

        // 改行を読み取ります。
        lexer.GetNextToken(NULL);

        // temp lexに何かが設定されている場合は、続行します。
        if (!tempLex.GetNextToken(token)) continue;

        //行の最初のトークンがv、vn、vt、またはfの場合 そのカウンターを増加します。
        if (strcmp(token, "v") == 0) totalVertices++;
        else if (strcmp(token, "vn") == 0) totalNormals++;
        else if (strcmp(token, "vt") == 0) totalTexC++;
        else if (strcmp(token, "f") == 0) totalFaces++;

        token[0] = '\0';
    }

    // データを保持するための一時スペースを割り当てます。
    // それぞれ3つの値（vインデックス/vtインデックス/vnインデックス）を持つ3つの頂点があるため、facesは9です。
    float* verts = new float[totalVertices * 3];
    float* norms = new float[totalNormals * 3];
    float* texC = new float[totalTexC * 2];
    int* faces = new int[totalFaces * 9];
    int vIndex = 0, nIndex = 0, tIndex = 0, fIndex = 0, index = 0;

    // ファイルの先頭に移動します。
    lexer.Reset();

    // すべてをやり直しますが、今回はデータを取得します。
    while (lexer.MoveToNextLine(tempLine))
    {
        // temp lexに設定し、過去の改行を読み取り、tokenを取得します。
        tempLex.SetTokenStream(tempLine);
        lexer.GetNextToken(NULL);
        if (!tempLex.GetNextToken(token)) continue;

        // vの場合、頂点x、y、zを取得します。
        if (strcmp(token, "v") == 0)
        {
            // xを取得して保存します。
            tempLex.GetNextToken(token);
            verts[vIndex] = (float)atof(token);
            vIndex++;

            // yを取得して保存します。
            tempLex.GetNextToken(token);
            verts[vIndex] = (float)atof(token);
            vIndex++;

            // zを取得して保存します。
            tempLex.GetNextToken(token);
            verts[vIndex] = (float)atof(token);
            vIndex++;
        }
        // それ以外の場合、vnの場合、頂点法線のx、y、zを取得します。
        else if (strcmp(token, "vn") == 0)
        {
            // xを取得して保存します。
            tempLex.GetNextToken(token);
            norms[nIndex] = (float)atof(token);
            nIndex++;

            // yを取得して保存します。
            tempLex.GetNextToken(token);
            norms[nIndex] = (float)atof(token);
            nIndex++;

            // zを取得して保存します。
            tempLex.GetNextToken(token);
            norms[nIndex] = (float)atof(token);
            nIndex++;
        }
        //それ以外の場合vtの場合、テクスチャ u、vを取得します。
        else if (strcmp(token, "vt") == 0)
        {
            // uを取得して保存します。
            tempLex.GetNextToken(token);
            texC[tIndex] = (float)atof(token);
            tIndex++;

            // vを取得して保存します。
            tempLex.GetNextToken(token);
            texC[tIndex] = (float)atof(token);
            tIndex++;
        }
        //それ以外の場合、fの場合、各頂点3のインデックスセットを取得します。
        else if (strcmp(token, "f") == 0)
        {
            // Load for each vertex (3 in a triangle).
            for (int i = 0; i < 3; i++)
            {
                //頂点ごとにロードします（三角形の3つの頂点）。
                tempLex.GetNextToken(token);
                int len = strlen(token);

                //セット（1/1/1）の間にスペースがないため、トークンを単純に読み取ることはできないため
                // ループして/記号の前の各値を取り出す必要があります。
                for (int s = 0; s < len + 1; s++)
                {
                    char buff[64];

                    // /でない場合、または最後にない場合。
                    if (token[s] != '/' && s < len)
                    {
                        buff[index] = token[s];
                        index++;
                    }
                    else
                    {
                        //それ以外の場合は、文字列を終了し、変換して保存します。
                        buff[index] = '\0';
                        faces[fIndex] = (int)atoi(buff);
                        fIndex++;
                        index = 0;
                    }
                }
            }
        }

        token[0] = '\0';
    }

    // もう必要ありません。
    lexer.Shutdown();

    // 割り当ててモデルオブジェクトを作成します。
    stObjModel* model = new stObjModel;
    if (!model) return NULL;
    memset(model, 0, sizeof(stObjModel));

    // facesの数を保存します。
    model->numFaces = totalFaces;

    // 一時カウンターをリセットします。
    vIndex = 0, nIndex = 0, tIndex = 0, fIndex = 0, index = 0;

    // モデルの各部分にデータを割り当てます。
    model->vertices = new float[totalFaces * 3 * 3];
    if (totalNormals) model->normals = new float[totalFaces * 3 * 3];
    if (totalTexC) model->texCoords = new float[totalFaces * 3 * 2];

    // ループしてモデルに入力します。
    for (int f = 0; f < totalFaces * 9; f += 3)
    {
        // 頂点を取得します。 インデックスは1から最大ではなく0から最大-1の範囲である必要があるため、1を減算します。
        // 頂点には3つのコンポーネント（x、y、z）があるため、3を掛けます。
        model->vertices[vIndex + 0] = verts[(faces[f + 0] - 1) * 3 + 0];
        model->vertices[vIndex + 1] = verts[(faces[f + 0] - 1) * 3 + 1];
        model->vertices[vIndex + 2] = verts[(faces[f + 0] - 1) * 3 + 2];
        vIndex += 3;

        //テクスチャ座標データでも同じことを行います。 2番目のデータはtexcoordデータなので、1をf（v / vt / vn）に適用します。
        if (model->texCoords)
        {
            model->texCoords[tIndex + 0] = texC[(faces[f + 1] - 1) * 2 + 0];
            model->texCoords[tIndex + 1] = texC[(faces[f + 1] - 1) * 2 + 1];
            tIndex += 2;
        }

        // 法線のデータでも同じことを行います。
        if (model->normals)
        {
            model->normals[nIndex + 0] = norms[(faces[f + 2] - 1) * 3 + 0];
            model->normals[nIndex + 1] = norms[(faces[f + 2] - 1) * 3 + 1];
            model->normals[nIndex + 2] = norms[(faces[f + 2] - 1) * 3 + 2];
            nIndex += 3;
        }
    }

    // 一時データを削除します。
    delete[] verts;
    delete[] norms;
    delete[] texC;
    delete[] faces;

    return model;
}

void FreeOBJModel(stObjModel* model)
{
    if (!model) return;

    // すべてのリソースを解放します。
    if (model->vertices) delete[] model->vertices;
    model->vertices = NULL;
    if (model->normals) delete[] model->normals;
    model->normals = NULL;
    if (model->texCoords) delete[] model->texCoords;
    model->texCoords = NULL;

    delete model;
    model = NULL;
}