#pragma once
#include "pch.h"
#include "WICTextureLoader.h"
using namespace Platform;

#ifndef PI
#define PI 3.14
#endif

Array<byte>^ LoadShaderFile(std::string file_path);
float random(float x, float y);