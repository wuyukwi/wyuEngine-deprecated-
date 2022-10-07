- 本プロジェクトは[CMake](https://cmake.org/)を使って作成しています。

# wyuEngine

## 簡単な紹介
- DirectX 9.0cを使った自作エンジンです。2Dゲーム対応の機能実装しています。3D機能まだ不完全です、これから実装する予定です。

**ビルド環境**
>c++17ビルドできるコンパイラ、Cmakeバージョン3.19以上

**実行環境**

>Win10,Win11

**使用方法**
>build.bat実行すればvsプロジェクト生成します(vs2017以降)

## フォルダ説明

**engine/3rdparty** (外部ライブラリー保存用フォルダ)

今使っている外部ラリー
1. DirectX 9.0c
2. xaudio2
3. DirectInput／XInput
4. [Dear ImGui](https://github.com/ocornut/imgui)
5. [json11](https://github.com/dropbox/json11)

**engine/include** (ヘッダファイル保存用フォルダ)

**engine/source** (ソースファイル保存用フォルダ)

##ソースコード説明
gameEngine.h
+ gameEngineクラス
>ゲームエンジン主なクラス、ゲームエンジン全般コントロールできる関数持っています。詳細はソースコード見てください。


``` c++

```

