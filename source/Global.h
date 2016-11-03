#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <stdlib.h>
#include <sys/types.h>

#define OT_LENGTH (256)

#define SCREEN_WIDTH (320)
#define SCREEN_HEIGHT (240)

#define SCREEN_CENTER_X (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT / 2)

typedef struct OrderingTable OrderingTable;
typedef struct FrameBuffer FrameBuffer;
typedef struct Camera Camera;

typedef struct Mesh Mesh;
typedef struct MeshAttr MeshAttr;
typedef struct MeshTriGour MeshTriGour;
typedef struct MeshTriGourTex MeshTriGourTex;

#endif
