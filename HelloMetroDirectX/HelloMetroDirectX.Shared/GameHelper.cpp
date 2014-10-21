#include "pch.h"
#include "GameHelper.h"
#include <fstream>
Array<byte>^ LoadShaderFile(std::string file_path)
{
	Array<byte>^ data = nullptr;
	std::ifstream file(file_path, std::ios::in | std::ios::binary | std::ios::ate);
	if (file.is_open())
	{
		int Length = (int)file.tellg();
		data = ref new Array<byte>(Length);
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char*>(data->Data), Length);
		file.close();
	}
	return data;
}
float random(float x, float y)
{
	float high, low, ret;
	if (x == y) return x;
	high = max(x, y);
	low = min(x, y);
	ret = low + ((high - low)*rand() / (RAND_MAX + 1.0f));
	return ret;
}