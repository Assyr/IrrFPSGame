#pragma once


#ifndef INC_PROJECTILE_H
#define INC_PROJECTILE_H


#include <irrlicht.h>
using namespace irr;


class Projectile
{
public:
	Projectile(const core::vector3df& pos, const core::vector3df& dir, scene::ISceneManager* smgr);

	~Projectile();

	void remove();

	bool update();

	inline const core::vector3df& getPreviousPosition() { return prevPos; }
	inline core::vector3df getPosition() { return Bill->getPosition(); }
private:
	scene::ISceneManager* sceneManager;
	core::vector3df direction;
	core::vector3df prevPos;
	scene::ISceneNode* Bill;
	f32 distanceTravelled;


};

#endif INC_PROJECTILE_H