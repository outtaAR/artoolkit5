/*
 *  arvrml.h
 *  ARToolKit5
 *
 *  This file is part of ARToolKit.
 *
 *  ARToolKit is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ARToolKit is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ARToolKit.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  As a special exception, the copyright holders of this library give you
 *  permission to link this library with independent modules to produce an
 *  executable, regardless of the license terms of these independent modules, and to
 *  copy and distribute the resulting executable under terms of your choice,
 *  provided that you also meet, for each linked independent module, the terms and
 *  conditions of the license of that module. An independent module is a module
 *  which is neither derived from nor based on this library. If you modify this
 *  library, you may extend this exception to your version of the library, but you
 *  are not obligated to do so. If you do not wish to do so, delete this exception
 *  statement from your version.
 *
 *  Copyright 2015 Daqri, LLC.
 *  Copyright 2002-2015 ARToolworks, Inc.
 *
 *  Modification to this file Copyright (c) 2016 Verizon.
 *   - Improved the integration between native iOS and ARToolkit library
 *
 *  Author(s): Hirokazu Kato, Philip Lamb
 *
 */
#ifndef AR_VRML_H
#define AR_VRML_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#  ifdef LIBARVRML_EXPORTS
#    define AR_DLL_API __declspec(dllexport)
#  else
#    ifdef _DLL
#      define AR_DLL_API __declspec(dllimport)
#    else
#      define AR_DLL_API extern
#    endif
#  endif
#else
#  define AR_DLL_API
#endif

AR_DLL_API int arVrmlLoadFile(const char *file);
AR_DLL_API int arVrmlFree( int id );
AR_DLL_API int arVrmlDraw( int id );
AR_DLL_API int arVrmlTimerUpdate( void );
AR_DLL_API int arVrmlSetInternalLight( int flag );

#ifdef __cplusplus
}
#endif

#endif
