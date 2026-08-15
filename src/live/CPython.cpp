#include "pch/pch.h"

static CPython g_CPython { };

template < typename FN >
FN CPython::Resolve( const char *name )
{
    static HMODULE module = GetModuleHandleA( LIVE_MODULE );

    return reinterpret_cast< FN >( GetProcAddress( module, name ) );
}

void CPython::Py_Initialize( )
{
    using FN = void ( * )( );

    Resolve< FN >( "Py_Initialize" )( );
}

void *CPython::PyImport_AddModule( const char *szName )
{
    using FN = void *( __cdecl * ) ( const char * );

    return Resolve< FN >( "PyImport_AddModule" )( szName );
}

void *CPython::PyImport_ImportModule( const char *szName )
{
    using FN = void *( __cdecl * ) ( const char * );

    return Resolve< FN >( "PyImport_ImportModule" )( szName );
}

int CPython::PyCallable_Check( void *obj )
{
    using FN = int( __cdecl * )( void * );

    return Resolve< FN >( "PyCallable_Check" )( obj );
}

void *CPython::PyObject_GetAttrString( void *obj, const char *attr_name )
{
    using FN = void *( __cdecl * ) ( void *, const char * );

    return Resolve< FN >( "PyObject_GetAttrString" )( obj, attr_name );
}

void *CPython::PyUnicode_FromString( const char *str )
{
    using FN = void *( __cdecl * ) ( const char * );

    return Resolve< FN >( "PyUnicode_FromString" )( str );
}

void *CPython::PyObject_CallObject( void *callable, void *args )
{
    using FN = void *( __cdecl * ) ( void *, void * );

    return Resolve< FN >( "PyObject_CallObject" )( callable, args );
}

void *CPython::PyTuple_New( intptr_t size )
{
    using FN = void *( __cdecl * ) ( intptr_t );

    return Resolve< FN >( "PyTuple_New" )( size );
}

int CPython::PyTuple_SetItem( void *tuple, intptr_t index, void *item )
{
    using FN = int( __cdecl * )( void *, intptr_t, void * );

    return Resolve< FN >( "PyTuple_SetItem" )( tuple, index, item );
}

void *CPython::PyModule_GetDict( void *pModule )
{
    using FN = void *( __cdecl * ) ( void * );

    return Resolve< FN >( "PyModule_GetDict" )( pModule );
}

void *CPython::PyRun_String( const char *szExec, int start, void *globals, void *locals )
{
    using FN = void *( __fastcall * ) ( const char *, int, void *, void * );

    return Resolve< FN >( "PyRun_String" )( szExec, start, globals, locals );
}

void CPython::Py_DecRef( void *pObject )
{
    using FN = void( __cdecl * )( void * );

    return Resolve< FN >( "Py_DecRef" )( pObject );
}

void *CPython::PyGILState_Ensure( )
{
    using FN = void *( __cdecl * ) ( );

    return Resolve< FN >( "PyGILState_Ensure" )( );
}

void CPython::PyGILState_Release( void *pObject )
{
    using FN = void( __cdecl * )( void * );

    return Resolve< FN >( "PyGILState_Release" )( pObject );
}

void *CPython::PyErr_Occurred( )
{
    using FN = void *( __cdecl * ) ( );

    return Resolve< FN >( "PyErr_Occurred" )( );
}

void CPython::PyErr_Print( )
{
    using FN = void( __cdecl * )( );

    return Resolve< FN >( "PyErr_Print" )( );
}

void *CPython::PyCFunction_New( void *ml, void *self )
{
    using FN = void *( __cdecl * ) ( void *, void * );

    return Resolve< FN >( "PyCFunction_New" )( ml, self );
}

int CPython::PyDict_SetItemString( void *p, const char *key, void *val )
{
    using FN = int( __cdecl * )( void *, const char *, void * );

    return Resolve< FN >( "PyDict_SetItemString" )( p, key, val );
}

int CPython::PyArg_ParseTuple( void *args, const char *format, ... )
{
    using FN = int( __cdecl * )( void *, const char *, va_list );

    auto fn = Resolve< FN >( "PyArg_ParseTuple" );

    va_list va;
    va_start( va, format );

    int result = fn( args, format, va );

    va_end( va );

    return result;
}

int CPython::PyArg_VaParse( void *args, const char *format, ... )
{
    using FN = int( __cdecl * )( void *, const char *, va_list );

    auto fn = Resolve< FN >( "PyArg_VaParse" );

    va_list va;
    va_start( va, format );

    int result = fn( args, format, va );

    va_end( va );

    return result;
}

void *CPython::PyBool_FromLong( long value )
{
    using FN = void *( __cdecl * ) ( long );

    return Resolve< FN >( "PyBool_FromLong" )( value );
}

void *CPython::PyLong_FromUnsignedLongLong( unsigned long long value )
{
    using FN = void *( __cdecl * ) ( unsigned long long );

    return Resolve< FN >( "PyLong_FromUnsignedLongLong" )( value );
}

void *CPython::PyFloat_FromDouble( double value )
{
    using FN = void *( __cdecl * ) ( double );

    return Resolve< FN >( "PyFloat_FromDouble" )( value );
}

void *CPython::PyModule_New( const char *name )
{
    using FN = void *( __cdecl * ) ( const char * );

    return Resolve< FN >( "PyModule_New" )( name );
}

void *CPython::PyImport_GetModuleDict( )
{
    using FN = void *( __cdecl * ) ( );

    return Resolve< FN >( "PyImport_GetModuleDict" )( );
}

void CPython::AddPythonFunction( void *module, const char *name, void *callback, const char *doc )
{
    // PyCFunction_New keeps a reference to this PyMethodDef for the lifetime of the
    // resulting callable (it does not copy it), so these must live as long as the
    // process/module does. A static container with stable references replaces the
    // previous unmanaged `new PyMethodDef` (no matching delete, and no way to reclaim it).
    static std::deque< PyMethodDef > s_MethodDefs;

    PyMethodDef &def = s_MethodDefs.emplace_back( );

    def.ml_name  = name;
    def.ml_meth  = callback;
    def.ml_flags = 0x0001; // METH_VARARGS
    def.ml_doc   = doc;

    void *func = PyCFunction_New( &def, nullptr );

    if ( func )
    {
        void *dict = PyModule_GetDict( module );

        PyDict_SetItemString( dict, name, func );

        Py_DecRef( func );
    }
}

bool CPython::ExecutePythonFunction( void *callable, void *args, void **result_out )
{
    if ( !callable || !PyCallable_Check( callable ) )
        return false;

    void *gil = PyGILState_Ensure( );

    void *result = PyObject_CallObject( callable, args );

    if ( !result )
    {
        PyErr_Print( );
        PyGILState_Release( gil );
        return false;
    }

    if ( result_out )
        *result_out = result;
    else
        Py_DecRef( result );

    PyGILState_Release( gil );
    return true;
}

int CPython::PyObject_IsTrue( void *obj )
{
    using FN = int( __cdecl * )( void * );

    return Resolve< FN >( "PyObject_IsTrue" )( obj );
}

void *CPython::PyObject_CallMethodNoArgs( void *self, const char *name )
{
    using FN = void *( __cdecl * ) ( void *, const char * );

    return Resolve< FN >( "PyObject_CallMethodNoArgs" )( self, name );
}

void *CPython::CallStatic( const char *moduleName, const char *objectName, const char *functionName )
{
    void *module = GetPython( )->PyImport_ImportModule( moduleName );
    if ( !module )
        return nullptr;

    void *object = GetPython( )->PyObject_GetAttrString( module, objectName );
    if ( !object )
        return nullptr;

    void *function = GetPython( )->PyObject_GetAttrString( object, functionName );
    if ( !function || !GetPython( )->PyCallable_Check( function ) )
        return nullptr;

    return GetPython( )->PyObject_CallObject( function, nullptr );
}

void *CPython::is_midi_preview( void *self, void *args )
{
    return GetPython( )->PyBool_FromLong( GetWindow( )->GetMidiPreview( ) );
}

void *CPython::send_midi_preview( void *self, void *args )
{
    auto  hwnd    = GetWindow( )->GetHWND( );
    HMENU hMenu   = GetMenu( hwnd );
    HMENU outMenu = nullptr;
    UINT  uID     = 0;

    if ( !hMenu )
        return GetPython( )->PyBool_FromLong( 0 );

    if ( GetWindow( )->FindMenuItemID( hMenu, "MIDI Note Editor Preview", uID, outMenu ) )
        SendMessage( hwnd, WM_COMMAND, uID, 0 );

    return GetPython( )->PyBool_FromLong( 0 );
}

void *CPython::is_key_released( void *self, void *args )
{
    int iKey = 0;
    if ( !GetPython( )->PyArg_VaParse( args, "i", &iKey ) )
        return GetPython( )->PyBool_FromLong( 0 );

    auto result = GetWindow( )->IsKeyReleased( iKey );

    return GetPython( )->PyBool_FromLong( result );
}

void *CPython::is_key_down( void *self, void *args )
{
    int iKey = 0;
    if ( !GetPython( )->PyArg_VaParse( args, "i", &iKey ) )
        return GetPython( )->PyBool_FromLong( 0 );

    auto result = GetWindow( )->IsKeyDown( iKey );

    return GetPython( )->PyBool_FromLong( result );
}

void *CPython::is_key_pressed( void *self, void *args )
{
    int iKey = 0;
    if ( !GetPython( )->PyArg_VaParse( args, "i", &iKey ) )
        return GetPython( )->PyBool_FromLong( 0 );

    auto result = GetWindow( )->IsKeyPressed( iKey );

    return GetPython( )->PyBool_FromLong( result );
}

void *CPython::get_detail_clip( void *self, void *args )
{
    auto result = GetWindow( )->GetDetailClip( );

    return GetPython( )->PyLong_FromUnsignedLongLong( result );
}

void *CPython::is_selected_track( void *self, void *args )
{
    bool     result = false;
    uint64_t track  = 0;
    if ( !GetPython( )->PyArg_VaParse( args, "K", &track ) )
        return GetPython( )->PyBool_FromLong( result );

    result = GetWindow( )->IsSelectedTrack( track );

    return GetPython( )->PyBool_FromLong( result );
}

void *CPython::time_selection_start_time( void *self, void *args )
{
    auto result = GetWindow( )->StartTime( );

    return GetPython( )->PyFloat_FromDouble( result );
}

void *CPython::time_selection_end_time( void *self, void *args )
{
    auto result = GetWindow( )->EndTime( );

    return GetPython( )->PyFloat_FromDouble( result );
}

CPython *GetPython( )
{
    return &g_CPython;
}