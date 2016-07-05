#ifndef INC_CPROJECTILE_H
#define INC_CPROJECTILE_H

#include <irrlicht.h>

using namespace irr;

/** A class representing a simple projectile for demo purposes */
class CProjectile {
      
	public:
		/**
		\brief Constructor
		\param pos - the position to start from
		\param dir - the direction to fly in
		\param smgr - the smgr to create the projectile in
		*/
		CProjectile(const core::vector3df& pos, const core::vector3df& dir, scene::ISceneManager* smgr);
		/**
		\brief Destructor
		*/
		~CProjectile();
		/**
		\brief Removes the projectile, creating an explosion effect at the same time
		*/
		void remove();
		/**
		\brief Updates the projectile's position. Returns true if it wishes to be removed
		\return a bool
		*/
		bool update();
    
		/**
		\brief Returns the current position of the projectile
		\return a vector3df
		*/
		inline core::vector3df getPosition() { return Bill->getPosition(); }
		/** 
		\brief Returns the position of the projectile at the last frame
		\return a vector3df
		*/
		inline const core::vector3df& getPreviousPosition() { return PrevPos; }
    
	private:
		scene::ISceneManager* SceneManager;
		core::vector3df Direction;
		core::vector3df PrevPos;
		scene::ISceneNode* Bill;
		f32 SqDistTravelled;

};

#endif /* INC_CPROJECTILE_H */
