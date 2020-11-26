#pragma once
#include <string>
#include "Mesh.h"

class HeightMap : public Mesh
{
public:
	HeightMap(const std::string& name);
	~HeightMap() {};

	Vector3 GetHeightMapSize() const { return heightMapSize; }
protected:
	Vector3 heightMapSize;
};