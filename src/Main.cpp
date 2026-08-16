#include "pch/pch.h"

DWORD WINAPI MainThread( LPVOID lpThreadParameter )
{
    while ( !GetModuleHandleA( LIVE_MODULE ) )
        Sleep( 100 );
     
    GetWindow( )->OnInit( );

    return 0;
}

BOOL WINAPI DllMain( HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved )
{
    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH :
            DisableThreadLibraryCalls( hInstance );
            CreateThread( 0, 0, MainThread, lpReserved, 0, 0 );
            break;
        case DLL_PROCESS_DETACH :
            break;
    }

    return TRUE;
}