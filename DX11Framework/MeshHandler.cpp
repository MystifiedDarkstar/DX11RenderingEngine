#include "MeshHandler.h"

MeshHandler::MeshHandler(ID3D11Device* Device) :
	m_Device(Device)
{

}

MeshHandler::~MeshHandler()
{
	m_Device = nullptr;
    m_Meshes.clear(); // deletes all Meshes and frees memory.
}

MeshData* MeshHandler::MeshLookUp(std::string MeshPath)
{
	if (m_Meshes.count(MeshPath) == 0) // No Mesh exists for this key, attempt to load it
	{
		// Load Mesh @ Path Specified.
		LoadMeshFromPath(MeshPath);

		// Return the new Mesh
		m_Meshes[MeshPath]->Count++;
		return m_Meshes[MeshPath].get();
	}

	else if (m_Meshes.count(MeshPath) > 0) // Mesh Must Exist at this key, Lets return it.
	{
		m_Meshes[MeshPath]->Count++;
		return m_Meshes[MeshPath].get();
	}
}

void MeshHandler::DecrementMeshUser(std::string MeshPath)
{
	if (m_Meshes.count(MeshPath) == 0) return; // No Mesh exists at this path

	m_Meshes[MeshPath]->Count--; // Decrement the current users for this mesh.
	 
	if (m_Meshes[MeshPath]->Count == 0) // Check if the number of users equals 0.
		UnloadMeshFromPath(MeshPath); // users = 0, unload the mesh from memory.
}

void MeshHandler::LoadMeshFromPath(std::string MeshPath)
{
	std::unique_ptr<MeshData> TempMesh; // Create a temporary mesh.
	TempMesh = std::make_unique<MeshData>(OBJLoader::Load(MeshPath.c_str(), m_Device)); // Make a nwe unique MeshData with the Mesh information, buffers etc.
	TempMesh->ID = MeshPath; // Set the ID of the mesh equal to its meshpath
	m_Meshes[MeshPath] = std::move(TempMesh); // move the unique owner into the map.
}

void MeshHandler::UnloadMeshFromPath(std::string MeshPath)
{
	std::map<std::string, std::unique_ptr<MeshData>>::iterator it; // Create a new iterator for the mesh map.
	it = m_Meshes.find(MeshPath); // Find the iterator for our Mesh Key.
	m_Meshes.erase(it); // Erase the mesh Data from the map and memory.
}
