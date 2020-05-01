#include "Unarmed.h"
#include "GameConstants.h"

Unarmed::Unarmed(CharacterBase & _owner)
	: Weapon(0, 0, 0, 0, _owner) {
	UpdateWeaponStats();

	m_projTxtrName = TxtrNames::NULL_TXTR_NAME;
	m_itemName = "Unarmed";
	// Set the fire rate delay to a high amount to ensure object can never be fired.
	// The delay remaining never decrements, but just to be safe
	m_fireDelayRemaining = 9999.f;
}

