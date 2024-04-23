#ifndef R_REF_MOD_H
#define R_REF_MOD_H

#include "../gameshared/q_shared.h"
#include "../gameshared/q_math.h"
#include "../cgame/ref.h"
#include "../gameshared/q_arch.h"
#include "../qcommon/qfiles.h"

typedef struct shader_s shader_t;
typedef struct model_s model_t;
typedef struct skinfile_s skinfile_t;

#define DECLARE_TYPEDEF_METHOD( ret, name, ... ) \
	typedef ret(*name##Fn )( __VA_ARGS__ ); \
	ret name (__VA_ARGS__);

// public API
DECLARE_TYPEDEF_METHOD( rserr_t, RF_Init, const char *applicationName, const char *screenshotPrefix, int startupColor, int iconResource, const int *iconXPM, void *hinstance, void *wndproc, void *parenthWnd, bool verbose );
DECLARE_TYPEDEF_METHOD( rserr_t, RF_SetMode, int x, int y, int width, int height, int displayFrequency, bool fullScreen, bool stereo );
DECLARE_TYPEDEF_METHOD( void, RF_AppActivate, bool active, bool destroy );
DECLARE_TYPEDEF_METHOD( rserr_t, RF_SetWindow, void *hinstance, void *wndproc, void *parenthWnd );
DECLARE_TYPEDEF_METHOD( void, RF_Shutdown, bool verbose );
DECLARE_TYPEDEF_METHOD( void, RF_BeginFrame, float cameraSeparation, bool forceClear, bool forceVsync );
DECLARE_TYPEDEF_METHOD( void, RF_EndFrame, void );
DECLARE_TYPEDEF_METHOD( void, RF_BeginRegistration, void );
DECLARE_TYPEDEF_METHOD( void, RF_EndRegistration, void );
DECLARE_TYPEDEF_METHOD( void, RF_RegisterWorldModel, const char *model, const dvis_t *pvsData );
DECLARE_TYPEDEF_METHOD( void, RF_ClearScene, void );
DECLARE_TYPEDEF_METHOD( void, RF_AddEntityToScene, const entity_t *ent );
DECLARE_TYPEDEF_METHOD( void, RF_AddLightToScene, const vec3_t org, float intensity, float r, float g, float b );
DECLARE_TYPEDEF_METHOD( void, RF_AddPolyToScene, const poly_t *poly );
DECLARE_TYPEDEF_METHOD( void, RF_AddLightStyleToScene, int style, float r, float g, float b );
DECLARE_TYPEDEF_METHOD( void, RF_RenderScene, const refdef_t *fd );
DECLARE_TYPEDEF_METHOD( void, RF_DrawStretchPic, int x, int y, int w, int h, float s1, float t1, float s2, float t2, const vec4_t color, const shader_t *shader );
DECLARE_TYPEDEF_METHOD( void, RF_DrawRotatedStretchPic, int x, int y, int w, int h, float s1, float t1, float s2, float t2, float angle, const vec4_t color, const shader_t *shader );
DECLARE_TYPEDEF_METHOD( void, RF_DrawStretchRaw, int x, int y, int w, int h, int cols, int rows, float s1, float t1, float s2, float t2, uint8_t *data );
DECLARE_TYPEDEF_METHOD( void, RF_DrawStretchRawYUV, int x, int y, int w, int h, float s1, float t1, float s2, float t2, ref_img_plane_t *yuv );
DECLARE_TYPEDEF_METHOD( void, RF_DrawStretchPoly, const poly_t *poly, float x_offset, float y_offset );
DECLARE_TYPEDEF_METHOD( void, RF_SetScissor, int x, int y, int w, int h );
DECLARE_TYPEDEF_METHOD( void, RF_GetScissor, int *x, int *y, int *w, int *h );
DECLARE_TYPEDEF_METHOD( void, RF_ResetScissor, void );
DECLARE_TYPEDEF_METHOD( void, RF_SetCustomColor, int num, int r, int g, int b );
DECLARE_TYPEDEF_METHOD( void, RF_ScreenShot, const char *path, const char *name, const char *fmtstring, bool silent );
DECLARE_TYPEDEF_METHOD( void, RF_EnvShot, const char *path, const char *name, unsigned pixels );
DECLARE_TYPEDEF_METHOD( bool, RF_RenderingEnabled, void );
DECLARE_TYPEDEF_METHOD( const char *, RF_GetSpeedsMessage, char *out, size_t size );
DECLARE_TYPEDEF_METHOD( int, RF_GetAverageFramerate, void );
DECLARE_TYPEDEF_METHOD( void, RF_ReplaceRawSubPic, shader_t *shader, int x, int y, int width, int height, uint8_t *data );
DECLARE_TYPEDEF_METHOD( void, RF_BeginAviDemo, void );
DECLARE_TYPEDEF_METHOD( void, RF_WriteAviFrame, int frame, bool scissor );
DECLARE_TYPEDEF_METHOD( void, RF_StopAviDemo, void );
DECLARE_TYPEDEF_METHOD( void, RF_TransformVectorToScreen, const refdef_t *rd, const vec3_t in, vec2_t out );
DECLARE_TYPEDEF_METHOD( bool, RF_LerpTag, orientation_t *orient, const model_t *mod, int oldframe, int frame, float lerpfrac, const char *name );
DECLARE_TYPEDEF_METHOD( void, RF_LightForOrigin, const vec3_t origin, vec3_t dir, vec4_t ambient, vec4_t diffuse, float radius );
DECLARE_TYPEDEF_METHOD( shader_t *, RF_GetShaderForOrigin, const vec3_t origin );
DECLARE_TYPEDEF_METHOD( struct cinematics_s *, RF_GetShaderCinematic, shader_t *shader );

DECLARE_TYPEDEF_METHOD( void, R_ClearScene, void );
DECLARE_TYPEDEF_METHOD( void, R_AddEntityToScene, const entity_t *ent );
DECLARE_TYPEDEF_METHOD( void, R_AddLightToScene, const vec3_t org, float intensity, float r, float g, float b );
DECLARE_TYPEDEF_METHOD( void, R_AddPolyToScene, const poly_t *poly );
DECLARE_TYPEDEF_METHOD( void, R_RenderScene, const refdef_t *fd );
DECLARE_TYPEDEF_METHOD( void, R_ModelBounds, const model_t *mod, vec3_t mins, vec3_t maxs );
DECLARE_TYPEDEF_METHOD( void, R_ModelFrameBounds, const model_t *mod, int frame, vec3_t mins, vec3_t maxs );
DECLARE_TYPEDEF_METHOD( model_t *, R_RegisterModel, const char *name );
DECLARE_TYPEDEF_METHOD( shader_t *, R_RegisterSkin, const char *name );
DECLARE_TYPEDEF_METHOD( shader_t *, R_RegisterPic, const char *name );
DECLARE_TYPEDEF_METHOD( shader_t *, R_RegisterRawPic, const char *name, int width, int height, uint8_t *data, int samples );
DECLARE_TYPEDEF_METHOD( shader_t *, R_RegisterLevelshot, const char *name, struct shader_s *defaultPic, bool *matchesDefault );
DECLARE_TYPEDEF_METHOD( skinfile_t *, R_RegisterSkinFile, const char *name );
DECLARE_TYPEDEF_METHOD( shader_t *, R_RegisterVideo, const char *name );
DECLARE_TYPEDEF_METHOD( bool, R_LerpTag, orientation_t *orient, const struct model_s *mod, int oldframe, int frame, float lerpfrac, const char *name );
DECLARE_TYPEDEF_METHOD( void, R_DrawStretchPic, int x, int y, int w, int h, float s1, float t1, float s2, float t2, const vec4_t color, const struct shader_s *shader );
DECLARE_TYPEDEF_METHOD( void, R_DrawStretchPoly, const struct poly_s *poly, float x_offset, float y_offset );
DECLARE_TYPEDEF_METHOD( void, R_DrawRotatedStretchPic, int x, int y, int w, int h, float s1, float t1, float s2, float t2, float angle, const vec4_t color, const struct shader_s *shader );
DECLARE_TYPEDEF_METHOD( void, R_Scissor, int x, int y, int w, int h );
DECLARE_TYPEDEF_METHOD( void, R_GetScissor, int *x, int *y, int *w, int *h );
DECLARE_TYPEDEF_METHOD( void, R_ResetScissor, void );
DECLARE_TYPEDEF_METHOD( void, R_GetShaderDimensions, const shader_t *shader, int *width, int *height );
DECLARE_TYPEDEF_METHOD( void, R_TransformVectorToScreen, const refdef_t *rd, vec3_t const in, vec2_t out );
DECLARE_TYPEDEF_METHOD( int, R_SkeletalGetNumBones, const model_t *mod, int *numFrames );
DECLARE_TYPEDEF_METHOD( int, R_SkeletalGetBoneInfo, const model_t *mod, int bone, char *name, size_t name_size, int *flags );
DECLARE_TYPEDEF_METHOD( void, R_SkeletalGetBonePose, const model_t *mod, int bone, int frame, bonepose_t *bonepose );
DECLARE_TYPEDEF_METHOD( struct cinematics_t *, R_GetShaderCinematic, shader_t *shader );

#undef DECLARE_TYPEDEF_METHOD

struct ref_import_s {
	RF_InitFn RF_Init;
	RF_SetModeFn RF_SetMode;
	RF_AppActivateFn RF_AppActivate;
	RF_SetWindowFn RF_SetWindow;
	RF_ShutdownFn RF_Shutdown;
	RF_BeginFrameFn RF_BeginFrame;
	RF_EndFrameFn RF_EndFrame;
	RF_BeginRegistrationFn RF_BeginRegistration;
	RF_EndRegistrationFn RF_EndRegistration;
	RF_RegisterWorldModelFn RF_RegisterWorldModel;
	RF_ClearSceneFn RF_ClearScene;
	RF_AddEntityToSceneFn RF_AddEntityToScene;
	RF_AddLightToSceneFn RF_AddLightToScene;
	RF_AddPolyToSceneFn RF_AddPolyToScene;
	RF_AddLightStyleToSceneFn RF_AddLightStyleToScene;
	RF_RenderSceneFn RF_RenderScene;
	RF_DrawStretchPicFn RF_DrawStretchPic;
	RF_DrawRotatedStretchPicFn RF_DrawRotatedStretchPic;
	RF_DrawStretchRawFn RF_DrawStretchRaw;
	RF_DrawStretchRawYUVFn RF_DrawStretchRawYUV;
	RF_DrawStretchPolyFn RF_DrawStretchPoly;
	RF_SetScissorFn RF_SetScissor;
	RF_GetScissorFn RF_GetScissor;
	RF_ResetScissorFn RF_ResetScissor;
	RF_SetCustomColorFn RF_SetCustomColor;
	RF_ScreenShotFn RF_ScreenShot;
	RF_EnvShotFn RF_EnvShot;
	RF_RenderingEnabledFn RF_RenderingEnabled;
	RF_GetSpeedsMessageFn RF_GetSpeedsMessage;
	RF_GetAverageFramerateFn RF_GetAverageFramerate;
	RF_ReplaceRawSubPicFn RF_ReplaceRawSubPic;
	RF_BeginAviDemoFn RF_BeginAviDemo;
	RF_WriteAviFrameFn RF_WriteAviFrame;
	RF_StopAviDemoFn RF_StopAviDemo;
	RF_TransformVectorToScreenFn RF_TransformVectorToScreen;
	RF_LerpTagFn RF_LerpTag;
	RF_LightForOriginFn RF_LightForOrigin;
	RF_GetShaderForOriginFn RF_GetShaderForOrigin;
	RF_GetShaderCinematicFn RF_GetShaderCinematic;
	R_ClearSceneFn R_ClearScene;
	R_AddEntityToSceneFn R_AddEntityToScene;
	R_AddLightToSceneFn R_AddLightToScene;
	R_AddPolyToSceneFn R_AddPolyToScene;
	R_RenderSceneFn R_RenderScene;
	R_ModelBoundsFn R_ModelBounds;
	R_ModelFrameBoundsFn R_ModelFrameBounds;
	R_RegisterModelFn R_RegisterModel;
	R_RegisterSkinFn R_RegisterSkin;
	R_RegisterPicFn R_RegisterPic;
	R_RegisterRawPicFn R_RegisterRawPic;
	R_RegisterLevelshotFn R_RegisterLevelshot;
	R_RegisterSkinFileFn R_RegisterSkinFile;
	R_RegisterVideoFn R_RegisterVideo;
	R_LerpTagFn R_LerpTag;
	R_DrawStretchPicFn R_DrawStretchPic;
	R_DrawStretchPolyFn R_DrawStretchPoly;
	R_DrawRotatedStretchPicFn R_DrawRotatedStretchPic;
	R_ScissorFn R_Scissor;
	R_GetScissorFn R_GetScissor;
	R_ResetScissorFn R_ResetScissor;
	R_GetShaderDimensionsFn R_GetShaderDimensions;
	R_TransformVectorToScreenFn R_TransformVectorToScreen;
	R_SkeletalGetNumBonesFn R_SkeletalGetNumBones;
	R_SkeletalGetBoneInfoFn R_SkeletalGetBoneInfo;
	R_SkeletalGetBonePoseFn R_SkeletalGetBonePose;
	R_GetShaderCinematicFn R_GetShaderCinematic;
};

#define DECLARE_REF_STRUCT() { \
  RF_Init, \
  RF_SetMode, \
  RF_AppActivate, \
  RF_SetWindow, \
  RF_Shutdown, \
  RF_BeginFrame, \
  RF_EndFrame, \
  RF_BeginRegistration, \
  RF_EndRegistration, \
  RF_RegisterWorldModel, \
  RF_ClearScene, \
  RF_AddEntityToScene, \
  RF_AddLightToScene, \
  RF_AddPolyToScene, \
  RF_AddLightStyleToScene, \
  RF_RenderScene, \
  RF_DrawStretchPic, \
  RF_DrawRotatedStretchPic, \
  RF_DrawStretchRaw, \
  RF_DrawStretchRawYUV, \
  RF_DrawStretchPoly, \
  RF_SetScissor, \
  RF_GetScissor, \
  RF_ResetScissor, \
  RF_SetCustomColor, \
  RF_ScreenShot, \
  RF_EnvShot, \
  RF_RenderingEnabled, \
  RF_GetSpeedsMessage, \
  RF_GetAverageFramerate, \
  RF_ReplaceRawSubPic, \
  RF_BeginAviDemo, \
  RF_WriteAviFrame, \
  RF_StopAviDemo, \
  RF_TransformVectorToScreen, \
  RF_LerpTag, \
  RF_LightForOrigin, \
  RF_GetShaderForOrigin, \
  RF_GetShaderCinematic, \
	R_ClearScene, \
	R_AddEntityToScene, \
	R_AddLightToScene, \
	R_AddPolyToScene, \
	R_RenderScene, \
	R_ModelBounds, \
	R_ModelFrameBounds, \
	R_RegisterModel, \
	R_RegisterSkin, \
	R_RegisterPic, \
	R_RegisterRawPic, \
	R_RegisterLevelshot, \
	R_RegisterSkinFile, \
	R_RegisterVideo, \
	R_LerpTag, \
	R_DrawStretchPic, \
	R_DrawStretchPoly, \
	R_DrawRotatedStretchPic, \
	R_Scissor, \
	R_GetScissor, \
	R_ResetScissor, \
	R_GetShaderDimensions, \
	R_TransformVectorToScreen, \
	R_SkeletalGetNumBones, \
	R_SkeletalGetBoneInfo, \
	R_SkeletalGetBonePose, \
	R_GetShaderCinematic, \
}

struct ref_import_s RF_Forward_Mod();

#if REF_DEFINE_INTERFACE_IMPL
static struct ref_import_s ref_import;
struct ref_import_s RF_Forward_Mod() {
	return ref_import;
}
rserr_t RF_Init( const char *applicationName, const char *screenshotPrefix, int startupColor,int iconResource, const int *iconXPM, void *hinstance, void *wndproc, void *parenthWnd,  bool verbose ) { return ref_import.RF_Init(applicationName, screenshotPrefix, startupColor, iconResource, iconXPM, hinstance, wndproc, parenthWnd, verbose ); }
rserr_t RF_SetMode( int x, int y, int width, int height, int displayFrequency, bool fullScreen, bool stereo ) { return ref_import.RF_SetMode(x,y,width, height, displayFrequency, fullScreen, stereo); }
void RF_AppActivate( bool active, bool destroy ) { ref_import.RF_AppActivate(active, destroy); }
rserr_t	RF_SetWindow( void *hinstance, void *wndproc, void *parenthWnd ) { return ref_import.RF_SetWindow(hinstance, wndproc, parenthWnd); }
void RF_Shutdown( bool verbose ) { ref_import.RF_Shutdown( verbose ); }
void RF_BeginFrame( float cameraSeparation, bool forceClear, bool forceVsync ) { ref_import.RF_BeginFrame( cameraSeparation, forceClear, forceVsync ); }
void RF_EndFrame( void ) { ref_import.RF_EndFrame(); }
void RF_BeginRegistration( void ) { ref_import.RF_BeginRegistration(); }
void RF_EndRegistration( void ) { ref_import.RF_EndRegistration(); }
void RF_RegisterWorldModel( const char *model, const dvis_t *pvsData ) { ref_import.RF_RegisterWorldModel(model,pvsData); }
void RF_ClearScene( void ) { ref_import.RF_ClearScene(); }
void RF_AddEntityToScene( const entity_t *ent ) { ref_import.RF_AddEntityToScene(ent); }
void RF_AddLightToScene( const vec3_t org, float intensity, float r, float g, float b ) { ref_import.RF_AddLightToScene(org, intensity, r, g, b); }
void RF_AddPolyToScene( const poly_t *poly ) { ref_import.RF_AddPolyToScene(poly); }
void RF_AddLightStyleToScene( int style, float r, float g, float b ){ ref_import.RF_AddLightStyleToScene(style, r, g ,b); }
void RF_RenderScene( const refdef_t *fd ) { ref_import.RF_RenderScene(fd);}
void RF_DrawStretchPic( int x, int y, int w, int h, float s1, float t1, float s2, float t2, const vec4_t color, const shader_t *shader ){ ref_import.RF_DrawStretchPic(x, y, w, h, s1, t1, s2,t2, color, shader); }
void RF_DrawRotatedStretchPic( int x, int y, int w, int h, float s1, float t1, float s2, float t2, float angle, const vec4_t color, const shader_t *shader) { ref_import.RF_DrawRotatedStretchPic(x,y, w, h, s1, t1, s2, t2, angle, color, shader); }
void RF_DrawStretchRaw( int x, int y, int w, int h, int cols, int rows, float s1, float t1, float s2, float t2, uint8_t *data ) { ref_import.RF_DrawStretchRaw(x, y, w, h, cols, rows, s1, t1, s2, t2, data ); }
void RF_DrawStretchRawYUV( int x, int y, int w, int h, float s1, float t1, float s2, float t2, ref_img_plane_t *yuv ) { ref_import.RF_DrawStretchRawYUV(x, y, w, h, s1, t1, s2, t2, yuv ); }
void RF_DrawStretchPoly( const poly_t *poly, float x_offset, float y_offset ) { ref_import.RF_DrawStretchPoly(poly, x_offset, y_offset); }
void RF_SetScissor( int x, int y, int w, int h ) { ref_import.RF_SetScissor(x,y,w,h);}
void RF_GetScissor( int *x, int *y, int *w, int *h ) { ref_import.RF_GetScissor(x,y,w,h);}
void RF_ResetScissor( void ){ ref_import.RF_ResetScissor();}
void RF_SetCustomColor( int num, int r, int g, int b ) { ref_import.RF_SetCustomColor(num, r ,g ,b); }
void RF_ScreenShot( const char *path, const char *name, const char *fmtstring, bool silent) { ref_import.RF_ScreenShot(path, name, fmtstring, silent); }
void RF_EnvShot( const char *path, const char *name, unsigned pixels) { ref_import.RF_EnvShot(path, name, pixels); }
bool RF_RenderingEnabled( void ){ return ref_import.RF_RenderingEnabled(); }
const char *RF_GetSpeedsMessage( char *out, size_t size) { return ref_import.RF_GetSpeedsMessage(out, size); }
int RF_GetAverageFramerate( void ) { return ref_import.RF_GetAverageFramerate(); }
void RF_ReplaceRawSubPic( shader_t *shader, int x, int y, int width, int height, uint8_t *data ) { ref_import.RF_ReplaceRawSubPic(shader, x,y,width, height, data);}
void RF_BeginAviDemo( void ){ ref_import.RF_BeginAviDemo();}
void RF_WriteAviFrame( int frame, bool scissor ) { ref_import.RF_WriteAviFrame(frame, scissor); }
void RF_StopAviDemo( void ) { ref_import.RF_StopAviDemo(); } 
void RF_TransformVectorToScreen( const refdef_t *rd, const vec3_t in, vec2_t out ) { ref_import.RF_TransformVectorToScreen(rd, in, out); }
bool RF_LerpTag( orientation_t *orient, const model_t *mod, int oldframe, int frame, float lerpfrac, const char *name) { return ref_import.RF_LerpTag(orient, mod, oldframe, frame, lerpfrac, name); }
void RF_LightForOrigin( const vec3_t origin, vec3_t dir, vec4_t ambient, vec4_t diffuse, float radius) { ref_import.RF_LightForOrigin(origin, dir, ambient, diffuse, radius); }
shader_t *RF_GetShaderForOrigin( const vec3_t origin ) { return ref_import.RF_GetShaderForOrigin(origin);}
struct cinematics_s *RF_GetShaderCinematic( shader_t *shader ) { return ref_import.RF_GetShaderCinematic( shader ); }

static inline void Q_ImportRefModule(const struct ref_import_s* ref) {
	ref_import = *ref;
}
#endif

#endif
