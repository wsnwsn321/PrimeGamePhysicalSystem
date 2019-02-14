#pragma once
#ifndef __PYENGINE_2_0_BOX_COLLIDER_
#define __PYENGINE_2_0_BOX_COLLIDER_
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "PhysicsComponent.h"

//#define USE_DRAW_COMPONENT

namespace PE {
	namespace Components
	{
		// This struct will be added to GameObjectManager as component
		// as a result events sent to game object manager will be able to get to this component
		// so we can create custom game objects through this class
		struct BoxCollider : public PhysicsComponent
		{
			PE_DECLARE_CLASS(BoxCollider); // creates a static handle and GteInstance*() methods. still need to create construct

			BoxCollider(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) : PhysicsComponent(context, arena, hMyself)
			{}

			// sub-component and event registration
			virtual void addDefaultComponents();

		};
	}
}

#endif