#pragma once

class IWindow
{
  public:
    virtual ~IWindow( )                        = default;
    virtual bool IsKeyDown( uint8_t iKey )     = 0;
    virtual bool IsKeyReleased( uint8_t iKey ) = 0;
    virtual bool IsKeyPressed( uint8_t iKey )  = 0;
};

class CWindow : public IWindow
{
  public:
    void                  OnInit( );
    void                  OnDestroy( );
    static LRESULT WINAPI GUI_WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    bool                  IsKeyDown( uint8_t iKey );
    bool                  IsKeyReleased( uint8_t iKey );
    bool                  IsKeyPressed( uint8_t iKey );

    HWND GetHWND( ) const
    {
        return m_hWindow;
    }

    void     BindMenuBool( HMENU hMenu, UINT itemId, bool *pFlag );
    bool     ToggleMenuBool( HMENU hMenu, UINT itemId );
    bool     IsMenuItemChecked( HMENU hMenu, UINT itemId );
    bool     FindMenuItemID( HMENU hMenu, const char *name, UINT &outID, HMENU &outMenu );
    bool     FindMenuNameByID( HMENU hMenu, UINT id, std::string &outName, HMENU &outMenu );
    uint64_t GetDetailClip( );
    bool     GetMidiPreview( );
    bool     IsSelectedTrack( uint64_t PyHandleTrack );
    double   StartTime( );
    double   EndTime( );

  private:
    WNDPROC m_WndProc = nullptr;
    HWND    m_hWindow = nullptr;
    bool    m_bKeyTable[ 256 ];
    bool    m_bInit = false;
};

CWindow *GetWindow( );