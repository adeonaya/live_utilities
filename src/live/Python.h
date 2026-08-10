#pragma once

template < typename FN >
FN Resolve( const char* name );

typedef struct
{
    const char *ml_name;
    void       *ml_meth;
    int         ml_flags;
    const char *ml_doc;
} PyMethodDef;

auto Py_Initialize( ) -> void;
auto PyImport_AddModule( const char *szName ) -> void *;
auto PyModule_GetDict( void *pModule ) -> void *;
auto PyRun_String( const char *szExec, int start, void *globals, void *locals ) -> void *;
auto Py_DecRef( void *pObject ) -> void;
auto PyGILState_Ensure( ) -> void *;
auto PyGILState_Release( void *pObject ) -> void;
auto PyErr_Occurred( ) -> void *;
auto PyErr_Print( ) -> void;
auto PyCFunction_New( void *ml, void *self ) -> void *;
auto PyDict_SetItemString( void *p, const char *key, void *val ) -> int;
auto PyArg_ParseTuple( void *args, const char *format, ... ) -> int;
auto PyLong_FromLong( long value ) -> void *;
auto PyBool_FromLong( long value ) -> void *;
auto PyLong_FromUnsignedLongLong( unsigned long long value ) -> void *;
auto PyModule_New( const char *name ) -> void *;
auto PyImport_GetModuleDict( ) -> void *;

void *is_midi_preview( void *self, void *args );
void *send_midi_preview( void *self, void *args );
void *is_key_released( void *self, void *args );
void *is_key_down( void *self, void *args );
void *is_key_pressed( void *self, void *args );
void *get_detail_clip( void *self, void *args );
void *is_selected_track( void *self, void *args );
void *time_selection_start_time( void *self, void *args );
void *time_selection_end_time( void *self, void *args );

void AddPythonFunction( void *module, const char *name, void *callback, const char *doc );




auto PyImport_ImportModule( const char *szName ) -> void *;

void *PyObject_GetAttrString( void *obj, const char *attr_name );

void *PyUnicode_FromString( const char *str );

void *PyObject_CallObject( void *callable, void *args );

void *PyTuple_New( intptr_t size );

int PyTuple_SetItem( void *tuple, intptr_t index, void *item );

void *PyLong_FromLong( long value );

auto PyCallable_Check( void *obj ) -> int;

bool ExecutePythonFunction( void *callable, void *args, void **result_out = nullptr );