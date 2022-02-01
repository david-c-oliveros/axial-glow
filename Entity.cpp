#include "Entity.h"


Entity::Entity(olc::vf2d vPos) :
    m_vPos(vPos)
{
    m_vVel = { 0.0f, 0.0f };
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
