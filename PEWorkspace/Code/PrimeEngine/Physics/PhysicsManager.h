#pragma once
#ifndef __PYENGINE_2_0_PHYSICSMANAGERE_H__
#define __PYENGINE_2_0_PHYSICSMANAGERE_H__

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
	namespace Components {
		struct PhysicsManager : public Component
		{
			PE_DECLARE_CLASS(PhysicsManager);

			// Constructor -------------------------------------------------------------
			PhysicsManager(PE::GameContext &context, PE::MemoryArena arena, Handle hMyself);
			virtual ~PhysicsManager() {}

			// Component ------------------------------------------------------------

			virtual void addDefaultComponents();


			// Individual events -------------------------------------------------------
			Array<PhysicsComponent*,1> m_phy_comp;
	
		}; // class SceneNode

	}; // namespace Components
}; // namespace PE
#endif
