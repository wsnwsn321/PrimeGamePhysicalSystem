#include "PrimeEngine/APIAbstraction/APIAbstractionDefines.h"

#include "PrimeEngine/Lua/LuaEnvironment.h"

#include "SoldierNPCMovementSM.h"
#include "SoldierNPCAnimationSM.h"
#include "SoldierNPC.h"
#include "Util.h"
#include <vector>
#include "PrimeEngine/Scene/DebugRenderer.h"
//Area for physics system

bool launched = false;
float m;
float mass;                          // mass
Matrix4x4 I_body;                    // body inertia

float linear_damping;                // for damping
float angular_damping;
float restitution;                   // for collision
Quaternion q;
//end of physics area
extern std::vector<PE::Components::PhysicsComponent*> StaticObject;
using namespace PE::Components;
using namespace PE::Events;
using namespace CharacterControl::Events;

namespace CharacterControl{

// Events sent by behavior state machine (or other high level state machines)
// these are events that specify where a soldier should move
namespace Events{

PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM_Event_MOVE_TO, Event);

SoldierNPCMovementSM_Event_MOVE_TO::SoldierNPCMovementSM_Event_MOVE_TO(Vector3 targetPos /* = Vector3 */)
: m_targetPosition(targetPos)
{ }

PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM_Event_STOP, Event);

PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM_Event_TARGET_REACHED, Event);
}

namespace Components{


PE_IMPLEMENT_CLASS1(SoldierNPCMovementSM, Component);


SoldierNPCMovementSM::SoldierNPCMovementSM(PE::GameContext &context, PE::MemoryArena arena, PE::Handle hMyself) 
: Component(context, arena, hMyself)
, m_state(STANDING)
{}

SceneNode *SoldierNPCMovementSM::getParentsSceneNode()
{
	PE::Handle hParent = getFirstParentByType<Component>();
	if (hParent.isValid())
	{
		// see if parent has scene node component
		return hParent.getObject<Component>()->getFirstComponent<SceneNode>();
		
	}
	return NULL;
}

PhysicsComponent *SoldierNPCMovementSM::getParentsPhysicalComponent()
{
	PE::Handle hParent = getFirstParentByType<Component>();
	if (hParent.isValid())
	{
		// see if parent has scene node component
		return hParent.getObject<Component>()->getFirstComponent<PhysicsComponent>();

	}
	return NULL;
}


void SoldierNPCMovementSM::addDefaultComponents()
{
	Component::addDefaultComponents();

	PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_MOVE_TO, SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_MOVE_TO);
	PE_REGISTER_EVENT_HANDLER(SoldierNPCMovementSM_Event_STOP, SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_STOP);
	
	PE_REGISTER_EVENT_HANDLER(Event_UPDATE, SoldierNPCMovementSM::do_UPDATE);
}

void SoldierNPCMovementSM::start(std::vector<Vector3> meshPoints)
{
	//start ()
		//Initialize coefficients
		w = Vector3(0, 0.0f, 0);
		linear_damping = 0.999f;
		angular_damping = 0.98f;
		restitution = 0.001f;     //elastic collision
		m = 1;
		mass = 1;
		PhysicsComponent *pPC = getParentsPhysicalComponent();
		std::vector<Vector3> vertices = meshPoints;//
		for (int i = 0; i < vertices.size(); i++)
		{
			mass += m;
			float diag = m * vertices[i].lengthSqr();
			I_body.m[0][0] += diag;
			I_body.m[1][1] += diag;
			I_body.m[2][2] += diag;
			I_body.m[0][0] -= m * vertices[i].getX() * vertices[i].getX();
			I_body.m[0][1] -= m * vertices[i].getX() * vertices[i].getY();
			I_body.m[0][2] -= m * vertices[i].getX() * vertices[i].getZ();
			I_body.m[1][0] -= m * vertices[i].getY() * vertices[i].getX();
			I_body.m[1][1] -= m * vertices[i].getY() * vertices[i].getY();
			I_body.m[1][2] -= m * vertices[i].getY() * vertices[i].getZ();
			I_body.m[2][0] -= m * vertices[i].getZ() * vertices[i].getX();
			I_body.m[2][1] -= m * vertices[i].getZ() * vertices[i].getY();
			I_body.m[2][2] -= m * vertices[i].getZ() * vertices[i].getZ();
		}
		I_body.m[3][3] = 1;
		
		if (strcmp(meshName, "nazicar.x_carmesh_mesh.mesha")==0) {
			v = Vector3(21, 3, 0);
		}
		else if (strcmp(meshName, "SoldierTransform.mesha")==0) {
			v = Vector3(0, 4, 8);
		}
	
}
void SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_MOVE_TO(PE::Events::Event *pEvt)
{
	SoldierNPCMovementSM_Event_MOVE_TO *pRealEvt = (SoldierNPCMovementSM_Event_MOVE_TO *)(pEvt);
	
	// change state of this state machine
	m_state = WALKING_TO_TARGET;
	m_targetPostion = pRealEvt->m_targetPosition;

	// make sure the animations are playing
	
	PE::Handle h("SoldierNPCAnimSM_Event_WALK", sizeof(SoldierNPCAnimSM_Event_WALK));
	Events::SoldierNPCAnimSM_Event_WALK *pOutEvt = new(h) SoldierNPCAnimSM_Event_WALK();
	
	SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
	pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(pOutEvt);
	// release memory now that event is processed
	h.release();
}

void SoldierNPCMovementSM::do_SoldierNPCMovementSM_Event_STOP(PE::Events::Event *pEvt)
{
	Events::SoldierNPCAnimSM_Event_STOP Evt;

	SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
	pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(&Evt);
}

void SoldierNPCMovementSM::do_UPDATE(PE::Events::Event *pEvt)
{
	//a7 physics update
	SceneNode *pSN = getParentsSceneNode();
	// Use this as your time step
	float dt = 0.02f;
	Vector3 pos = pSN->m_base.getPos();
	Vector3 tu = pSN->m_base.getU();
	Vector3 tv = pSN->m_base.getV();
	Vector3 tn = pSN->m_base.getN();
	Vector3 tempQ = pSN->m_base.getU() + pSN->m_base.getV() + pSN->m_base.getN();
	tempQ.normalize();

	q = Matrix_To_Quaternion(pSN->m_base);
	/*if (Input.GetKey(KeyCode.R) && launched)
	{
		pos = new Vector3(0, 0.6f, 0);
		restitution = 0.01f;
		launched = false;
		v = new Vector3(0, 0, 0);
		w = new Vector3(0, 10, 0);
	}
	
	if (Input.GetKey(KeyCode.I) && !launched)
	{
		launched = true;
		v = new Vector3(5, 3, 0);
	}
	*/
	launched = true;
	//v = Vector3(0, 0.3, 0.5);

	// Part I: Update velocities
	if (launched)
	{
		v.m_y = v.m_y - 0.98f * dt;
	}
	//damping
	//w = w * angular_damping;
	v = v * linear_damping;

	// Part II: Collision Handler
	//Collision_Handler();
	if(1)
	{
		Matrix4x4 R = pSN->m_base;
		Vector3 N_ground = Vector3(0, 1, 0);
		Vector3 N_wall = Vector3(0, 0, -1);
		Vector3 N_wall2 = Vector3(0, 0, 1);
		Vector3 N_wall3 = Vector3(-1, 0, 0);
		Vector3 N_wall4 = Vector3(1, 0, 0);
		Vector3 r_sum = Vector3(0, 0, 0);
		Vector3 r_sum2 = Vector3(0, 0, 0);
		Vector3 r_i, pos_i, v_i;
		float c = 0, c2 = 0, c3 = 0, c4 = 0, c5 = 0;
		PhysicsComponent *pPC = getParentsPhysicalComponent();
		std::vector<Vector3> vertices = pPC->m_meshPoints;//
		SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
		PE::Handle hSoldierSceneNode = pSol->getFirstComponentHandle<PE::Components::SceneNode>();
		Matrix4x4 base = hSoldierSceneNode.getObject<PE::Components::SceneNode>()->m_worldTransform;
		//collision detection
		for (int i = 0; i < vertices.size(); i=i+10)
		{
			r_i = R*(vertices[i]);
			//r_i = R*pos;
			pos_i = pos + r_i;
			Vector3 pos_i1 = pos_i;
			Vector3 pos_i2 = pos_i;
			v_i = v + w.crossProduct(r_i);

			if (pos_i.dotProduct(N_ground) < 0 && v_i.dotProduct(N_ground)<0)
			{
				r_sum += r_i;
				c += 1;
			}
			Vector3 temp_p = pos_i1 -StaticObject[0]->m_pc_base* Vector3(0, -12, 0);
			if (temp_p.dotProduct(N_wall) < 0 && v_i.dotProduct(N_wall)<0)
			{
				r_sum2 += r_i;
				c2 += 1;
			}
			
			Vector3 temp_p2 = pos_i2 - StaticObject[1]->m_pc_base* Vector3(-11, -0, -24);
			if (temp_p2.dotProduct(N_wall2) < 0 && v_i.dotProduct(N_wall2)<0)
			{
				r_sum2 += r_i;
				c3 += 1;
			}
			//right,  change y for wall pos
			Vector3 temp_p3 = pos_i2 - StaticObject[2]->m_pc_base* Vector3(0, -10, -0);
			if (temp_p3.dotProduct(N_wall3) < 0 && v_i.dotProduct(N_wall3)<0)
			{
				r_sum2 += r_i;
				c4 += 1;
			}
			//left change x for wall pos
			Vector3 temp_p4 = pos_i2 - StaticObject[3]->m_pc_base* Vector3(-23, -0, -0);
			if (temp_p4.dotProduct(N_wall4) < 0 && v_i.dotProduct(N_wall4)<0)
			{
				r_sum2 += r_i;
				c5 += 1;
			}

		}
		//collision response for ground
		if (c != 0.0f)
		{
			r_i = r_sum / c;
			v_i = v + w.crossProduct(r_i);
			Matrix4x4 R_i = Get_Cross_Matrix(r_i);
			Matrix4x4 I = R * I_body * R.transpose();
			Matrix4x4 identity = Matrix4x4();
			Matrix4x4 neg = (R_i * I.inverse() * R_i)*(-1.0f);
			Matrix4x4 K = (identity*(1.0f / mass)) + neg;
			Matrix4x4 K_I = K.inverse();

			Vector3 j = (K_I)*(-v_i - restitution * (v_i.dotProduct(N_ground)*N_ground));
			//v = v + (j / mass);
			v = Vector3(0.55f*v.getX(), -0.7*v.getY(), 0.98f*v.getZ());
			//w.m_x += (I.inverse()*r_i.crossProduct( j)).m_x;
			//w.m_y += (I.inverse() * r_i.crossProduct(j)).m_y;
			//w.m_z += (I.inverse() * r_i.crossProduct(j)).m_z;
		}
		if (c2 != 0)
		{
			v = Vector3(0.55f*v.getX(), 0.7*v.getY(), -0.98f*v.getZ());
		}
		if (c3 != 0)
		{
			v = Vector3(0.55f*v.getX(), 0.7*v.getY(), -0.98f*v.getZ());
		}
		if (c4 != 0)
		{
			v = Vector3(-0.98f*v.getX(), 0.7*v.getY(), 0.98f*v.getZ());
		}
		if (c5 != 0)
		{
			v = Vector3(-0.98f*v.getX(), 0.7*v.getY(), 0.98f*v.getZ());
		}


	}
	//
	// Part III: Update position & orientation
	pos = pos + v * dt;

	Quaternion q1 = Quaternion(w.getX(), w.getY(), w.getZ(), 0)*q;
	q = Quaternion(q.getX() + 0.5f * dt * q1.getX(),
		q.getY() + 0.5f * dt * q1.getY(),
		q.getZ() + 0.5f * dt * q1.getZ(),
		q.getW() + 0.5f * dt * q1.getW());
	q.normalize();

	//Update linear status

	//Update angular status


	// Part IV: Assign to the bunny object

	//transform.position = pos;
	//transform.rotation = q;
	//pSN->m_base.turnTo(q.asVector3()-pos);
	pSN->m_base.setPos(pos);
	//original update
	if (0) {
	if (m_state == WALKING_TO_TARGET)
	{
		// see if parent has scene node component
		SceneNode *pSN = getParentsSceneNode();
		PhysicsComponent *pPC = getParentsPhysicalComponent();
		if (pSN)
		{
			Vector3 curPos = pSN->m_base.getPos();
			float dsqr = (m_targetPostion - curPos).lengthSqr();

			bool reached = true;
			if (dsqr > 0.01f)
			{
				// not at the spot yet
				Event_UPDATE *pRealEvt = (Event_UPDATE *)(pEvt);
				static float speed = 1.4f;
				float allowedDisp = speed * pRealEvt->m_frameTime;

				Vector3 dir = (m_targetPostion - curPos);
				dir.normalize();
				float dist = sqrt(dsqr);
				if (dist > allowedDisp)
				{
					dist = allowedDisp; // can move up to allowedDisp
					reached = false; // not reaching destination yet
				}

				// instantaneous turn
				pSN->m_base.turnInDirection(dir, 3.1415f);
				pSN->m_base.setPos(curPos + dir * dist);
			}

			if (reached)
			{
				m_state = STANDING;

				// target has been reached. need to notify all same level state machines (components of parent)
				{
					PE::Handle h("SoldierNPCMovementSM_Event_TARGET_REACHED", sizeof(SoldierNPCMovementSM_Event_TARGET_REACHED));
					Events::SoldierNPCMovementSM_Event_TARGET_REACHED *pOutEvt = new(h) SoldierNPCMovementSM_Event_TARGET_REACHED();

					PE::Handle hParent = getFirstParentByType<Component>();
					if (hParent.isValid())
					{
						hParent.getObject<Component>()->handleEvent(pOutEvt);
					}

					// release memory now that event is processed
					h.release();
				}

				if (m_state == STANDING)
				{
					// no one has modified our state based on TARGET_REACHED callback
					// this means we are not going anywhere right now
					// so can send event to animation state machine to stop
					{
						Events::SoldierNPCAnimSM_Event_STOP evt;

						SoldierNPC *pSol = getFirstParentByTypePtr<SoldierNPC>();
						pSol->getFirstComponent<PE::Components::SceneNode>()->handleEvent(&evt);
					}
				}
			}
		}
	}
}
}

}}




