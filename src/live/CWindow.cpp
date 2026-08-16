#include "pch/pch.h"

static CWindow g_CWindow { };

bool CWindow::IsKeyDown( uint8_t iKey )
{
    return m_bKeyTable[ iKey ];
}

bool CWindow::IsKeyReleased( uint8_t iKey )
{
    static bool bPressed[ 256 ]; // indexed by uint8_t (0-255); was 0xFF, one short
    if ( !m_bKeyTable[ iKey ] )
    {
        if ( bPressed[ iKey ] )
            return !( bPressed[ iKey ] = false );
    }
    else
        bPressed[ iKey ] = true;

    return false;
}

bool CWindow::IsKeyPressed( uint8_t iKey )
{
    static bool bPressed[ 256 ]; // indexed by uint8_t (0-255); was 0xFF, one short
    if ( m_bKeyTable[ iKey ] )
    {
        if ( !bPressed[ iKey ] )
            return bPressed[ iKey ] = true;
    }
    else
        bPressed[ iKey ] = false;

    return false;
}

LRESULT WINAPI CWindow::GUI_WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch ( uMsg )
    {
        // case WM_COMMAND :
        //{
        //     std::string name;
        //     HMENU       menu = nullptr;
        //     if ( GetWindow( )->FindMenuNameByID( GetMenu( hwnd ), wParam, name, menu ) )
        //     {
        //
        //     }
        //     break;
        // }
        case WM_LBUTTONDOWN :
        case WM_LBUTTONUP :
            GetWindow( )->m_bKeyTable[ VK_LBUTTON ] = ( uMsg == WM_LBUTTONDOWN );
            break;

        case WM_RBUTTONDOWN :
        case WM_RBUTTONUP :
            GetWindow( )->m_bKeyTable[ VK_RBUTTON ] = ( uMsg == WM_RBUTTONDOWN );
            break;

        case WM_MBUTTONDOWN :
        case WM_MBUTTONUP :
            GetWindow( )->m_bKeyTable[ VK_MBUTTON ] = ( uMsg == WM_MBUTTONDOWN );
            break;

        case WM_MOUSEMOVE :
            break;

        case WM_SYSKEYDOWN :
        case WM_SYSKEYUP :
        case WM_KEYDOWN :
        case WM_KEYUP :
            bool     bDown    = ( uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYDOWN );
            int      iKey     = ( int ) wParam;
            uint32_t ScanCode = LOBYTE( HIWORD( lParam ) );

            GetWindow( )->m_bKeyTable[ iKey ] = bDown;

            switch ( iKey )
            {
                case VK_SHIFT :
                    if ( ScanCode == MapVirtualKey( VK_LSHIFT, 0 ) )
                        GetWindow( )->m_bKeyTable[ VK_LSHIFT ] = bDown;
                    if ( ScanCode == MapVirtualKey( VK_RSHIFT, 0 ) )
                        GetWindow( )->m_bKeyTable[ VK_RSHIFT ] = bDown;
                    break;

                case VK_CONTROL :
                    if ( ScanCode == MapVirtualKey( VK_LCONTROL, 0 ) )
                        GetWindow( )->m_bKeyTable[ VK_LCONTROL ] = bDown;
                    if ( ScanCode == MapVirtualKey( VK_RCONTROL, 0 ) )
                        GetWindow( )->m_bKeyTable[ VK_RCONTROL ] = bDown;
                    break;

                case VK_MENU :
                    if ( ScanCode == MapVirtualKey( VK_LMENU, 0 ) )
                        GetWindow( )->m_bKeyTable[ VK_LMENU ] = bDown;
                    if ( ScanCode == MapVirtualKey( VK_RMENU, 0 ) )
                        GetWindow( )->m_bKeyTable[ VK_RMENU ] = bDown;
                    break;
            }
            break;
    }

    if ( GetWindow( )->IsKeyPressed( 0x35 ) )
    {
        // These calls end up in raw CPython C-API calls (CLive -> CPython::CallStatic /
        // PyObject_* ). This runs on the window-procedure thread, which does not hold the
        // GIL, so it must be acquired explicitly before touching the interpreter.
        void *gil = GetPython( )->PyGILState_Ensure( );

        printf( "GetApplication: 0x%p\n", GetLive( )->GetApplication( ) );
        printf( "GetDocument: 0x%p\n", GetLive( )->GetDocument( ) );
        printf( "IsPlaying: %s\n", GetLive( )->IsPlaying( ) ? "true" : "false" );

        GetPython( )->PyGILState_Release( gil );
    }

    return CallWindowProcA( GetWindow( )->m_WndProc, hwnd, uMsg, wParam, lParam );
}

void CWindow::BindMenuBool( HMENU hMenu, UINT itemId, bool *pFlag )
{
    MENUITEMINFO mii = { };
    mii.cbSize       = sizeof( MENUITEMINFO );
    mii.fMask        = MIIM_DATA | MIIM_STATE;
    mii.dwItemData   = reinterpret_cast< ULONG_PTR >( pFlag );
    mii.fState       = *pFlag ? MFS_CHECKED : MFS_UNCHECKED;
    SetMenuItemInfo( hMenu, itemId, FALSE, &mii );
}

bool CWindow::ToggleMenuBool( HMENU hMenu, UINT itemId )
{
    MENUITEMINFO mii = { };
    mii.cbSize       = sizeof( MENUITEMINFO );
    mii.fMask        = MIIM_DATA;

    if ( !GetMenuItemInfo( hMenu, itemId, FALSE, &mii ) )
        return false;

    bool *pFlag = reinterpret_cast< bool * >( mii.dwItemData );
    *pFlag      = !( *pFlag );

    mii.fMask  = MIIM_STATE;
    mii.fState = *pFlag ? MFS_CHECKED : MFS_UNCHECKED;
    SetMenuItemInfo( hMenu, itemId, FALSE, &mii );

    return *pFlag;
}

bool CWindow::IsMenuItemChecked( HMENU hMenu, UINT itemId )
{
    MENUITEMINFO mii = { };
    mii.cbSize       = sizeof( MENUITEMINFO );
    mii.fMask        = MIIM_STATE;
    GetMenuItemInfo( hMenu, itemId, FALSE, &mii );
    return ( mii.fState & MFS_CHECKED ) != 0;
}

bool CWindow::FindMenuItemID( HMENU hMenu, const char *name, UINT &outID, HMENU &outMenu )
{
    if ( !hMenu )
        return false;

    int count = GetMenuItemCount( hMenu );

    for ( int i = 0; i < count; i++ )
    {
        char text[ 256 ] = { };
        GetMenuStringA( hMenu, i, text, sizeof( text ), MF_BYPOSITION );

        if ( std::strcmp( text, name ) == 0 )
        {
            MENUITEMINFOA mii = { };
            mii.cbSize        = sizeof( mii );
            mii.fMask         = MIIM_ID;

            if ( GetMenuItemInfoA( hMenu, i, TRUE, &mii ) )
            {
                outID   = mii.wID;
                outMenu = hMenu;
                return true;
            }
        }

        HMENU sub = GetSubMenu( hMenu, i );
        if ( sub && FindMenuItemID( sub, name, outID, outMenu ) )
        {
            outMenu = sub;
            return true;
        }
    }

    return false;
}

bool CWindow::FindMenuNameByID( HMENU hMenu, UINT id, std::string &outName, HMENU &outMenu )
{
    if ( !hMenu )
        return false;

    int count = GetMenuItemCount( hMenu );

    for ( int i = 0; i < count; i++ )
    {
        MENUITEMINFOA mii = { };
        mii.cbSize        = sizeof( mii );
        mii.fMask         = MIIM_ID;

        if ( GetMenuItemInfoA( hMenu, i, TRUE, &mii ) )
        {
            if ( mii.wID == id )
            {
                char text[ 256 ] = { };
                GetMenuStringA( hMenu, i, text, sizeof( text ), MF_BYPOSITION );

                outName = text;
                outMenu = hMenu;
                return true;
            }
        }

        HMENU sub = GetSubMenu( hMenu, i );
        if ( sub && FindMenuNameByID( sub, id, outName, outMenu ) )
        {
            return true;
        }
    }

    return false;
}

uint64_t CWindow::GetDetailClip( )
{
    uint64_t result = 0;

    auto *App = GetLive( )->GetApplication( );

    if ( !App )
        return result;

    auto *Document = App->Document;

    if ( !Document )
        return result;

    auto *LeftSong = Document->LeftSong;

    if ( !LeftSong )
        return result;

    auto *DetailClips = LeftSong->DetailClips;

    if ( !DetailClips )
        return result;

    auto *DetailClipModel = DetailClips->DetailClipModel;

    if ( !DetailClipModel )
        return result;

    auto *DetailClip = DetailClipModel->ForegroundClip;

    if ( !DetailClip )
        return result;

    result = reinterpret_cast< uint64_t >( &DetailClip->PyHandler );

    if ( !result )
        return 0;

    return result;
}

bool CWindow::GetMidiPreview( )
{
    bool result = false;

    auto *App = GetLive( )->GetApplication( );

    if ( !App )
        return result;

    auto *Document = App->Document;

    if ( !Document )
        return result;

    auto *LeftSong = Document->LeftSong;

    if ( !LeftSong )
        return result;

    auto *PreviewMIDI = LeftSong->PreviewMIDI;

    if ( !PreviewMIDI )
        return result;

    result = PreviewMIDI->m_bEnable;

    return result;
}

bool CWindow::IsSelectedTrack( uint64_t TrackHandle )
{
    bool result = false;

    auto Track = reinterpret_cast< TPyHandle< ATrack > * >( TrackHandle );

    if ( !Track )
        return result;

    auto *SelectedTrack = Track->Object;

    if ( !SelectedTrack )
        return result;

    auto *SelectedButton = SelectedTrack->Selected;

    if ( !SelectedButton )
        return result;

    result = SelectedButton->m_bEnable;

    return result;
}

double CWindow::StartTime( )
{
    double result = 0;

    auto *App = GetLive( )->GetApplication( );

    if ( !App )
        return result;

    auto *Document = App->Document;

    if ( !Document )
        return result;

    auto *TimeSelection = Document->TimeSelection;

    if ( !TimeSelection )
        return result;

    result = TimeSelection->start_time;

    return result;
}

double CWindow::EndTime( )
{
    double result = 0;

    auto *App = GetLive( )->GetApplication( );

    if ( !App )
        return result;

    auto *Document = App->Document;

    if ( !Document )
        return result;

    auto *TimeSelection = Document->TimeSelection;

    if ( !TimeSelection )
        return result;

    result = TimeSelection->end_time;

    return result;
}

void CWindow::OnInit( )
{
    while ( !m_hWindow )
    {
        m_hWindow = FindWindowW( L"Ableton Live Window Class", NULL );
        Sleep( 100 );
    }

    m_WndProc = ( WNDPROC ) SetWindowLongPtrA( m_hWindow, GWLP_WNDPROC, ( LONG_PTR ) GUI_WndProc );
    m_bInit   = true;

    {
        GetPython( )->Py_Initialize( );
        const char *moduleName = "cpp";
        void       *state      = GetPython( )->PyGILState_Ensure( );
        void       *cpp        = GetPython( )->PyModule_New( moduleName );

        if ( cpp )
        {
            GetPython( )->AddPythonFunction( cpp, "is_midi_preview", GetPython( )->is_midi_preview, "Check if MIDI Preview is enabled" );
            GetPython( )->AddPythonFunction( cpp, "send_midi_preview", GetPython( )->send_midi_preview, "Send MIDI Preview" );
            GetPython( )->AddPythonFunction( cpp, "is_key_released", GetPython( )->is_key_released, "Verify if this key is released" );
            GetPython( )->AddPythonFunction( cpp, "is_key_pressed", GetPython( )->is_key_pressed, "Verify if this key is pressed" );
            GetPython( )->AddPythonFunction( cpp, "is_key_down", GetPython( )->is_key_down, "Verify if this key is down" );
            GetPython( )->AddPythonFunction( cpp, "get_detail_clip", GetPython( )->get_detail_clip, "getting detail clip" );
            GetPython( )->AddPythonFunction( cpp, "is_selected_track", GetPython( )->is_selected_track, "if the track is selected" );
            GetPython( )->AddPythonFunction( cpp, "time_selection_start_time", GetPython( )->time_selection_start_time,
                                             "time_selection_start_time" );
            GetPython( )->AddPythonFunction( cpp, "time_selection_end_time", GetPython( )->time_selection_end_time,
                                             "time_selection_end_time" );

            void *modules = GetPython( )->PyImport_GetModuleDict( );

            GetPython( )->PyDict_SetItemString( modules, moduleName, cpp );
        }

        GetPython( )->PyGILState_Release( state );
    }

    uint64_t ModuleBase = reinterpret_cast< uint64_t >( GetModuleHandleA( NULL ) );

    const IMAGE_DOS_HEADER *DOSHeader = reinterpret_cast< IMAGE_DOS_HEADER * >( ModuleBase );

    const IMAGE_NT_HEADERS *NtHeaders = reinterpret_cast< IMAGE_NT_HEADERS * >( ModuleBase + DOSHeader->e_lfanew );

    const DWORD SizeOfImage = NtHeaders->OptionalHeader.SizeOfImage;

    ALiveApp *App = nullptr;

    while ( !App )
    {
        App = *( ALiveApp ** ) PatternScan< uint64_t >( "48 89 1D ? ? ? ? 48 83 C4 ? 41 5F", ModuleBase, SizeOfImage, true );
        Sleep( 100 );
    }

    GetLive( )->SetApplication( App );

    //AllocConsole( );
    //FILE *Dummy;
    //freopen_s( &Dummy, "CONOUT$", "w", stdout );
    //freopen_s( &Dummy, "CONIN$", "r", stdin );
}

CWindow *GetWindow( )
{
    return &g_CWindow;
}