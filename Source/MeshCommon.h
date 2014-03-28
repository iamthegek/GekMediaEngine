#ifndef __MESH_COMMON__H
#define __MESH_COMMON__H

#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
#define SAFE_DELETE(p) if (p) { delete p; p = NULL; }
#define SNPRINTF _snprintf_s

const int INDEX_BUFFER = 0; 
const int POS_VB = 1;
const int NORMAL_VB = 2;	
const int TEXCOORD_VB = 3; 
const int TANGENT_VB = 4;
const int BONE_VB = 5;

const int NUM_STAT_MESH_VBs = 5;
const int NUM_BONE_MESH_VBs = 6;

#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define TANGENT_LOCATION	 3
#define BONE_ID_LOCATION     4
#define BONE_WEIGHT_LOCATION 5

#define NUM_BONES_PER_VEREX  4
const int MAX_BONES = 64;

#endif