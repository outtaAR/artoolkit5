/*
 *  video.c
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
 *  Copyright 2003-2015 ARToolworks, Inc.
 *
 *  Modification to this file Copyright (c) 2016 Catalyst Foundry.
 *   - Improved the integration between native iOS and ARToolkit library
 *
 *  Author(s): Hirokazu Kato, Philip Lamb
 *
 */
/* 
 *   author: Hirokazu Kato ( kato@sys.im.hiroshima-cu.ac.jp )
 *
 *   Revision: 6.0   Date: 2003/09/29
 */
#include <stdio.h>
#include <AR/video.h>

static AR2VideoParamT   *vid = NULL;

int arVideoGetDefaultDevice( void )
{
#if defined(AR_DEFAULT_INPUT_V4L)
    return AR_VIDEO_DEVICE_V4L;
#elif defined(AR_DEFAULT_INPUT_DV)
    return AR_VIDEO_DEVICE_DV;
#elif defined(AR_DEFAULT_INPUT_1394CAM)
    return AR_VIDEO_DEVICE_1394CAM;
#elif defined(AR_DEFAULT_INPUT_SGI)
    return AR_VIDEO_DEVICE_SGI;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_DIRECTSHOW)
    return AR_VIDEO_DEVICE_WINDOWS_DIRECTSHOW;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_DSVIDEOLIB)
    return AR_VIDEO_DEVICE_WINDOWS_DSVIDEOLIB;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_DRAGONFLY)
    return AR_VIDEO_DEVICE_WINDOWS_DRAGONFLY;
#elif defined(AR_DEFAULT_INPUT_QUICKTIME)
    return AR_VIDEO_DEVICE_QUICKTIME;
#elif defined(AR_DEFAULT_INPUT_GSTREAMER)
    return AR_VIDEO_DEVICE_GSTREAMER;
#elif defined(AR_DEFAULT_INPUT_IPHONE)
    return AR_VIDEO_DEVICE_IPHONE;
#elif defined(AR_DEFAULT_INPUT_QUICKTIME7)
    return AR_VIDEO_DEVICE_QUICKTIME7;
#elif defined(AR_DEFAULT_INPUT_IMAGE)
    return AR_VIDEO_DEVICE_IMAGE;
#elif defined(AR_DEFAULT_INPUT_ANDROID)
    return AR_VIDEO_DEVICE_ANDROID;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_MEDIA_FOUNDATION)
    return AR_VIDEO_DEVICE_WINDOWS_MEDIA_FOUNDATION;
#elif defined(AR_DEFAULT_INPUT_WINDOWS_MEDIA_CAPTURE)
    return AR_VIDEO_DEVICE_WINDOWS_MEDIA_CAPTURE;
#else
    return AR_VIDEO_DEVICE_DUMMY;
#endif
}

int arVideoOpen( const char *config )
{
    if( vid != NULL ) {
        ARLOGe("arVideoOpen: Error, video device already open.\n");
        return -1;
    }
    vid = ar2VideoOpen( config );
    if( vid == NULL ) return -1;

    return 0;
}

int arVideoOpenAsync(const char *config, void (*callback)(void *), void *userdata)
{
    if( vid != NULL ) {
        ARLOGe("arVideoOpenAsync: Error, video device already open.\n");
        return -1;
    }
    vid = ar2VideoOpenAsync(config, callback, userdata);
    if( vid == NULL ) return -1;
    
    return 0;
}

int arVideoClose( void )
{
    int     ret;

    if( vid == NULL ) return -1;

    ret = ar2VideoClose( vid );
    vid = NULL;

    return ret;
}

int arVideoDispOption( void )
{
    if( vid == NULL ) return -1;

    return  ar2VideoDispOption( vid );
}

int arVideoGetDevice( void )
{
    if( vid == NULL ) return -1;
    
    return ar2VideoGetDevice(vid);
}

int arVideoGetId( ARUint32 *id0, ARUint32 *id1 )
{
    if( vid == NULL ) return -1;

    return ar2VideoGetId( vid, id0, id1 );
}

int arVideoGetSize( int *x, int *y )
{
    if( vid == NULL ) return -1;

    return ar2VideoGetSize( vid, x, y );
}

int arVideoGetPixelSize( void )
{
    if( vid == NULL ) return -1;

    return ar2VideoGetPixelSize( vid );
}

AR_PIXEL_FORMAT arVideoGetPixelFormat( void )
{
    if( vid == NULL ) return ((AR_PIXEL_FORMAT)-1);

    return ar2VideoGetPixelFormat( vid );
}

ARUint8 *arVideoGetImage( void )
{
    AR2VideoBufferT *buffer;

    if( vid == NULL ) return NULL;

    buffer = ar2VideoGetImage(vid);
	if (buffer == NULL) return (NULL);
    if( buffer->fillFlag == 0 ) return NULL;

    return buffer->buff;
}

int arVideoCapStart( void )
{
    if( vid == NULL ) return -1;

    return ar2VideoCapStart( vid );
}

int arVideoCapStartAsync(AR_VIDEO_FRAME_READY_CALLBACK callback, void *userdata)
{
    if( vid == NULL ) return -1;
    
    return ar2VideoCapStartAsync(vid, callback, userdata);
}

int arVideoCapStop( void )
{
    if( vid == NULL ) return -1;

    return ar2VideoCapStop( vid );
}

int   arVideoGetParami( int paramName, int *value )
{
    if (paramName == AR_VIDEO_GET_VERSION) return (ar2VideoGetParami(NULL, AR_VIDEO_GET_VERSION, NULL));
                                                    
    if( vid == NULL ) return -1;

    return ar2VideoGetParami( vid, paramName, value );
}

int   arVideoSetParami( int paramName, int  value )
{
    if( vid == NULL ) return -1;

    return ar2VideoSetParami( vid, paramName, value );
}

int   arVideoGetParamd( int paramName, double *value )
{
    if( vid == NULL ) return -1;

    return ar2VideoGetParamd( vid, paramName, value );
}

int   arVideoSetParamd( int paramName, double  value )
{
    if( vid == NULL ) return -1;

    return ar2VideoSetParamd( vid, paramName, value );
}

int   arVideoSaveParam( char *filename )
{
    if( vid == NULL ) return -1;

    return ar2VideoSaveParam( vid, filename );
}

int   arVideoLoadParam( char *filename )
{
    if( vid == NULL ) return -1;

    return ar2VideoLoadParam( vid, filename );
}

int arVideoSetBufferSize(const int width, const int height)
{
    if( vid == NULL ) return -1;
    
    return ar2VideoSetBufferSize( vid, width, height );
}

int arVideoGetBufferSize(int *width, int *height)
{
    if( vid == NULL ) return -1;
    
    return ar2VideoGetBufferSize( vid, width, height );
}

int arVideoGetCParam(ARParam *cparam)
{
    if (vid == NULL) return -1;
    
    return ar2VideoGetCParam(vid, cparam);
}

int arVideoGetCParamAsync(void (*callback)(const ARParam *, void *), void *userdata)
{
    if (vid == NULL) return -1;
    
    return ar2VideoGetCParamAsync(vid, callback, userdata);;
}

// N.B. This function is duplicated in libAR, so that libAR doesn't need to
// link to libARvideo. Therefore, if changes are made here they should be duplicated there.
int arVideoUtilGetPixelSize( const AR_PIXEL_FORMAT arPixelFormat )
{
    switch( arPixelFormat ) {
        case AR_PIXEL_FORMAT_RGB:
        case AR_PIXEL_FORMAT_BGR:
            return 3;
        case AR_PIXEL_FORMAT_RGBA:
        case AR_PIXEL_FORMAT_BGRA:
        case AR_PIXEL_FORMAT_ABGR:
        case AR_PIXEL_FORMAT_ARGB:
            return 4;
        case AR_PIXEL_FORMAT_MONO:
        case AR_PIXEL_FORMAT_420v: // Report only size of luma pixels (i.e. plane 0).
        case AR_PIXEL_FORMAT_420f: // Report only size of luma pixels (i.e. plane 0).
        case AR_PIXEL_FORMAT_NV21: // Report only size of luma pixels (i.e. plane 0).
            return 1;
        case AR_PIXEL_FORMAT_2vuy:
        case AR_PIXEL_FORMAT_yuvs:
        case AR_PIXEL_FORMAT_RGB_565:
        case AR_PIXEL_FORMAT_RGBA_5551:
        case AR_PIXEL_FORMAT_RGBA_4444:
            return 2;
        default:
            return (0);
    }
}

// N.B. This function is duplicated in libAR, so that libAR doesn't need to
// link to libARvideo. Therefore, if changes are made here they should be duplicated there.
const char *arVideoUtilGetPixelFormatName(const AR_PIXEL_FORMAT arPixelFormat)
{
    const char *names[] = {
        "AR_PIXEL_FORMAT_RGB",
        "AR_PIXEL_FORMAT_BGR",
        "AR_PIXEL_FORMAT_RGBA",
        "AR_PIXEL_FORMAT_BGRA",
        "AR_PIXEL_FORMAT_ABGR",
        "AR_PIXEL_FORMAT_MONO",
        "AR_PIXEL_FORMAT_ARGB",
        "AR_PIXEL_FORMAT_2vuy",
        "AR_PIXEL_FORMAT_yuvs",
        "AR_PIXEL_FORMAT_RGB_565",
        "AR_PIXEL_FORMAT_RGBA_5551",
        "AR_PIXEL_FORMAT_RGBA_4444",
        "AR_PIXEL_FORMAT_420v",
        "AR_PIXEL_FORMAT_420f",
        "AR_PIXEL_FORMAT_NV21"
    };
    if ((int)arPixelFormat < 0 || (int)arPixelFormat > AR_PIXEL_FORMAT_MAX) {
        ARLOGe("arVideoUtilGetPixelFormatName: Error, unrecognised pixel format (%d).\n", (int)arPixelFormat);
        return (NULL);
    }
    return (names[(int)arPixelFormat]);
}
