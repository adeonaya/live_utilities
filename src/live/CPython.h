#pragma once

typedef struct
{
    const char *ml_name;
    void       *ml_meth;
    int         ml_flags;
    const char *ml_doc;
} PyMethodDef;

using PyObject = void;

class CPython
{
  public:
    template < typename FN >
    FN    Resolve( const char *name );
    void  Py_Initialize( );
    void *PyImport_AddModule( const char *szName );
    void *PyModule_GetDict( void *pModule );
    void *PyRun_String( const char *szExec, int start, void *globals, void *locals );
    void  Py_DecRef( void *pObject );
    void *PyGILState_Ensure( );
    void  PyGILState_Release( void *pObject );
    void *PyErr_Occurred( );
    void  PyErr_Print( );
    void *PyCFunction_New( void *ml, void *self );
    int   PyDict_SetItemString( void *p, const char *key, void *val );
    int   PyArg_ParseTuple( void *args, const char *format, ... );
    int   PyArg_VaParse( void *args, const char *format, ... );
    void *PyFloat_FromDouble( double value );
    void *PyLong_FromLong( long value );
    void *PyBool_FromLong( long value );
    void *PyLong_FromUnsignedLongLong( unsigned long long value );
    void *PyModule_New( const char *name );
    void *PyImport_GetModuleDict( );
    void *PyImport_ImportModule( const char *szName );
    void *PyObject_GetAttrString( void *obj, const char *attr_name );
    void *PyUnicode_FromString( const char *str );
    void *PyObject_CallObject( void *callable, void *args );
    void *PyTuple_New( intptr_t size );
    int   PyTuple_SetItem( void *tuple, intptr_t index, void *item );
    int   PyCallable_Check( void *obj );

    void *PyObject_CallMethod( void *obj, const char *method, const char *format, ... );
    int  PyObject_IsTrue( void *obj );
    template < typename... Args >
    void *CallMethod( void *obj, const char *method, Args... args );
    void  AddPythonFunction( void *module, const char *name, void *callback, const char *doc );
    bool  ExecutePythonFunction( void *callable, void *args, void **result_out = nullptr );
    void *PyObject_CallMethodNoArgs( void *self, const char *name );
    void *CallStatic( const char *moduleName, const char *objectName, const char *functionName );


    static void *is_midi_preview( void *self, void *args );
    static void *send_midi_preview( void *self, void *args );
    static void *is_key_released( void *self, void *args );
    static void *is_key_down( void *self, void *args );
    static void *is_key_pressed( void *self, void *args );
    static void *get_detail_clip( void *self, void *args );
    static void *is_selected_track( void *self, void *args );
    static void *time_selection_start_time( void *self, void *args );
    static void *time_selection_end_time( void *self, void *args );

  private:
};

CPython *GetPython( );