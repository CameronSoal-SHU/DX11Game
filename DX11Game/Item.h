#pragma once
#include "GameObject.h"
#include "CharacterBase.h"
#include <string>


class PlayMode;

// Item class inherits from the GameObject class
// so it can be rendered and updated onscreen when needed
// (Possibly removed if not necessary?)
class Item : public GameObject
{
public:
	enum item_type { PRIMARY = 0, SECONDARY = 1 };

	Item();
	~Item();

	void Update(float _deltaTime) override;
	void Render(float _deltaTime, DirectX::SpriteBatch& _sprBatch) override;

	virtual void OnUse() = 0;

	/* Public Accessors */
	std::string GetItemName() const { return m_itemName; }
	void SetItemName(const std::string& _name) { m_itemName = _name; }

	CharacterBase& GetOwner() const { return *m_owner; }
	void SetOwner(CharacterBase& _newOwner) { m_owner = &_newOwner; }

	void SetModeOwner(PlayMode& _playMode) { m_ptrPlayMode = &_playMode; }

	bool IsCurrentlyOwned() const { return m_owner != nullptr; }
protected:
	// Does this item belong in the primary, secondary or utility slot?
	item_type m_type;

	D3D& m_d3d;
	PlayMode* m_ptrPlayMode = nullptr;					// Mode owner of object;

	CharacterBase* m_owner = nullptr;	// What Character currently owns this item? (nullptr when owned by no one)
	std::string m_itemName;
};

