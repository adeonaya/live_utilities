#pragma once

template < class T >
class AMember
{
  private:
    char pad_0x0000[ 0x8 ]; // 0x0000
  public:
    T *Object; // 0x0008
};

template < class T >
class TWeakPtr
{
  private:
    char pad_0x0000[ 0x18 ]; // 0x0000
  public:
    T *Object; // 0x0018
};

template < class T >
class TPyHandle
{
  private:
    char pad_0x0000[ 0x18 ]; // 0x0000
  public:
    T *Object; // 0x0018
};

template < class T >
class TPyViewData
{
  private:
    char pad_0x0000[ 0x18 ]; // 0x0000
  public:
    T *Object; // 0x0018
};

class AClip
{
  public:
  private:
    char pad_0x0000[ 0x198 ]; // 0x0000
  public:
    TPyHandle< AClip > PyHandler; // 0x0198
  private:
    char pad_0x01A0[ 0x8 ]; // 0x01A0

}; // Size=0x01A8

class TDetailClipModel
{
  public:
  private:
    char pad_0x0000[ 0x78 ]; // 0x0000
  public:
    AClip *ForegroundClip; // 0x0078

}; // Size=0x0080

class TDetailClips
{
  public:
  private:
    char pad_0x0000[ 0x20 ]; // 0x0000
  public:
    TDetailClipModel *DetailClipModel; // 0x0020

}; // Size=0x0028

class ARemoteableBool
{
  public:
  private:
    char pad_0x0000[ 0x68 ]; // 0x0000
  public:
    bool m_bEnable; // 0x0068
}; // Size=0x0070

class ATrack
{
  public:
  private:
    char pad_0x0000[ 0x80 ]; // 0x0000
  public:
    ARemoteableBool *Selected; // 0x0080

}; // Size=0x0088

class LSong
{
  public:
  private:
    char pad_0x0000[ 0x308 ]; // 0x0000
  public:
    TDetailClips *DetailClips; // 0x0308
  private:
    char pad_0x0310[ 0x648 ]; // 0x0310
  public:
    ARemoteableBool *PreviewMIDI; // 0x0958

}; // Size=0x0960

class ATimeSelection
{
  public:
  private:
    char pad_0x0000[ 0x58 ]; // 0x0000
  public:
    double start_time; // 0x0058
    double end_time;   // 0x0060

}; // Size=0x0068

class ALiveDocument
{
  public:
  private:
    char pad_0x0000[ 0x1F0 ]; // 0x0000
  public:
    LSong *LeftSong; // 0x01F0
  private:
    char pad_0x01F8[ 0x98 ]; // 0x01F8
  public:
    ATimeSelection *TimeSelection; // 0x0290
  private:
    char pad_0x0298[ 0x80 ]; // 0x0298

}; // Size=0x0318

class ALiveApp
{
  public:
  private:
    char pad_0x0000[ 0x278 ]; // 0x0000
  public:
    ALiveDocument *Document; // 0x0278
};

class CLive
{
  public:
    void     *GetDocument( );
    bool      IsPlaying( );
    ALiveApp *GetApplication( );
    void      SetApplication( ALiveApp *LiveApp_addr );

  private:
    ALiveApp *App;
};

CLive *GetLive( );