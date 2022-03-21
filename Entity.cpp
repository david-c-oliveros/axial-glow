#include "Entity.h"


Entity::Entity(olc::vf2d vPos, olc::vf2d vVel) :
    m_vPos(vPos), m_vVel(vVel)
{
}


Entity::~Entity() {}


olc::vf2d Entity::GetPos()
{
    return m_vPos;
}


olc::vf2d Entity::GetVel()
{
    return m_vVel;
}


void Entity::SetPos(olc::vf2d vPos)
{
    m_vPos = vPos;
}


void Entity::SetVel(olc::vf2d vVel)
{
    m_vVel = vVel;
}


void Entity::AddVel(olc::vf2d vVel)
{
    m_vVel = m_vVel + vVel;
}
