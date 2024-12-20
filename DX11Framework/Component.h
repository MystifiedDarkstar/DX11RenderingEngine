#pragma once

#include "ComponentTypes.h"
#include "EngineStructures.h"
#include "GameObject.h"

#include <vector>

class Component
{
public:
	Component(GameObject* OwningGO, int UpdateOrder = 100, ComponentTypes type = COMPTYPE_None); // Default Constructor, Update order is 100 unless Specified otherwise.
	virtual ~Component() {} // Destructor, Should be overriddon by the inherited Component Type.
	
	virtual void Update() {} // Update Function, Should be overiddon by inherited Component Type.
	virtual void Render() {}

	int GetUpdateOrder() const { return m_UpdateOrder; } // Return the Update Order. Will determine when its updated compared to other components.
	ComponentTypes GetCompType() { return m_CompType; }
	GameObject* GetParentObject();

private:
	ComponentTypes m_CompType;
	int m_UpdateOrder;

protected:
	GameObject* m_ParentObject;
};

int Partition(std::vector<std::unique_ptr<Component>>& array, int start, int end);

namespace Components
{
	void SortComponents(std::vector<std::unique_ptr<Component>>& array, int start, int end);
}

