#include "pch/pch.h"

static CLive g_CLive { };

using PyObject = void;

void *CLive::GetDocument( )
{
    auto *Application = GetApplication( );
    if ( !Application )
        return nullptr;

    auto *get_document = GetPython( )->PyObject_GetAttrString( Application, "get_document" );

    auto *Document = GetPython( )->PyObject_CallObject( get_document, nullptr );

    return Document;
}

bool CLive::IsPlaying( )
{
    auto *Document = GetDocument( );
    if ( !Document )
        return false;

    auto *is_playing = GetPython( )->PyObject_GetAttrString( Document, "is_playing" );

    if ( !is_playing )
    {
        GetPython( )->PyErr_Print( );
        return false;
    }

    int result = GetPython( )->PyObject_IsTrue( is_playing );

    GetPython( )->Py_DecRef( is_playing );

    if ( result == -1 )
    {
        GetPython( )->PyErr_Print( );
        return false;
    }

    return result == 1;
}

void CLive::SetApplication( ALiveApp* pApp )
{
    App = pApp;
}

ALiveApp *CLive::GetApplication( )
{
    return App;
}

CLive *GetLive( )
{
    return &g_CLive;
}