
#import "ARMarker.h"
#import "ARMarkerSquare.h"
#import "ARMarkerNFT.h"

#import <AR/gsub_es.h>
#import <Eden/EdenMath.h> // EdenMathInvertMatrix().

#import <stdio.h>
#import <stdlib.h>
#import <string.h>
#import <sys/param.h> // MAXPATHLEN

NSString *const ARMarkerCreatedNotification = @"ARMarkerCreatedNotification";
NSString *const ARMarkerAppearedNotification = @"ARMarkerAppearedNotification";
NSString *const ARMarkerDisappearedNotification = @"ARMarkerDisappearedNotification";
NSString *const ARMarkerUpdatedPoseNotification = @"ARMarkerUpdatedPoseNotification";
NSString *const ARMarkerDestroyedNotification = @"ARMarkerDestroyedNotification";

const ARPose ARPoseUnity = {{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};

static char *get_buff(char *buf, int n, FILE *fp, int skipblanks)
{
    char *ret;
    
    do {
        ret = fgets(buf, n, fp);
        if (ret == NULL) return (NULL); // EOF or error.
        
        // Remove NLs and CRs from end of string.
        size_t l = strlen(buf);
        while (l > 0) {
            if (buf[l - 1] != '\n' && buf[l - 1] != '\r') break;
            l--;
            buf[l] = '\0';
        }
    } while (buf[0] == '#' || (skipblanks && buf[0] == '\0')); // Reject comments and blank lines.
    
    return (ret);
}


@implementation ARMarker {
    ARdouble   positionScalefactor;
    ARFilterTransMatInfo *ftmi;
    ARdouble   filterCutoffFrequency;
    ARdouble   filterSampleRate;
    BOOL       needToCalculatePoseInverse;
}

@synthesize name, valid, pose, marker_width, marker_height, positionScalefactor;

+ (NSMutableArray *)newNFTMarkers:(NSString *)markerName
{
    NSString      *markersConfigDataDir;
    NSMutableArray *markers = [[NSMutableArray array] retain];
    
    // Load the marker data file.
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0]; // Get documents folder
    markersConfigDataDir = [documentsDirectory stringByAppendingPathComponent:markerName];
    
    ARMarker *tempObject = [[ARMarkerNFT alloc] initWithNFTDataSetPathname:[markersConfigDataDir UTF8String]];
    
    if (tempObject) {
        tempObject.filtered = TRUE;
        tempObject.filterCutoffFrequency = 15.0f;
        [markers addObject:tempObject];
        [tempObject release];
    }
    
    return (markers);
}

+ (NSMutableArray *)newMarkersFromConfigDataFile:(NSString *)markersConfigDataFilePath arPattHandle:(ARPattHandle *)arPattHandle_in arPatternDetectionMode:(int *)patternDetectionMode_out
{
    NSString      *markersConfigDataFileFullPath;
    int            numMarkers;
    NSMutableArray *markers;
    FILE          *fp;
    char           buf[MAXPATHLEN], buf1[MAXPATHLEN];
    int            tempI;
    ARdouble       tempF;
    int            i;
    int            patt_type = 0;

    markers = [[NSMutableArray array] retain];
    
    // Load the marker data file.
    if ([markersConfigDataFilePath hasPrefix:@"/"]) {
        markersConfigDataFileFullPath = markersConfigDataFilePath;
    } else {
        NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
        NSString *documentsDirectory = [paths objectAtIndex:0]; // Get documents folder
        
        markersConfigDataFileFullPath = [documentsDirectory stringByAppendingPathComponent:markersConfigDataFilePath];
    }
    NSString *markersConfigDataDir = [markersConfigDataFileFullPath stringByDeletingLastPathComponent];
    char markersConfigDataFileFullPathC[MAXPATHLEN];
    [markersConfigDataFileFullPath getFileSystemRepresentation:markersConfigDataFileFullPathC maxLength:MAXPATHLEN];
    if ((fp = fopen(markersConfigDataFileFullPathC, "r")) == NULL) {
        NSLog(@"Error: unable to locate object data file %@.\n", markersConfigDataFileFullPath);
        [markers release];
        return nil;
    }
    
    // First line is number of markers to read.
    get_buff(buf, MAXPATHLEN, fp, 1);
    if (sscanf(buf, "%d", &numMarkers) != 1 ) {
        NSLog(@"Error in marker configuration data file; expected marker count.\n");
        fclose(fp);
        [markers release];
        return nil;
    }
    
#ifdef DEBUG
    NSLog(@"Reading %d marker configuration(s).\n", numMarkers);
#endif
    
    for (i = 0; i < numMarkers; i++) {
        
        ARMarker *tempObject = NULL;
        
        // Read marker name.
        if (!get_buff(buf, MAXPATHLEN, fp, 1)) {
            NSLog(@"Error in marker configuration data file; expected marker name.\n");
            break;
        }
        
        // Read marker type.
        if (!get_buff(buf1, MAXPATHLEN, fp, 1)) {
            NSLog(@"Error in marker configuration data file; expected marker type.\n");
            break;
        }
        
        // Interpret marker type, and read more data.
        if (strcmp(buf1, "SINGLE") == 0) {
            
            // Read marker width.
            if (!get_buff(buf1, MAXPATHLEN, fp, 1) || sscanf(buf1, 
#ifdef ARDOUBLE_IS_FLOAT
                                                             "%f"
#else
                                                             "%lf"
#endif
                                                             , &tempF) != 1) {
                NSLog(@"Error in marker configuration data file; expected marker width.\n");
                break;
            }
            
            // Interpret marker name (still in buf), test if it's a pattern number, load as pattern file if not.
            if (sscanf(buf, "%d", &tempI) != 1) {
                if (!arPattHandle_in) {
                    break;
                }
                tempObject = [[ARMarkerSquare alloc] initWithPatternFile:[markersConfigDataDir stringByAppendingPathComponent:[NSString stringWithCString:buf encoding:NSUTF8StringEncoding]]
                                                             width:tempF arPattHandle:arPattHandle_in];
                patt_type |= 0x01;
            } else {
                tempObject = [[ARMarkerSquare alloc] initWithBarcode:tempI width:tempF];
                patt_type |= 0x02;
            }
        } else if (strcmp(buf1, "NFT") == 0) {
            tempObject = [[ARMarkerNFT alloc] initWithNFTDataSetPathname:[[markersConfigDataDir stringByAppendingPathComponent:[NSString stringWithCString:buf encoding:NSUTF8StringEncoding]] UTF8String]];
        } else {
            NSLog(@"Error in marker configuration data file; unsupported marker type %s.\n", buf1);
        }
                    
        // Look for optional tokens. A blank line marks end of options.
        while (get_buff(buf, MAXPATHLEN, fp, 0) && (buf[0] != '\0')) {
            if (strncmp(buf, "FILTER", 6) == 0) {
                if (tempObject) {
                    if (strlen(buf) != 6) {
                        if (sscanf(&buf[6],
#ifdef ARDOUBLE_IS_FLOAT
                                   "%f"
#else
                                   "%lf"
#endif
                                   , &tempF) == 1) tempObject.filterCutoffFrequency = tempF;
                    }
                    tempObject.filtered = TRUE;
                }
            }
            // Unknown tokens are ignored.
        }
        
        if (tempObject) {
            [markers addObject:tempObject];
            [tempObject release];
        }
    }
    
    // Work out square marker detection mode.
    if (patternDetectionMode_out) {
        if ((patt_type & 0x03) == 0x03) *patternDetectionMode_out = AR_TEMPLATE_MATCHING_COLOR_AND_MATRIX;
        else if (patt_type & 0x02)      *patternDetectionMode_out = AR_MATRIX_CODE_DETECTION;
        else                            *patternDetectionMode_out = AR_TEMPLATE_MATCHING_COLOR;
    }
    
    fclose(fp);
    return (markers);
}

+ (ARMarker *)findMarkerWithName:(NSString *)name inMarkers:(NSArray *)markers
{
    ARMarker *marker;
    
    for (marker in markers) {
        if ([marker.name isEqualToString:name]) break;
    }
    return (marker);
}

- (id) init
{
    if ((self = [super init])) {
        valid = validPrev = FALSE;
        positionScalefactor = 1.0f;
        ftmi = NULL;
        filterCutoffFrequency = AR_FILTER_TRANS_MAT_CUTOFF_FREQ_DEFAULT;
        filterSampleRate = AR_FILTER_TRANS_MAT_SAMPLE_RATE_DEFAULT;
        
        [[NSNotificationCenter defaultCenter] postNotificationName:ARMarkerCreatedNotification object:self];
    }
    return (self);
}

- (void) dealloc
{
    
    [[NSNotificationCenter defaultCenter] postNotificationName:ARMarkerDestroyedNotification object:self];
    
    if (ftmi) arFilterTransMatFinal(ftmi);
    
    [super dealloc];
}

- (void) setFiltered:(BOOL)flag
{
    if (flag && !ftmi) {
        ftmi = arFilterTransMatInit(filterSampleRate, filterCutoffFrequency);
    } else if (!flag && ftmi) {
        arFilterTransMatFinal(ftmi);
        ftmi = NULL;
    }
}

- (BOOL)isFiltered
{
    return (ftmi != NULL);
}

- (ARdouble)filterSampleRate
{
    return filterSampleRate;
}

- (void) setFilterSampleRate:(ARdouble)rate
{
    filterSampleRate = rate;
    if (ftmi) arFilterTransMatSetParams(ftmi, filterSampleRate, filterCutoffFrequency);
}

- (ARdouble)filterCutoffFrequency
{
    return filterCutoffFrequency;
}

- (void) setFilterCutoffFrequency:(ARdouble)freq
{
    filterCutoffFrequency = freq;
    if (ftmi) arFilterTransMatSetParams(ftmi, filterSampleRate, filterCutoffFrequency);
}

- (void) update
{
    if (valid) {
        
        // Filter the pose estimate.
        if (ftmi) {
            if (arFilterTransMat(ftmi, trans, !validPrev) < 0) {
                NSLog(@"arFilterTransMat error with marker %@.\n", self);
            }
        }
        
        if (!validPrev) {
            // Marker has become visible, tell any dependent objects.
            [[NSNotificationCenter defaultCenter] postNotificationName:ARMarkerAppearedNotification object:self];
        }

        // We have a new pose, so set that.
        arglCameraViewRHf(trans, pose.T, positionScalefactor);
        needToCalculatePoseInverse = TRUE;
        // Tell any dependent objects about the update.
        [[NSNotificationCenter defaultCenter] postNotificationName:ARMarkerUpdatedPoseNotification object:self];
        
    } else {
        
        if (validPrev) {
            // Marker has ceased to be visible, tell any dependent objects.
            [[NSNotificationCenter defaultCenter] postNotificationName:ARMarkerDisappearedNotification object:self];
        }
    }                    
}

- (ARPose)poseInverse
{
    if (needToCalculatePoseInverse) {
        EdenMathInvertMatrix(poseInverse.T, pose.T);
        needToCalculatePoseInverse = FALSE;
    }
    return (poseInverse);
}

@end
