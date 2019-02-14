#pragma once
#ifndef __PYENGINE_2_0_PHYSICS_COMPONENT_H__
#define __PYENGINE_2_0_PHYSICS_COMPONENT_H__
// API Abstraction
#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

// Outer-Engine includes
#include <assert.h>

// Inter-Engine includes
#include "PrimeEngine/MemoryManagement/Handle.h"
#include "PrimeEngine/PrimitiveTypes/PrimitiveTypes.h"
#include "../Events/Component.h"
#include "PhysicsComponent.h"
#include <vector>
//#define USE_DRAW_COMPONENT

namespace PE {
		namespace Components
		{
			// This struct will be added to GameObjectManager as component
			// as a result events sent to game object manager will be able to get to this component
			// so we can create custom game objects through this class
			struct PhysicsComponent : public Component
			{
				PE_DECLARE_CLASS(PhysicsComponent); // creates a static handle and GteInstance*() methods. still need to create construct

				PhysicsComponent(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself);
				virtual ~PhysicsComponent() {}

				// sub-component and event registration
				virtual void addDefaultComponents();

				//////////////////////////////////////////////////////////////////////////
				// Game Specific functionality
				//////////////////////////////////////////////////////////////////////////
				//
				PE::Handle m_hPC;

				int m_type;
				Vector3 m_sphere_center;
				float radius;
				Matrix4x4 m_pc_base;
				std::vector<PrimitiveTypes::Float32> m_vertexArray;
				std::vector<Vector3> m_meshPoints;
				char* objectName;
					
			};
	}
}

#endif