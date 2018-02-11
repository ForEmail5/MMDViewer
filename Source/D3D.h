#ifndef D3D_H
#define D3D_H

#include <D3D11.h>
#include <D3DX11.h>
#include <cassert>

#define SAFE_RELEASE(obj) if(obj) obj->Release()

#define mmd_offsetof(s,m) (&(((s*)0)->m))

#endif