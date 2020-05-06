#include "Collision2D.h"
#include "GameConstants.h"

#include <algorithm>
#include "D3DUtil.h"

// Default
Collider::Collider()
	: m_position({ 0.f, 0.f }), m_half({ 0.f, 0.f }) 
{}

// Create a copy of an existing collider
Collider::Collider(const Collider& _collider)
	: m_position(_collider.m_position), m_half(_collider.m_half) {
	DBOUT("Collider Copied!");
}

// Use sprite information to generate collider box
Collider::Collider(const Sprite& _sprite)
	: Collider(_sprite.GetPos(), _sprite.GetScreenRadius())
{}

// Use position and radius to generate collider box with Collision datatypes
Collider::Collider(const Point& _pos, const Point& _half)
	: m_position(_pos), m_half(_half) 
{}

// Use position and radius to generate collider box with DirectX datatypes
Collider::Collider(const DirectX::SimpleMath::Vector2& _pos, const DirectX::SimpleMath::Vector2& _half)
	: m_position(_pos), m_half(_half) {
}

Point Collider::GetPosition() const {
	return m_position;
}

// Set origin position of collider box
void Collider::SetPosition(const DirectX::SimpleMath::Vector2 & _pos) {
	m_position = _pos;
}

Point Collider::GetHixboxRadius() const {
	return m_half;
}

// Set radius from origin of collider box
void Collider::SetHitboxRadius(const DirectX::SimpleMath::Vector2 & _radius) {
	m_half = _radius;
}

void Collider::Update(const Sprite& _sprite) {
	SetPosition(_sprite.GetPos());
}

Point::Point(float _x, float _y)
	: x(_x), y(_y) 
{}

Point::Point(const DirectX::SimpleMath::Vector2 & _point)
	: x(_point.x), y(_point.y) 
{}

Point Point::Clone() {
	return Point(this->x, this->y);
}

float Point::Normalise() {
	float length = (this->x * this->x) + (this->y * this->y);

	if (length > 0.f) {
		// Get the inverse sqrt of the length
		length = sqrtf(length);
		const float invSqrtLen = 1.f / length;

		this->x *= invSqrtLen;
		this->y *= invSqrtLen;
	}
	else {
		this->x = 1.f;
		this->y = 0.f;
	}

	return length;
}

DirectX::SimpleMath::Vector2 Point::ToVec2()
{
	return DirectX::SimpleMath::Vector2(x, y);
}

Sweep::Sweep() 
	: hit(nullptr), position(Point()), time(1.f) 
{}

Hit::Hit(Collider* _collider) 
	: collider(_collider), position(Point()),
	delta(Point()), normal(Point()), time(0.f) 
{}

bool Hit::Collided() const {
	return collider != nullptr;
}

Hit Collider::IntersectPoint(const Point & _point) {
	const float deltaX = _point.x - this->m_position.x;
	const float pointX = this->m_half.x - fabsf(deltaX);

	// No collision possible
	if (!(pointX > 0))
		return nullptr;

	const float deltaY = _point.y - this->m_position.y;
	const float pointY = this->m_half.y - fabsf(deltaY);

	// No collision possible
	if (!(pointY > 0))
		return nullptr;

	// Collision found
	Hit hit = Hit(this);

	// Calculate axis with smallest overlap to create intersection point
	if (pointX < pointY) {
		const float signX = (float)sign(deltaX);
		hit.delta.x = pointX * signX;
		hit.normal.x = signX;
		hit.position.x = this->m_position.x + (this->m_half.x * signX);
		hit.position.y = _point.y;
	}
	else {
		const float signY = (float)sign(deltaY);
		hit.delta.y = pointY * signY;
		hit.normal.y = signY;
		hit.position.x = _point.x;
		hit.position.y = this->m_position.y + (this->m_half.y * signY);
	}

	return hit;
}

Hit Collider::IntersectPoint(const DirectX::SimpleMath::Vector2 & _point) {
	Point vToP = Point(_point);

	return IntersectPoint(vToP);
}

Hit Collider::IntersectSegment(const Point & _pos, const Point& _delta, float _paddingX, float _paddingY) {
	// Based on equation S(t) = A + t(B - A), for 0 <= t <= 1
	const float scaleX = 1.f / _delta.x;
	const float scaleY = 1.f / _delta.y;

	const float signX = (float)sign(scaleX);
	const float signY = (float)sign(scaleY);

	// Calculate linear time at which point the segment intersects with the box's near and far edges
	const float nearTimeX = fabsf((this->m_position.x - signX * (this->m_half.x + _paddingX) - m_position.x) * scaleX);
	const float nearTimeY = fabsf((this->m_position.y - signY * (this->m_half.y + _paddingY) - m_position.y) * scaleY);

	const float farTimeX = fabsf((this->m_position.x + signX * (this->m_half.x + _paddingX) - m_position.x) * scaleX);
	const float farTimeY = fabsf((this->m_position.y + signY * (this->m_half.y + _paddingY) - m_position.y) * scaleY);

	// Compare times to see if collision was possible
	if (nearTimeX > farTimeY || nearTimeY > farTimeX) // NO COLLISION POSSIBLE
		return nullptr;

	// Find greater of 2 times
	const float nearTime = nearTimeX > nearTimeY ? nearTimeX : nearTimeY;
	const float farTime = farTimeX < farTimeY ? farTimeX : farTimeY;
	/* 
	If the near time is greater than or equal to 1, 
	the line starts in front of the nearest edge, but finishes before reaching it.
	If the far time is less than or equal to 0, 
	the line starts in front of the far side of the box, and points away from it.
	*/
	if (nearTime >= 1 || farTime <= 0)
		return nullptr;

	// Collision
	Hit hit = Hit(this);
	hit.time = std::clamp(nearTime, 0.f, 1.f);

	// If near time is greater than 0, the segment starts outside and is entering the box.
	// Otherwise, the segment starts inside the box and is exiting it.
	if (nearTimeX > nearTimeY) {
		hit.normal.x = -signX;
		hit.normal.y = 0.f;
	}
	else {
		hit.normal.x = 0.f;
		hit.normal.y = -signY;
	}

	hit.delta.x = (1.f - hit.time) * -_delta.x;
	hit.delta.y = (1.f - hit.time) * -_delta.y;


	hit.position.x = _pos.x + _delta.x * hit.time;
	hit.position.y = _pos.y + _delta.y * hit.time;

	return hit;
}

Hit Collider::IntersectAABB(const Collider & _box) {
	const float deltaX = _box.m_position.x - this->m_position.x;
	const float pointX = (_box.m_half.x + this->m_half.x) - fabsf(deltaX);

	// No collision possible
	if (!(pointX > 0))
		return nullptr;

	const float deltaY = _box.m_position.y - this->m_position.y;
	const float pointY = (_box.m_half.y + this->m_half.y) - fabsf(deltaY);

	// No collision possible
	if (!(pointY > 0))
		return nullptr;

	// Collision found!
	Hit hit = Hit(this);

	if (pointX < pointY) {
		const float signX = (float)sign(deltaX);

		hit.delta.x = pointX * signX;
		hit.normal.x = signX;
		hit.position.x = this->m_position.x + (this->m_half.x * signX);
		hit.position.y = _box.m_position.y;
	}
	else {
		const float signY = (float)sign(deltaY);

		hit.delta.y = pointY * signY;
		hit.normal.y = signY;
		hit.position.x = _box.m_position.x;
		hit.position.y = this->m_position.y + (this->m_half.y * signY);
	}

	return hit;
}

Sweep Collider::SweepAABB(const Collider& _box, Point _delta) {
	Sweep sweep;

	if (_delta.x == 0 && _delta.y == 0) {
		sweep.position.x = _box.m_position.x;
		sweep.position.y = _box.m_position.y;

		sweep.hit = &this->IntersectAABB(_box);
		sweep.time = sweep.hit ? (sweep.hit->time = 0.f) : 1.f;

		return sweep;
	}
	else {
		sweep.hit = &IntersectSegment(_box.m_position, _delta, _box.m_half.x, _box.m_half.y);

		if (sweep.hit->collider) {
			sweep.time = std::clamp(sweep.hit->time - GameConsts::EPSILON, 0.f, 1.f);
			sweep.position.x = _box.m_position.x + _delta.x * sweep.time;
			sweep.position.y = _box.m_position.y + _delta.y * sweep.time;

			Point direction = _delta.Clone();
			direction.Normalise();

			sweep.hit->position.x = std::clamp(
				sweep.hit->position.x + direction.x * _box.m_half.x,
				this->m_position.x - this->m_half.x, this->m_position.x + this->m_half.x);
			sweep.hit->position.y = std::clamp(
				sweep.hit->position.y + direction.y * _box.m_half.y,
				this->m_position.y - this->m_half.y, this->m_position.y + this->m_half.y);
		}
		else {
			sweep.position.x = _box.m_position.x + _delta.x;
			sweep.position.y = _box.m_position.y + _delta.y;
			sweep.time = 1.f;
		}

		return sweep;
	}
}

Sweep Collider::SweepInto(const std::vector<Collider>& _staticColliders, const Point & _delta) {
	Sweep nearest = Sweep();

	nearest.time = 1.f;
	nearest.position.x = this->m_position.x + _delta.x;
	nearest.position.y = this->m_position.y + _delta.y;

	for (Collider collider : _staticColliders) {
		const Sweep sweep = collider.SweepAABB(*this, _delta);

		if (sweep.time < nearest.time)
			nearest = sweep;
	}

	return nearest;
}
