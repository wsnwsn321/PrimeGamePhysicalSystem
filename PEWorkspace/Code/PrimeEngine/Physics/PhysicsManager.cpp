#include "PrimeEngine/Lua/LuaEnvironment.h"
#include "PrimeEngine/Events/StandardEvents.h"
#include "PrimeEngine/Physics/PhysicsComponent.h"
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "PhysicsManager.h"


namespace PE {
	namespace Components {

		PE_IMPLEMENT_CLASS1(PhysicsManager, Component);

		//PhysicsComponent *PhysicsComponent::PhysicsComponent = 0;

		// Constructor -------------------------------------------------------------
		PhysicsManager::PhysicsManager(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : Component(context, arena, hMyself), m_phy_comp(context, arena, 128)
		{

		}

		void PhysicsManager::addDefaultComponents()
		{
			Component::addDefaultComponents();
			
		}
	}
}