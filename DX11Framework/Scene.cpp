#include "Scene.h"
#include <fstream>
#include <string>

Scene::Scene(SceneManager* SceneMan, RenderingSystem* RenderSys, std::string JsonDataPath) :
    m_SceneManager(SceneMan),
    m_Rendering(RenderSys)
{
    ReadCameraData(JsonDataPath); // Read in Lighting and camera values.
    ReadLightingData(JsonDataPath);
    ReadSceneData(JsonDataPath); // Read in gameobject data
    SetUpScene(); // Scene override to create spcific objects and set references to objects in scene.
}

Scene::~Scene()
{
    m_Rendering = nullptr;
    m_GameObjects.clear();

    EndScene(); // Scene specific override destructor function.
}

void Scene::Render()
{
    ConstantBuffer CbData = m_Rendering->GetCBData();

    //Store this frames data in constant buffer struct
    
    CbData.View = XMMatrixTranspose(XMLoadFloat4x4(m_Cameras[m_activeCameraID]->CreateViewMatrix()));
    CbData.Projection = XMMatrixTranspose(XMLoadFloat4x4(m_Cameras[m_activeCameraID]->CreateProjectionMatrix()));
    CbData.CameraPosition = m_Cameras[m_activeCameraID]->GetEye();

    m_Rendering->SetCBData(CbData);
    //Write constant buffer data onto GPU
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    m_Rendering->GetD3DDevContext()->Map(m_Rendering->GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, &CbData, sizeof(CbData));
    m_Rendering->GetD3DDevContext()->Unmap(m_Rendering->GetConstantBuffer(), 0);

    // Iterate all gameobjects and call their render call.
    for (auto& GO : m_GameObjects)
    {
        GO.second->Render();
    }
}

void Scene::RenderSkyBox()
{
    ConstantBuffer CbData;

    // Calculate world position
    XMFLOAT4X4 CameraWorld;
    XMStoreFloat4x4(&CameraWorld, XMMatrixTranslation(m_Cameras[m_activeCameraID]->GetEye().x, 
                                                      m_Cameras[m_activeCameraID]->GetEye().y, 
                                                      m_Cameras[m_activeCameraID]->GetEye().z));

    //Store this Objects data in constant buffer struct
    CbData.World = XMMatrixTranspose(XMLoadFloat4x4(&CameraWorld));
    CbData.View = XMMatrixTranspose(XMLoadFloat4x4(m_Cameras[m_activeCameraID]->CreateViewMatrix()));
    CbData.Projection = XMMatrixTranspose(XMLoadFloat4x4(m_Cameras[m_activeCameraID]->CreateProjectionMatrix()));

    //Write constant buffer data onto GPU
    D3D11_MAPPED_SUBRESOURCE mappedSubresource;
    m_Rendering->GetD3DDevContext()->Map(m_Rendering->GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
    memcpy(mappedSubresource.pData, &CbData, sizeof(CbData));
    m_Rendering->GetD3DDevContext()->Unmap(m_Rendering->GetConstantBuffer(), 0);

    ObjectDrawDESC* Object = new ObjectDrawDESC();
    Object->Mesh = m_Rendering->GetMeshHandler()->MeshLookUp("3DSModels/SM_Cube.obj");
    Object->ColourMap = m_Rendering->GetTextureHandler()->TextureLookUp("Textures/skybox.dds");

    m_Rendering->AddSkyboxObjectToDrawQueue(Object);
}

void Scene::Update(float DeltaTime)
{
    // Iterate and update all gameobjects.
    for (auto& GO : m_GameObjects)
    {
        GO.second->Update();
    }

    // Changes active camera
    if (GetAsyncKeyState(VK_LEFT) & 0x0001)
    {
        if (m_activeCameraID == 0)
        {
            m_activeCameraID = m_Cameras.size() - 1;
        }

        else
        {
            m_activeCameraID--;
        }

        ConstantBuffer CbData = m_Rendering->GetCBData();

        // Set the camera position in the constant buffer.
        CbData.CameraPosition = m_Cameras[m_activeCameraID]->GetEye();

        m_Rendering->SetCBData(CbData);
    }

    else if (GetAsyncKeyState(VK_RIGHT) & 0x0001)
    {
        if (m_activeCameraID == m_Cameras.size() - 1)
        {
            m_activeCameraID = 0;
        }
        else
        {
            m_activeCameraID++;
        }
        ConstantBuffer CbData = m_Rendering->GetCBData();

        // Set the camera position in the constant buffer.
        CbData.CameraPosition = m_Cameras[m_activeCameraID]->GetEye();

        m_Rendering->SetCBData(CbData);
    }

    // Update Cameras
    m_Cameras[m_activeCameraID]->Update(DeltaTime);
    std::string Text = "Current Camera: ";

    if (m_Cameras[m_activeCameraID]->GetType() == CT_Debug)
        Text += "Debug";

    else if (m_Cameras[m_activeCameraID]->GetType() == CT_Static)
        Text += "Static";

    std::wstring TempWstring = std::wstring(Text.begin(), Text.end());
    m_ActiveCameraText->SetText(TempWstring.c_str());

    // Update scene - Scene Specific update cycle.
    UpdateScene(DeltaTime);
}

GameObject* Scene::GetGameObject(UINT ID)
{
    auto it = m_GameObjects.find(ID);

    if (it != m_GameObjects.end())
        return it->second.get();

    else
        return nullptr;

    //if (m_GameObjects.count(ID) == 1) // Check that the Gameobject ID actually exists in our map.
    //    return m_GameObjects[ID].get(); // return the raw pointer.
    //else
    //    return nullptr; // ID does not exist in map, return nullptr.
}

void Scene::RemoveGameObject(UINT GameObjectID)
{
    std::map<UINT, std::unique_ptr<GameObject>>::iterator it; // Create a iterator for our gameobject map.
    it = m_GameObjects.find(GameObjectID); // Find the iterator for the ID Specified
    m_GameObjects.erase(it); // Erase the gameobject from our map using the iterator.
}

void Scene::ReadCameraData(std::string SceneInfoPath)
{
    ConstantBuffer CbData = m_Rendering->GetCBData();

    // Parse Json File
    json JFile;
    std::ifstream fileOpen("ConfigFiles/Scene_Main.json");
    JFile = json::parse(fileOpen);
    std::string v = JFile["version"].get<std::string>();

    // Load in camera values
    json& Cameras = JFile["Cameras"];
    int CamCount = Cameras.size();

    for (int i = 0; i < CamCount; i++)
    {
        json& CameraDesc = Cameras.at(i);
        //Camera
        if (CameraDesc["Type"] == "Static")
        {
            XMFLOAT3 Eye = XMFLOAT3(CameraDesc["EyePos"][0], CameraDesc["EyePos"][1], CameraDesc["EyePos"][2]);
            XMFLOAT3 At = XMFLOAT3(CameraDesc["LookAt"][0], CameraDesc["LookAt"][1], CameraDesc["LookAt"][2]);
            XMFLOAT3 Up = XMFLOAT3(CameraDesc["UpVector"][0], CameraDesc["UpVector"][1], CameraDesc["UpVector"][2]);
            
            // Create a camera using the specified values above.
            m_Cameras.push_back(std::make_unique<StaticCamera>(Eye, At, Up, m_Rendering->GetScreenInfo().WindowWidth, m_Rendering->GetScreenInfo().WindowHeight, 0.01f, 100.0f));
        }

        if (CameraDesc["Type"] == "Debug")
        {
            XMFLOAT3 Eye = XMFLOAT3(CameraDesc["EyePos"][0], CameraDesc["EyePos"][1], CameraDesc["EyePos"][2]);
            XMFLOAT3 To = XMFLOAT3(CameraDesc["LookTo"][0], CameraDesc["LookTo"][1], CameraDesc["LookTo"][2]);
            XMFLOAT3 Up = XMFLOAT3(CameraDesc["UpVector"][0], CameraDesc["UpVector"][1], CameraDesc["UpVector"][2]);

            // Create a camera using the specified values above.
            m_Cameras.push_back(std::make_unique<DebugCamera>(Eye, To, Up, m_Rendering->GetScreenInfo().WindowWidth, m_Rendering->GetScreenInfo().WindowHeight, 0.01f, 100.0f));
        }
    }

    // Set the camera position in the constant buffer.
    CbData.CameraPosition = m_Cameras[m_activeCameraID]->GetEye();

    m_Rendering->SetCBData(CbData);
    // close the json file
    fileOpen.close();
}

void Scene::ReadLightingData(std::string SceneInfoPath)
{
    // Load in Lighting Values
    ConstantBuffer CbData = m_Rendering->GetCBData();

    // Parse Json File
    json JFile;
    std::ifstream fileOpen("ConfigFiles/Scene_Main.json");
    JFile = json::parse(fileOpen);
    std::string v = JFile["version"].get<std::string>();

    json& Lights = JFile["Lights"]; // Gets the array of lights
    int LightsCount = Lights.size();

    for (int i = 0; i < LightsCount; i++)
    {
        json& LightDesc = Lights.at(i);
        if (LightDesc["Type"] == "Directional")
        {
            DirectionalLight DLightStruct;

            DLightStruct.LightDir = XMFLOAT3(LightDesc["LightDir"][0], LightDesc["LightDir"][1], LightDesc["LightDir"][2]);
            DLightStruct.AmbientLight = XMFLOAT4(LightDesc["AmbientLight"][0], LightDesc["AmbientLight"][1], LightDesc["AmbientLight"][2], LightDesc["AmbientLight"][3]);
            DLightStruct.DiffuseLight = XMFLOAT4(LightDesc["DiffuseLight"][0], LightDesc["DiffuseLight"][1], LightDesc["DiffuseLight"][2], LightDesc["DiffuseLight"][3]);
            DLightStruct.SpecularLight = XMFLOAT4(LightDesc["SpecularLight"][0], LightDesc["SpecularLight"][1], LightDesc["SpecularLight"][2], LightDesc["SpecularLight"][3]);
            DLightStruct.SpecularPower = LightDesc["SpecularPower"];

            CbData.DLights[CbData.DLightCount] = DLightStruct;
            CbData.DLightCount++;
        }
        else if (LightDesc["Type"] == "Point")
        {
            PointLight PLightStruct;

            PLightStruct.Ambient = XMFLOAT4(LightDesc["Ambient"][0], LightDesc["Ambient"][1], LightDesc["Ambient"][2], LightDesc["Ambient"][3]);
            PLightStruct.Diffuse = XMFLOAT4(LightDesc["Diffuse"][0], LightDesc["Diffuse"][1], LightDesc["Diffuse"][2], LightDesc["Diffuse"][3]);
            PLightStruct.Attenuation = XMFLOAT3(LightDesc["Attenuation"][0], LightDesc["Attenuation"][1], LightDesc["Attenuation"][2]);
            PLightStruct.LightRange = LightDesc["Range"];
            PLightStruct.Position = XMFLOAT3(LightDesc["Position"][0], LightDesc["Position"][1], LightDesc["Position"][2]);

            CbData.PLights[CbData.PLightCount] = PLightStruct;
            CbData.PLightCount++;
        }
    }

    m_Rendering->SetCBData(CbData);
}

void Scene::ReadSceneData(std::string SceneInfoPath)
{
    // Load in Scene Data
    json JFile;
    std::ifstream fileOpen(SceneInfoPath);
    JFile = json::parse(fileOpen);
    std::string v = JFile["version"].get<std::string>();

    json& GameObjects = JFile["GameObjects"]; // Gets the array of GameObjects
    int Size = GameObjects.size();

    // iterate over all objects inside gameobject array
    for (unsigned int i = 0; i < Size; i++)
    {
        json& ObjectDesc = GameObjects.at(i); // create a object description for this object - this has all information about this gameobject.
        GameObject* TempObject = CreateGameObject(ObjectDesc["ID"]); // Create temp Object to add information into based on json file.
        TempObject->SetID(ObjectDesc["ID"]); // Read in Object ID, Important later when we need to set member references for objects modified at runtime!

        // Decrypt what Tag was Applied In JSON.
        if (ObjectDesc["Tag"] == "Player")
            TempObject->SetTag(GOTAG_Player);

        else if (ObjectDesc["Tag"] == "Enemy")
            TempObject->SetTag(GOTAG_Enemy);

        else if (ObjectDesc["Tag"] == "Environment")
            TempObject->SetTag(GOTAG_Environment);

        else if (ObjectDesc["Tag"] == "GUI")
            TempObject->SetTag(GOTAG_GUI);

        // Set Position, Rotation, and Scale for the Object.
        TempObject->SetPosition(XMFLOAT3(ObjectDesc["Position"][0], ObjectDesc["Position"][1], ObjectDesc["Position"][2]));
        TempObject->SetRotation(XMFLOAT3(ObjectDesc["Rotation"][0], ObjectDesc["Rotation"][1], ObjectDesc["Rotation"][2]));
        TempObject->SetScale(XMFLOAT3(ObjectDesc["Scale"][0], ObjectDesc["Scale"][1], ObjectDesc["Scale"][2]));

        // Decrypt What components we need to create, every component has different stats.
        json& Components = ObjectDesc["Components"]; // Gets the array of components inside my object im currently looking at
        int componentsSize = Components.size();

        // Iterate through all components that we want to create on this gameobject.
        for (unsigned int i = 0; i < componentsSize; i++)
        {
            // Create a component description 
            json& CompDesc = Components.at(i);
            if (CompDesc["COMPTYPE"] == "StaticMesh")
            {
                // Load in Static Mesh Data
                StaticMeshComp* TempSM = CreateStaticMeshComponent(TempObject, m_Rendering->GetMeshHandler()->MeshLookUp(CompDesc["MeshDataPath"]));
                
                if (CompDesc["ColourTexturePath"] != "") // User Must have Specified a ColourMap path.
                    TempSM->SetColourTexture(m_Rendering->GetTextureHandler()->TextureLookUp(CompDesc["ColourTexturePath"]));

                if (CompDesc["SpecularTexturePath"] != "") // user must have specified a SpecularMap path.
                    TempSM->SetSpecularTexture(m_Rendering->GetTextureHandler()->TextureLookUp(CompDesc["SpecularTexturePath"]));

                if (CompDesc["HasTransparency"] == 1)
                {
                    float x = CompDesc["Transparency"][0];
                    float y = CompDesc["Transparency"][1];
                    float z = CompDesc["Transparency"][2];
                    float w = CompDesc["Transparency"][3];
                    XMVECTOR TransVal = {x, y, z, w};
                    TempSM->SetTransparencyActive(TransVal);
                }
            }
            else if (CompDesc["COMPTYPE"] == "SpriteRenderer")
            {
                CreateSpriteRendererComponent(TempObject, 
                                              m_Rendering->GetTextureHandler()->TextureLookUp(CompDesc["SpritePath"]),
                                              XMFLOAT2(CompDesc["SpriteSize"][0], CompDesc["SpriteSize"][1]));
            }
            else if (CompDesc["COMPTYPE"] == "TextRenderer")
            {
                int R = CompDesc["TextColour"][0];
                int G = CompDesc["TextColour"][1];
                int B = CompDesc["TextColour"][2];
                int A = CompDesc["TextColour"][3];
                XMVECTOR Color = { R, G, B, A };

                std::string TempValue = CompDesc["TextValue"];
                std::wstring FinalText = std::wstring(TempValue.begin(), TempValue.end());

                CreateTextRendererComponent(TempObject,
                                            FinalText.c_str(),
                                            Color);
            }
        }
    }

    // Close the json file.
    fileOpen.close();

    // Grab non-specific scene objects (should be in every scene)
    m_StaffsUniLogo = GetGameObject(1);

    m_ActiveCameraTextGO = GetGameObject(0);
    m_ActiveCameraText = (TextRenderer*)m_ActiveCameraTextGO->GetComponent(COMPTYPE_GUIText);
}

GameObject* Scene::CreateGameObject()
{
    std::unique_ptr<GameObject> TempObject = std::make_unique<GameObject>(this, GOTAG_Enemy, m_Rendering); // Make a new unique gameobject pointer
    UINT ID = TempObject->GetID(); // Set Gameobject ID for map key.
    m_GameObjects[ID] = std::move(TempObject); // add the new object into the gameobject map, ensuring to move the unique owner to the map, the map now owns teh gameobject.
    return m_GameObjects[ID].get(); // return the raw pointer to the gameobject we just created.
}

GameObject* Scene::CreateGameObject(UINT ID)
{
    std::unique_ptr<GameObject> TempObject = std::make_unique<GameObject>(this, GOTAG_Enemy, m_Rendering); // Make a new unique gameobject pointer
    m_GameObjects[ID] = std::move(TempObject); // add the new object into the gameobject map, ensuring to move the unique owner to the map, the map now owns teh gameobject.
    return m_GameObjects[ID].get(); // return the raw pointer to the gameobject we just created.
}

StaticMeshComp* Scene::CreateStaticMeshComponent(GameObject* OwningGO, MeshData* InMesh)
{
    // Create a new unique Static Mesh Component, using unique pointers to better control ownership of objects.
    std::unique_ptr<StaticMeshComp> TempSM = std::make_unique<StaticMeshComp>(OwningGO, InMesh, m_Rendering);
    StaticMeshComp* Temp = TempSM.get(); // Store a temporary value of the static mesh raw pointer.
    OwningGO->AddComponent(std::move(TempSM)); // Add the static mesh component to the gameobject, ensuring to move ownership to the gameobject.
    return Temp; // return a raw pointer to the static mesh component.
}

SpriteRenderer* Scene::CreateSpriteRendererComponent(GameObject* OwningGO, TextureData* Sprite, XMFLOAT2 Size)
{
    std::unique_ptr<SpriteRenderer> TempSR = std::make_unique<SpriteRenderer>(OwningGO, Sprite, Size, m_Rendering);
    SpriteRenderer* Temp = TempSR.get();
    OwningGO->AddComponent(std::move(TempSR));
    return Temp;
}

TextRenderer* Scene::CreateTextRendererComponent(GameObject* OwningGO, std::wstring Text, XMVECTOR Colour)
{
    std::unique_ptr<TextRenderer> TempTR = std::make_unique<TextRenderer>(OwningGO, m_Rendering, Text, Colour, XMFLOAT2(OwningGO->GetScale().x, OwningGO->GetScale().y));
    TextRenderer* TempRawTR = TempTR.get();
    OwningGO->AddComponent(std::move(TempTR));
    return TempRawTR;
}
