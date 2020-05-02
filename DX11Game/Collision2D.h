#pragma once
#include <D3D11.h>

#include "Sprite.h"
#include "SimpleMath.h"
/*
Collision detection and logic adapted from https://noonat.github.io/intersect/#aabb-vs-swept-aabb
Converted to C++
*/

// Type-safe method to retrieve the sign of a value (-1, 0, 1)
template<typename T>
int sign(T _val) {
	return (T(0) < _val) - (_val < T(0));
}

struct Point {
	Point(float _x = 0.f, float _y = 0.f);
	Point(const DirectX::SimpleMath::Vector2& _point);
	float x, y;

	Point& operator=(const DirectX::SimpleMath::Vector2& _newPoint) {
		x = _newPoint.x;
		y = _newPoint.y;

		return *this;
	}

	// Copy an instance of this Point
	Point Clone();

	// Normalises the point vector and returns the length
	float Normalise();

	// Returns Point in DX11 Vector2
	DirectX::SimpleMath::Vector2 ToVec2();
};

class Collider;

struct Hit {
	Collider* collider;
	Point position;		// Point of contact between 2 objects
	Point delta;		// Overlap between 2 objects
	Point normal;		// Surface normal at the point of contact
	float time;			/* Used for Segment/Sweep intersections, 
						   fraction from 0-1 for how far along the line the collision occured */

	Hit(Collider* _ptrCollider);
	bool Collided() const;
};

struct Sweep {
	Hit*   hit;			// Becomes a Hit object on collision, null when there is no collision
	Point position;		// Furthest point the object reached along the swept path before hitting something
	float time;			// Copy of Sweep.hit->time, offset by EPSILON, or 1 if the object didn't hit anything during sweep

	Sweep();
};

class Collider {
public:
	Collider();
	Collider(const Collider& _collider);
	Collider(const Sprite& _sprite);
	Collider(const Point& _pos, const Point& _half);
	Collider(const DirectX::SimpleMath::Vector2& _pos, const DirectX::SimpleMath::Vector2& _half);

	Point GetPosition() const;
	void SetPosition(const DirectX::SimpleMath::Vector2& _pos);

	Point GetHixboxRadius() const;
	void SetHitboxRadius(const DirectX::SimpleMath::Vector2& _radius);

	std::string GetTag() const { return m_colliderTag; }
	void SetTag(const std::string& _tag) { m_colliderTag = _tag; }

	void Update(const Sprite& _sprite);

	// Find the overlap on X and Y axis, find axis with smallest overlap 
	// to create intersection point on the edge of the box if possible
	Hit IntersectPoint(const Point& _point);
	Hit IntersectPoint(const DirectX::SimpleMath::Vector2& _point);
	// Finds the time of the line's intersection with the near and far edges
	// of each axis of the AABB.
	Hit IntersectSegment(const Point& _pos, const Point& _delta,
		float _paddingX = 0.f, float _paddingY = 0.f);
	// Find overlaps between 2 boxes on each axis.
	Hit IntersectAABB(const Collider& _collider);
	Sweep SweepAABB(const Collider& _box, Point _delta);
	Sweep SweepInto(const std::vector<Collider>& _staticColliders, const Point& _delta);
private:
	Point m_position;		// Central point of box collider
	Point m_half;			// Half box size for each axis (Radius)

	std::string m_colliderTag;
};