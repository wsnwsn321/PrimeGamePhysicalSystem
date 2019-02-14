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
#include "PhysicsComponent.h"

namespace PE {
	namespace Components {

		PE_IMPLEMENT_CLASS1(PhysicsComponent, Component);


		// Constructor -------------------------------------------------------------
		PhysicsComponent::PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : Component(context, arena, hMyself)
		{
			
		}

		void PhysicsComponent::addDefaultComponents()
		{
			Component::addDefaultComponents();
		}
	}
}