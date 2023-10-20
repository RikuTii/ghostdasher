#pragma once



enum RenderState
{
	NoDraw,
	Draw,
	DrawInterval
};

enum EntityType
{
	GeneralEntity,
	LocalplayerEntity,
	PlayerEntity,
	HostileEntity,
	CrateEntity,
	DoorButtonEntity,
	DoorEntity,
	ScaredHostileEntity,
	BouncingBossEntity,
	ProjectileEntity
};

enum EntityCategory
{
	CategoryGeneral,
	CategoryBoss,
	CategoryAlwaysActive
};

enum DamageType
{
	MeleeDamage,
	ProjectileDamage
};

enum FacingDirection
{
	Up,
	Down,
	Left,
	Right
};

enum IntersectionDirection
{
	InteresectNone,
	InteresecLeft,
	InteresecUp,
	InteresecRight,
	InteresecDown
};