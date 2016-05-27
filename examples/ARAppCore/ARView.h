#import "EAGLView.h"
#import "ARMarker.h" // ARVec3

@class ARViewController;

@protocol ARViewTouchDelegate<NSObject>
@optional
- (void) handleTouchAtLocation:(CGPoint)location tapCount:(NSUInteger)tapCount;
@end

// Notifications.
extern NSString *const ARViewUpdatedCameraLensNotification;
extern NSString *const ARViewUpdatedCameraPoseNotification;
extern NSString *const ARViewUpdatedViewportNotification;
extern NSString *const ARViewDrawPreCameraNotification;
extern NSString *const ARViewDrawPostCameraNotification;
extern NSString *const ARViewDrawOverlayNotification;
extern NSString *const ARViewTouchNotification;

enum viewPortIndices {
    viewPortIndexLeft = 0,
    viewPortIndexBottom,
    viewPortIndexWidth,
    viewPortIndexHeight
};

typedef enum {
    ARViewContentScaleModeStretch = 0,
    ARViewContentScaleModeFit,
    ARViewContentScaleModeFill,
    ARViewContentScaleModeFit1to1
} ARViewContentScaleMode;

typedef enum {
    ARViewContentAlignModeTopLeft = 0,
    ARViewContentAlignModeTop,
    ARViewContentAlignModeTopRight,
    ARViewContentAlignModeLeft,
    ARViewContentAlignModeCenter,
    ARViewContentAlignModeRight,
    ARViewContentAlignModeBottomLeft,
    ARViewContentAlignModeBottom,
    ARViewContentAlignModeBottomRight,
} ARViewContentAlignMode;

@interface ARView : EAGLView <ARViewTouchDelegate> {
}

- (id) initWithFrame:(CGRect)frame pixelFormat:(NSString*)format depthFormat:(EAGLDepthFormat)depth withStencil:(BOOL)stencil preserveBackbuffer:(BOOL)retained;
@property float *cameraLens;
@property float *cameraPose;
@property(readonly) GLint *viewPort;
- (void) drawView:(id)sender;

// Points to the parent view controller.
@property (nonatomic, assign) IBOutlet ARViewController *arViewController;

// These properties allow variation on the way content is drawn in the GL window.
@property int contentWidth;
@property int contentHeight;
@property BOOL contentRotate90;
@property BOOL contentFlipH;
@property BOOL contentFlipV;
@property ARViewContentScaleMode contentScaleMode; // Defaults to ARViewContentScaleModeFill.
@property ARViewContentAlignMode contentAlignMode; // Defaults to ARViewContentAlignModeCenter.

// Interaction.
@property(nonatomic, assign) id <ARViewTouchDelegate> touchDelegate;
@property(nonatomic, readonly) BOOL rayIsValid;
@property(nonatomic, readonly) ARVec3 rayPoint1;
@property(nonatomic, readonly) ARVec3 rayPoint2;

@end
