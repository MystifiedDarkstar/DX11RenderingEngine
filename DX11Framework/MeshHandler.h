#pragma once

#include "EngineStructures.h"
#include "OBJLoader.h"
#include <iostream>
#include <map>

class MeshHandler
{
public:
	MeshHandler() = default;
	MeshHandler(ID3D11Device* Device);
	~MeshHandler();

	MeshData* MeshLookUp(std::string MeshPath);
	void DecrementMeshUser(std::string MeshPath);

private:
	void LoadMeshFromPath(std::string MeshPath);
	void UnloadMeshFromPath(std::string MeshPath);

private:
	std::map<std::string, std::unique_ptr<MeshData>> m_Meshes;
	ID3D11Device* m_Device;
};

