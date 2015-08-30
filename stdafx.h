// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define NOMINMAX
#define _CRT_SECURE_NO_WARNINGS

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#define _USE_MATH_DEFINES
#include <cmath>

#include <stdlib.h>
#include <tchar.h>
#include <assert.h>

// C++
#include <algorithm>
#include <map>
#include <vector>
#include <deque>
#include <chrono>
#include <random>

// DirectX
#include <d3d11.h>
#include <D3Dcompiler.h>
#include <wrl.h>	// ComPtr
//#include <SimpleMath.h>
//#include <DDSTextureLoader.h>
//#include <WICTextureLoader.h>
//#include <SpriteFont.h>

//using namespace DirectX;
//using namespace SimpleMath;
using Microsoft::WRL::ComPtr;

static const int BONE_MAX = 70;

// TODO: reference additional headers your program requires here
#include "source/af_math.h"
#include "source/system_metrics.h"
#include "source/dib.h"
#include "source/device_man11.h"
#include "source/buffer_man11.h"
#include "source/tex_man11.h"
#include "source/helper_text.h"
#include "source/helper_dx.h"
#include "source/helper.h"
#include "source/shader_man11.h"
#include "source/font_man.h"
#include "source/sky_man11.h"
#include "source/mat_man.h"
#include "source/matrix_man.h"
#include "source/mesh_renderer11.h"
#include "source/joint_db.h"
#include "source/mesh_x.h"
#include "source/bvh.h"
#include "source/debug_renderer.h"
#include "source/grid_renderer.h"
#include "source/water_surface.h"
#include "source/fps.h"
#include "source/post_effect_man.h"
#include "source/compute_shader_man.h"
#include "source/compute_shader_skinning.h"

#include "source/dev_camera.h"
#include "source/app.h"


#define SCR_W 1024
#define SCR_H 600
