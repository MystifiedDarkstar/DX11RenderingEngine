#include "Component.h"

Component::Component(GameObject* OwningGO, int UpdateOrder, ComponentTypes type) :
    m_ParentObject(OwningGO),
    m_UpdateOrder(UpdateOrder),
    m_CompType(type)
{

}

GameObject* Component::GetParentObject()
{
    return m_ParentObject;
}

namespace Components
{
    // This is used to sort components in order of their update order, this is useful so we can update pyhsics before rendering.
    void SortComponents(std::vector<std::unique_ptr<Component>>& array, int start, int end)
    {
        if (start < end)
        {
            int pivotIndex = Partition(array, start, end);
            SortComponents(array, start, pivotIndex - 1);
            SortComponents(array, pivotIndex + 1, end);
        }
    }
}
int Partition(std::vector<std::unique_ptr<Component>>& array, int start, int end)
{
    int pivotIndex = end;

    int low = start - 1;

    for (int i = start; i <= end; i++)
    {
        if (array[i]->GetUpdateOrder() < array[pivotIndex]->GetUpdateOrder())
        {
            low++;
            std::swap(array[low], array[i]);
        }
    }
    std::swap(array[low + 1], array[end]);
    return low + 1;
}
