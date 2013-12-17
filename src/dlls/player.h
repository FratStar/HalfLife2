//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef PLAYER_H
#define PLAYER_H
#ifdef _WIN32
#pragma once
#endif

#include "basecombatcharacter.h"
#include "usercmd.h"
#include "playerlocaldata.h"
#include "PlayerState.h"
#include "dlls/iplayerinfo.h"

#define CMD_MAXBACKUP 30

// For queuing and processing usercmds
class CCommandContext
{
public:
	CUserCmd		cmds[ CMD_MAXBACKUP ];

	int				numcmds;
	int				totalcmds;
	int				dropped_packets;
	bool			paused;
};

//-----------------------------------------------------------------------------
// Forward declarations: 
//-----------------------------------------------------------------------------
class CBaseCombatWeapon;
class CBaseViewModel;
class CTeam;
class IPhysicsPlayerController;
class IServerVehicle;
class CUserCmd;
class CFuncLadder;
class CNavArea;

// for step sounds
struct surfacedata_t;

// !!!set this bit on guns and stuff that should never respawn.
#define	SF_NORESPAWN	( 1 << 30 )

//
// Player PHYSICS FLAGS bits
//
enum PlayerPhysFlag_e
{
	PFLAG_DIROVERRIDE	= ( 1<<0 ),		// override the player's directional control (trains, physics gun, etc.)
	PFLAG_DUCKING		= ( 1<<1 ),		// In the process of ducking, but totally squatted yet
	PFLAG_USING			= ( 1<<2 ),		// Using a continuous entity
	PFLAG_OBSERVER		= ( 1<<3 ),		// player is locked in stationary cam mode. Spectators can move, observers can't.
	PFLAG_VPHYSICS_MOTIONCONTROLLER = ( 1<<4 ),	// player is physically attached to a motion controller

	// If you add another flag here check that you aren't 
	// overwriting phys flags in the HL2 player classes
};

//
// generic player
//
//-----------------------------------------------------
//This is Half-Life player entity
//-----------------------------------------------------
#define CSUITPLAYLIST	4		// max of 4 suit sentences queued up at any time
#define	SUIT_REPEAT_OK		0

#define SUIT_NEXT_IN_30SEC	30
#define SUIT_NEXT_IN_1MIN	60
#define SUIT_NEXT_IN_5MIN	300
#define SUIT_NEXT_IN_10MIN	600
#define SUIT_NEXT_IN_30MIN	1800
#define SUIT_NEXT_IN_1HOUR	3600

#define CSUITNOREPEAT		32

#define TEAM_NAME_LENGTH	16

// constant items
#define ITEM_HEALTHKIT		1
#define ITEM_BATTERY		4

#define AUTOAIM_2DEGREES  0.0348994967025
#define AUTOAIM_5DEGREES  0.08715574274766
#define AUTOAIM_8DEGREES  0.1391731009601
#define AUTOAIM_10DEGREES 0.1736481776669
#define AUTOAIM_20DEGREES 0.3490658503989

// useful cosines
#define DOT_1DEGREE   0.9998476951564
#define DOT_2DEGREE   0.9993908270191
#define DOT_3DEGREE   0.9986295347546
#define DOT_4DEGREE   0.9975640502598
#define DOT_5DEGREE   0.9961946980917
#define DOT_6DEGREE   0.9945218953683
#define DOT_7DEGREE   0.9925461516413
#define DOT_8DEGREE   0.9902680687416
#define DOT_9DEGREE   0.9876883405951
#define DOT_10DEGREE  0.9848077530122
#define DOT_15DEGREE  0.9659258262891
#define DOT_20DEGREE  0.9396926207859
#define DOT_25DEGREE  0.9063077870367
#define DOT_30DEGREE  0.866025403784
#define DOT_45DEGREE  0.707106781187
enum
{
	VPHYS_WALK = 0,
	VPHYS_CROUCH,
	VPHYS_NOCLIP,
};


enum PlayerConnectedState
{
	PlayerConnected,
	PlayerDisconnecting,
	PlayerDisconnected,
};

extern bool gInitHUD;
extern ConVar *sv_cheats;

class CBasePlayer;
class CPlayerInfo : public IBotController, public IPlayerInfo
{
public:
	CPlayerInfo () { m_pParent = NULL; } 
	~CPlayerInfo () {}
	void SetParent( CBasePlayer *parent ) { m_pParent = parent; } 

	// IPlayerInfo interface
	virtual const char *GetName();
	virtual int			GetUserID();
	virtual const char *GetNetworkIDString();
	virtual int			GetTeamIndex();
	virtual void		ChangeTeam( int iTeamNum );
	virtual int			GetFragCount();
	virtual int			GetDeathCount();
	virtual bool		IsConnected();
	virtual int			GetArmorValue();

	virtual bool IsHLTV();
	virtual bool IsPlayer();
	virtual bool IsFakeClient();
	virtual bool IsDead();
	virtual bool IsInAVehicle();
	virtual bool IsObserver();
	virtual const Vector GetAbsOrigin();
	virtual const QAngle GetAbsAngles();
	virtual const Vector GetPlayerMins();
	virtual const Vector GetPlayerMaxs();
	virtual const char *GetWeaponName();
	virtual const char *GetModelName();
	virtual const int GetHealth();
	virtual const int GetMaxHealth();

	// bot specific functions	
	virtual void SetAbsOrigin( Vector & vec );
	virtual void SetAbsAngles( QAngle & ang );
	virtual void RemoveAllItems( bool removeSuit );
	virtual void SetActiveWeapon( const char *WeaponName );
	virtual void SetLocalOrigin( const Vector& origin );
	virtual const Vector GetLocalOrigin( void );
	virtual void SetLocalAngles( const QAngle& angles );
	virtual const QAngle GetLocalAngles( void );
	virtual void PostClientMessagesSent( void );
	virtual bool IsEFlagSet( int nEFlagMask );

	virtual void RunPlayerMove( CBotCmd *ucmd );
	virtual void SetLastUserCommand( const CBotCmd &cmd );

	virtual CBotCmd GetLastUserCommand();

private:
	CBasePlayer *m_pParent; 
};


class CBasePlayer : public CBaseCombatCharacter
{
public:
	DECLARE_CLASS( CBasePlayer, CBaseCombatCharacter );
protected:
	// HACK FOR BOTS
	friend class CBotManager;
	static edict_t *s_PlayerEdict; // must be set before calling constructor
public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
	
	CBasePlayer();
	~CBasePlayer();

	// IPlayerInfo passthrough (because we can't do multiple inheritance)
	IPlayerInfo *GetPlayerInfo() { return &m_PlayerInfo; }
	IBotController *GetBotController() { return &m_PlayerInfo; }

	virtual void			UpdateOnRemove( void );

	static CBasePlayer		*CreatePlayer( const char *className, edict_t *ed );

	virtual void			CreateViewModel( int viewmodelindex = 0 );
	CBaseViewModel			*GetViewModel( int viewmodelindex = 0 );
	void					HideViewModels( void );
	void					DestroyViewModels( void );

	CPlayerState			*PlayerData( void ) { return &pl; }
	
	int						RequiredEdictIndex( void ) { return ENTINDEX(edict()); } 

	void					LockPlayerInPlace( void );
	void					UnlockPlayer( void );

	virtual void			DrawDebugGeometryOverlays(void);
	
	// Networking is about to update this entity, let it override and specify it's own pvs
	virtual void			SetupVisibility( CBaseEntity *pViewEntity, unsigned char *pvs, int pvssize );
	virtual int				UpdateTransmitState();
	virtual int				ShouldTransmit( const CCheckTransmitInfo *pInfo );

	// Returns true if this player wants pPlayer to be moved back in time when this player runs usercmds.
	// Saves a lot of overhead on the server if we can cull out entities that don't need to lag compensate
	// (like team members, entities out of our PVS, etc).
	virtual bool			WantsLagCompensationOnEntity( const CBasePlayer	*pPlayer, const CUserCmd *pCmd, const CBitVec<MAX_EDICTS> *pEntityTransmitBits ) const;

	virtual void			Spawn( void );
	virtual void			Activate( void );
	virtual void			SharedSpawn(); // Shared between client and server.

	virtual void			InitialSpawn( void );
	virtual void			InitHUD( void ) {}
	virtual void			ShowViewPortPanel( const char * name, bool bShow = true, KeyValues *data = NULL );

	virtual void			PlayerDeathThink( void );

	virtual void			Jump( void );
	//Lemuel Wilson
	virtual void			DoubleJump( void );
	//Lemuel Wilson
	virtual void			Duck( void );

	const char				*GetTracerType( void );
	void					MakeTracer( const Vector &vecTracerSrc, const trace_t &tr, int iTracerType );
	void					DoImpactEffect( trace_t &tr, int nDamageType );

#if !defined( NO_ENTITY_PREDICTION )
	void					AddToPlayerSimulationList( CBaseEntity *other );
	void					RemoveFromPlayerSimulationList( CBaseEntity *other );
	void					SimulatePlayerSimulatedEntities( void );
	void					ClearPlayerSimulationList( void );
#endif

	// Physics simulation (player executes it's usercmd's here)
	virtual void			PhysicsSimulate( void );
	virtual unsigned int	PhysicsSolidMaskForEntity( void ) const;

	virtual void			PreThink( void );
	virtual void			PostThink( void );
	virtual int				TakeHealth( float flHealth, int bitsDamageType );
	virtual void			TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr );
	bool					ShouldTakeDamageInCommentaryMode( const CTakeDamageInfo &inputInfo );
	virtual int				OnTakeDamage( const CTakeDamageInfo &info );
	virtual void			DamageEffect(float flDamage, int fDamageType);

	virtual void			OnDamagedByExplosion( const CTakeDamageInfo &info );

	virtual Vector			EyePosition( );			// position of eyes
	const QAngle			&EyeAngles( );
	void					EyePositionAndVectors( Vector *pPosition, Vector *pForward, Vector *pRight, Vector *pUp );
	virtual const QAngle	&LocalEyeAngles();		// Direction of eyes
	void					EyeVectors( Vector *pForward, Vector *pRight = NULL, Vector *pUp = NULL );

	// Sets the view angles
	void					SnapEyeAngles( const QAngle &viewAngles );

	virtual QAngle			BodyAngles();
	virtual Vector			BodyTarget( const Vector &posSrc, bool bNoisy);
	virtual bool			ShouldFadeOnDeath( void ) { return FALSE; }
	
	virtual int				OnTakeDamage_Alive( const CTakeDamageInfo &info );
	virtual void			Event_Killed( const CTakeDamageInfo &info );
	void					Event_Dying( void );

	bool					IsHLTV( void ) const { return pl.hltv; }
	virtual	bool			IsPlayer( void ) const { return true; }			// Spectators return TRUE for this, use IsObserver to seperate cases
	virtual bool			IsNetClient( void ) const { return true; }		// Bots should return FALSE for this, they can't receive NET messages
																			// Spectators should return TRUE for this

	virtual bool			IsFakeClient( void ) const;

	// Get the client index (entindex-1).
	int						GetClientIndex()	{ return ENTINDEX( edict() ) - 1; }

	// returns the player name
	const char *			GetPlayerName() { return m_szNetname; }
	void					SetPlayerName( const char *name );

	int						GetUserID() { return engine->GetPlayerUserId( edict() ); }
	const char *			GetNetworkIDString(); 
	virtual const Vector	GetPlayerMins( void ) const; // uses local player
	virtual const Vector	GetPlayerMaxs( void ) const; // uses local player


	void					VelocityPunch( const Vector &vecForce );
	void					ViewPunch( const QAngle &angleOffset );
	void					ViewPunchReset( float tolerance = 0 );
	void					ShowViewModel( bool bShow );
	void					ShowCrosshair( bool bShow );

	// View model prediction setup
	void					CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov );

	// Handle view smoothing when going up stairs
	void					SmoothViewOnStairs( Vector& eyeOrigin );
	virtual float			CalcRoll (const QAngle& angles, const Vector& velocity, float rollangle, float rollspeed);
	void					CalcViewRoll( QAngle& eyeAngles );

	virtual int				Save( ISave &save );
	virtual int				Restore( IRestore &restore );
	virtual bool			ShouldSavePhysics();
	virtual void			OnRestore( void );

	virtual void			PackDeadPlayerItems( void );
	virtual void			RemoveAllItems( bool removeSuit );
	bool					IsDead() const;
#ifdef CSTRIKE_DLL
	virtual bool			IsRunning( void ) const	{ return false; } // bot support under cstrike (AR)
#endif

	bool					HasPhysicsFlag( unsigned int flag ) { return (m_afPhysicsFlags & flag) != 0; }

	// Weapon stuff
	virtual Vector			Weapon_ShootPosition( );
	virtual bool			Weapon_CanUse( CBaseCombatWeapon *pWeapon );
	virtual void			Weapon_Equip( CBaseCombatWeapon *pWeapon );
	virtual	void			Weapon_Drop( CBaseCombatWeapon *pWeapon, const Vector *pvecTarget /* = NULL */, const Vector *pVelocity /* = NULL */ );
	virtual	bool			Weapon_Switch( CBaseCombatWeapon *pWeapon, int viewmodelindex = 0 );		// Switch to given weapon if has ammo (false if failed)
	virtual void			Weapon_SetLast( CBaseCombatWeapon *pWeapon );
	virtual bool			Weapon_ShouldSetLast( CBaseCombatWeapon *pOldWeapon, CBaseCombatWeapon *pNewWeapon ) { return true; }
	virtual bool			Weapon_ShouldSelectItem( CBaseCombatWeapon *pWeapon );
	void					Weapon_DropSlot( int weaponSlot );

	bool					HasAnyAmmoOfType( int nAmmoIndex );

	// JOHN:  sends custom messages if player HUD data has changed  (eg health, ammo)
	virtual void			UpdateClientData( void );
	void					RumbleEffect( unsigned char index, unsigned char rumbleData, unsigned char rumbleFlags );
	
	// Player is moved across the transition by other means
	virtual int				ObjectCaps( void ) { return BaseClass::ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
	virtual void			Precache( void );
	bool					IsOnLadder( void );
	virtual void			ExitLadder() {}

	virtual int				FlashlightIsOn( void ) { return false; }
	virtual void			FlashlightTurnOn( void ) { };
	virtual void			FlashlightTurnOff( void ) { };
	virtual bool			IsIlluminatedByFlashlight( CBaseEntity *pEntity, float *flReturnDot ) {return false; }
	
	void					UpdatePlayerSound ( void );
	virtual void			UpdateStepSound( surfacedata_t *psurface, const Vector &vecOrigin, const Vector &vecVelocity );
	virtual void			PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force );
	virtual void			DeathSound( const CTakeDamageInfo &info );

	Class_T					Classify ( void );
	virtual void			SetAnimation( PLAYER_ANIM playerAnim );
	void					SetWeaponAnimType( const char *szExtention );

	// custom player functions
	virtual void			ImpulseCommands( void );
	virtual void			CheatImpulseCommands( int iImpulse );
	virtual bool			ClientCommand(const char *cmd);

	void					NotifySinglePlayerGameEnding() { m_bSinglePlayerGameEnding = true; }
	bool					IsSinglePlayerGameEnding() { return m_bSinglePlayerGameEnding == true; }
	
	// Observer functions
	virtual bool			StartObserverMode(int mode); // true, if successful
	virtual void			StopObserverMode( void );	// stop spectator mode
	virtual bool			SetObserverMode(int mode); // sets new observer mode, returns true if successful
	virtual int				GetObserverMode( void ); // returns observer mode or OBS_NONE
	virtual bool			SetObserverTarget(CBaseEntity * target);
	virtual void			ObserverUse( bool bIsPressed ); // observer pressed use
	virtual CBaseEntity		*GetObserverTarget( void ); // returns players targer or NULL
	virtual CBaseEntity		*FindNextObserverTarget( bool bReverse ); // returns next/prev player to follow or NULL
	virtual int				GetNextObserverSearchStartPoint( bool bReverse ); // Where we should start looping the player list in a FindNextObserverTarget call
	virtual bool			IsValidObserverTarget(CBaseEntity * target); // true, if player is allowed to see this target
	virtual void			CheckObserverSettings(); // checks, if target still valid (didn't die etc)
	virtual void			JumptoPosition(const Vector &origin, const QAngle &angles);
	virtual void			ForceObserverMode(int mode); // sets a temporary mode, force because of invalid targets
	virtual void			ResetObserverMode(); // resets all observer related settings

	virtual bool			StartReplayMode( float fDelay, float fDuration, int iEntity );
	virtual void			StopReplayMode();
	virtual int				GetDelayTicks();
	virtual int				GetReplayEntity();

	virtual void			CreateCorpse( void ) { }
	virtual CBaseEntity		*EntSelectSpawnPoint( void );

	// Vehicles
	bool					IsInAVehicle( void ) const;
	virtual bool			GetInVehicle( IServerVehicle *pVehicle, int nRole );
	virtual void			LeaveVehicle( const Vector &vecExitPoint = vec3_origin, const QAngle &vecExitAngles = vec3_angle );
	int						GetVehicleAnalogControlBias() { return m_iVehicleAnalogBias; }
	void					SetVehicleAnalogControlBias( int bias ) { m_iVehicleAnalogBias = bias; }
	
	// override these for 
	virtual void			OnVehicleStart() {}
	virtual void			OnVehicleEnd( Vector &playerDestPosition ) {} 
	IServerVehicle			*GetVehicle();
	CBaseEntity				*GetVehicleEntity( void );
	bool					UsingStandardWeaponsInVehicle( void );
	
	void					AddPoints( int score, bool bAllowNegativeScore );
	void					AddPointsToTeam( int score, bool bAllowNegativeScore );
	virtual bool			BumpWeapon( CBaseCombatWeapon *pWeapon );
	bool					RemovePlayerItem( CBaseCombatWeapon *pItem );
	CBaseEntity				*HasNamedPlayerItem( const char *pszItemName );
	bool 					HasWeapons( void );// do I have ANY weapons?
	virtual void			SelectLastItem(void);
	virtual void 			SelectItem( const char *pstr, int iSubType = 0 );
	void					ItemPreFrame( void );
	virtual void			ItemPostFrame( void );
	virtual CBaseEntity		*GiveNamedItem( const char *szName, int iSubType = 0 );
	void					EnableControl(bool fControl);
	virtual void			CheckTrainUpdate( void );
	void					AbortReload( void );

	void					SendAmmoUpdate(void);

	void					WaterMove( void );
	float					GetWaterJumpTime() const;
	void					SetWaterJumpTime( float flWaterJumpTime );
	float					GetSwimSoundTime( void ) const;
	void					SetSwimSoundTime( float flSwimSoundTime );

	virtual void			SetPlayerUnderwater( bool state );
	bool					IsPlayerUnderwater( void ) { return m_bPlayerUnderwater; }
	virtual bool			CanBreatheUnderwater() const { return false; }
	virtual void			PlayerUse( void );
	virtual void			PlayUseDenySound() {}

	virtual CBaseEntity		*FindUseEntity( void );
	virtual bool			IsUseableEntity( CBaseEntity *pEntity, unsigned int requiredCaps );
	bool					ClearUseEntity();
	CBaseEntity				*DoubleCheckUseNPC( CBaseEntity *pNPC, const Vector &vecSrc, const Vector &vecDir );


	// physics interactions
	// mass/size limit set to zero for none
	static bool				CanPickupObject( CBaseEntity *pObject, float massLimit, float sizeLimit );
	virtual void			PickupObject( CBaseEntity *pObject, bool bLimitMassAndSize = true ) {}
	virtual void			ForceDropOfCarriedPhysObjects( CBaseEntity *pOnlyIfHoldindThis = NULL ) {}
	virtual float			GetHeldObjectMass( IPhysicsObject *pHeldObject );

	void					CheckSuitUpdate();
	void					SetSuitUpdate(char *name, int fgroup, int iNoRepeat);
	virtual void			UpdateGeigerCounter( void );
	void					CheckTimeBasedDamage( void );

	void					ResetAutoaim( void );
	
	virtual Vector			GetAutoaimVector( float flScale );
	virtual Vector			GetAutoaimVector( float flScale, float flMaxDist );
	virtual void			GetAutoaimVector( autoaim_params_t &params );

	float					GetAutoaimScore( const Vector &eyePosition, const Vector &viewDir, const Vector &vecTarget, CBaseEntity *pTarget, float fScale );
	QAngle					AutoaimDeflection( Vector &vecSrc, autoaim_params_t &params );
	virtual bool			ShouldAutoaim( void );
	void					SetTargetInfo( Vector &vecSrc, float flDist );

	void					SetViewEntity( CBaseEntity *pEntity );
	CBaseEntity				*GetViewEntity( void ) { return m_hViewEntity; }

	virtual void			ForceClientDllUpdate( void );  // Forces all client .dll specific data to be resent to client.

	void					DeathMessage( CBaseEntity *pKiller );

	virtual void			ProcessUsercmds( CUserCmd *cmds, int numcmds, int totalcmds,
								int dropped_packets, bool paused );

	void					AvoidPhysicsProps( CUserCmd *pCmd );

	// Run a user command. The default implementation calls ::PlayerRunCommand. In TF, this controls a vehicle if
	// the player is in one.
	virtual void			PlayerRunCommand(CUserCmd *ucmd, IMoveHelper *moveHelper);
	void					RunNullCommand();
	CUserCmd *				GetCurrentCommand( void )	{ return m_pCurrentCommand; }

	// Team Handling
	virtual void			ChangeTeam( int iTeamNum );

	// say/sayteam allowed?
	virtual bool			CanSpeak( void ) { return true; }
	virtual bool			CanHearChatFrom( CBasePlayer *pPlayer ) { return true; }
	audioparams_t			&GetAudioParams() { return m_Local.m_audio; }

	virtual void 			ModifyOrAppendPlayerCriteria( AI_CriteriaSet& set );

	const QAngle& GetPunchAngle();
	void SetPunchAngle( const QAngle &punchAngle );

	virtual void DoMuzzleFlash();

	CNavArea *GetLastKnownArea( void ) const		{ return m_lastNavArea; }		// return the last nav area the player occupied - NULL if unknown
	const char *GetLastKnownPlaceName( void ) const	{ return m_szLastPlaceName; }	// return the last nav place name the player occupied

	virtual void			CheckChatText( char *p, int bufsize ) {}

public:
	// Player Physics Shadow
	void					SetupVPhysicsShadow( CPhysCollide *pStandModel, const char *pStandHullName, CPhysCollide *pCrouchModel, const char *pCrouchHullName );
	IPhysicsPlayerController* GetPhysicsController() { return m_pPhysicsController; }
	virtual void			VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );
	void					VPhysicsUpdate( IPhysicsObject *pPhysics );
	virtual void			VPhysicsShadowUpdate( IPhysicsObject *pPhysics );
	virtual bool			IsFollowingPhysics( void ) { return false; }
	bool					IsRideablePhysics( IPhysicsObject *pPhysics );
	IPhysicsObject			*GetGroundVPhysics();

	virtual void			Touch( CBaseEntity *pOther );
	void					SetTouchedPhysics( bool bTouch );
	bool					TouchedPhysics( void );
	Vector					GetSmoothedVelocity( void );

	virtual void			InitVCollision( void );
	virtual void			VPhysicsDestroyObject();
	void					SetVCollisionState( int collisionState );
	void					PostThinkVPhysics( void );
	virtual void			UpdatePhysicsShadowToCurrentPosition();
	void					UpdateVPhysicsPosition( const Vector &position, const Vector &velocity, float secondsToArrival );

	// Accessor methods
	int		FragCount() const		{ return m_iFrags; }
	int		DeathCount() const		{ return m_iDeaths;}
	bool	IsConnected() const		{ return m_iConnected != PlayerDisconnected; }
	bool	IsDisconnecting() const	{ return m_iConnected == PlayerDisconnecting; }
	bool	IsSuitEquipped() const	{ return m_Local.m_bWearingSuit; }
	int		ArmorValue() const		{ return m_ArmorValue; }
	bool	HUDNeedsRestart() const { return m_fInitHUD; }
	float	MaxSpeed() const		{ return m_flMaxspeed; }
	Activity GetActivity( ) const	{ return m_Activity; }
	inline void SetActivity( Activity eActivity ) { m_Activity = eActivity; }
	bool	IsPlayerLockedInPlace() const { return m_iPlayerLocked != 0; }
	bool	IsObserver() const		{ return (m_afPhysicsFlags & PFLAG_OBSERVER) != 0; }
	bool	IsOnTarget() const		{ return m_fOnTarget; }
	float	MuzzleFlashTime() const { return m_flFlashTime; }
	float	PlayerDrownTime() const	{ return m_AirFinished; }

	int		GetObserverMode() const	{ return m_iObserverMode; }
	CBaseEntity *GetObserverTarget() const	{ return m_hObserverTarget; }

	void	ResetFragCount();
	void	IncrementFragCount( int nCount );

	void	ResetDeathCount();
	void	IncrementDeathCount( int nCount );

	void	SetArmorValue( int value );
	void	IncrementArmorValue( int nCount, int nMaxValue = -1 );

	void	SetConnected( PlayerConnectedState iConnected ) { m_iConnected = iConnected; }
	virtual void EquipSuit( bool bPlayEffects = true );
	virtual void RemoveSuit( void );
	void	SetMaxSpeed( float flMaxSpeed ) { m_flMaxspeed = flMaxSpeed; }

	void	NotifyNearbyRadiationSource( float flRange );

	void	SetAnimationExtension( const char *pExtension );

	void	SetAdditionalPVSOrigin( const Vector &vecOrigin );
	void	SetCameraPVSOrigin( const Vector &vecOrigin );
	void	SetMuzzleFlashTime( float flTime );
	void	SetUseEntity( CBaseEntity *pUseEntity );
	CBaseEntity *GetUseEntity();

	// Only used by the physics gun... is there a better interface?
	void	SetPhysicsFlag( int nFlag, bool bSet );

	void	AllowImmediateDecalPainting();

	// Suicide...
	virtual void CommitSuicide();

	// For debugging...
	void	ForceOrigin( const Vector &vecOrigin );

	// Bot accessors...
	void	SetTimeBase( float flTimeBase );
	float	GetTimeBase() const;
	void	SetLastUserCommand( const CUserCmd &cmd );
	const CUserCmd *GetLastUserCommand( void );
	virtual bool IsBot() const;

	bool	IsPredictingWeapons( void ) const; 
	int		CurrentCommandNumber() const;
	const CUserCmd *GetCurrentUserCommand() const;

	int		GetFOV( void ) const;												// Get the current FOV value
	int		GetDefaultFOV( void ) const;										// Default FOV if not specified otherwise
	bool	SetFOV( CBaseEntity *pRequester, int FOV, float zoomRate = 0.0f );	// Alters the base FOV of the player (must have a valid requester)
	void	SetDefaultFOV( int FOV );											// Sets the base FOV if nothing else is affecting it by zooming
	CBaseEntity *GetFOVOwner( void ) { return m_hZoomOwner; }
	float	GetFOVDistanceAdjustFactor(); // shared between client and server

	int		GetImpulse( void ) const { return m_nImpulse; }

	// Movement constraints
	void	ActivateMovementConstraint( CBaseEntity *pEntity, const Vector &vecCenter, float flRadius, float flConstraintWidth, float flSpeedFactor );
	void	DeactivateMovementConstraint( );

	// talk control
	void	NotePlayerTalked() { m_fLastPlayerTalkTime = gpGlobals->curtime; }
	float	LastTimePlayerTalked() { return m_fLastPlayerTalkTime; }

	//---------------------------------
	// Inputs
	//---------------------------------
	void	InputSetHealth( inputdata_t &inputdata );
	void	InputSetHUDVisibility( inputdata_t &inputdata );

	surfacedata_t *GetSurfaceData( void ) { return m_pSurfaceData; }
	void SetLadderNormal( Vector vecLadderNormal ) { m_vecLadderNormal = vecLadderNormal; }

private:
	
	// For queueing up CUserCmds and running them from PhysicsSimulate
	int					GetCommandContextCount( void ) const;
	CCommandContext		*GetCommandContext( int index );
	CCommandContext		*AllocCommandContext( void );
	void				RemoveCommandContext( int index );
	void				RemoveAllCommandContexts( void );

	int					DetermineSimulationTicks( void );
	void				AdjustPlayerTimeBase( int simulation_ticks );

public:
	
	// Used by gamemovement to check if the entity is stuck.
	int m_StuckLast;
	
	// FIXME: Make these protected or private!

	// This player's data that should only be replicated to 
	//  the player and not to other players.
	CNetworkVarEmbedded( CPlayerLocalData, m_Local );

	// Used by env_soundscape_triggerable to manage when the player is touching multiple
	// soundscape triggers simultaneously.
	// The one at the HEAD of the list is always the current soundscape for the player.
	CUtlVector<EHANDLE> m_hTriggerSoundscapeList;

	// Player data that's sometimes needed by the engine
	CNetworkVarEmbedded( CPlayerState, pl );

	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_vecViewOffset );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_flFriction );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_iAmmo );
	
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_hGroundEntity );

	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_lifeState );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_iHealth );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_vecBaseVelocity );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_nNextThinkTick );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_vecVelocity );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_nWaterLevel );
	
	int						m_nButtons;
	int						m_afButtonPressed;
	int						m_afButtonReleased;
	int						m_afButtonLast;

	CNetworkVar( bool, m_fOnTarget );		//Is the crosshair on a target?

	char					m_szAnimExtension[32];

	int						m_nUpdateRate;		// user snapshot rate cl_updaterate
	float					m_fLerpTime;		// users cl_interp
	bool					m_bLagCompensation;	// user wants lag compenstation
	bool					m_bPredictWeapons; //  user has client side predicted weapons
	
	float		GetDeathTime( void ) { return m_flDeathTime; }

private:

	Activity				m_Activity;

protected:

	void					CalcPlayerView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov );
	void					CalcVehicleView( IServerVehicle *pVehicle, Vector& eyeOrigin, QAngle& eyeAngles, 	
								float& zNear, float& zFar, float& fov );
	void					CalcObserverView( Vector& eyeOrigin, QAngle& eyeAngles, float& fov );
	void					CalcViewModelView( const Vector& eyeOrigin, const QAngle& eyeAngles);

	// FIXME: Make these private! (tf_player uses them)

	// Secondary point to derive PVS from when zoomed in with binoculars/sniper rifle.  The PVS is 
	//  a merge of the standing origin and this additional origin
	Vector					m_vecAdditionalPVSOrigin; 
	// Extra PVS origin if we are using a camera object
	Vector					m_vecCameraPVSOrigin;

	CNetworkHandle( CBaseEntity, m_hUseEntity );			// the player is currently controlling this entity because of +USE latched, NULL if no entity

	int						m_iTrain;				// Train control position

	float					m_iRespawnFrames;	// used in PlayerDeathThink() to make sure players can always respawn
 	unsigned int			m_afPhysicsFlags;	// physics flags - set when 'normal' physics should be revisited or overriden
	
	// Vehicles
	CNetworkHandle( CBaseEntity, m_hVehicle );

	int						m_iVehicleAnalogBias;

	void					UpdateButtonState( int nUserCmdButtonMask );

	int						m_lastDamageAmount;		// Last damage taken

	Vector					m_DmgOrigin;
	float					m_DmgTake;
	float					m_DmgSave;
	int						m_bitsDamageType;	// what types of damage has player taken
	int						m_bitsHUDDamage;	// Damage bits for the current fame. These get sent to the hud via gmsgDamage

	CNetworkVar( float, m_flDeathTime );		// the time at which the player died  (used in PlayerDeathThink())

	CNetworkVar( int, m_iObserverMode );	// if in spectator mode != 0
	CNetworkVar( int,	m_iFOV );			// field of view
	CNetworkVar( int,	m_iDefaultFOV );	// default field of view

	int						m_iObserverLastMode; // last used observer mode
	CNetworkHandle( CBaseEntity, m_hObserverTarget );	// entity handle to m_iObserverTarget
	bool					m_bForcedObserverMode; // true, player was forced by invalid targets to switch mode
	
	EHANDLE					m_hZoomOwner;		//This is a pointer to the entity currently controlling the player's zoom
												//Only this entity can change the zoom state once it has ownership

	float					m_tbdPrev;				// Time-based damage timer
	int						m_idrowndmg;			// track drowning damage taken
	int						m_idrownrestored;		// track drowning damage restored
	int						m_nPoisonDmg;			// track recoverable poison damage taken
	int						m_nPoisonRestored;		// track poison damage restored
	// NOTE: bits damage type appears to only be used for time-based damage
	BYTE					m_rgbTimeBasedDamage[CDMG_TIMEBASED];

	// Player Physics Shadow
	int						m_vphysicsCollisionState;

	virtual int SpawnArmorValue( void ) const { return 0; }

	float					m_fNextSuicideTime; // the time after which the player can next use the suicide command

	// Replay mode	
	float					m_fDelay;			// replay delay in seconds
	float					m_fReplayEnd;		// time to stop replay mode
	int						m_iReplayEntity;	// follow this entity in replay

private:
	void HandleFuncTrain();

// DATA
private:
	CUtlVector< CCommandContext > m_CommandContext;
	// Player Physics Shadow

protected: 
	IPhysicsPlayerController	*m_pPhysicsController;
	IPhysicsObject				*m_pShadowStand;
	IPhysicsObject				*m_pShadowCrouch;
	Vector						m_oldOrigin;
	Vector						m_vecSmoothedVelocity;
	bool						m_touchedPhysObject;

private:

	int						m_iPlayerSound;// the index of the sound list slot reserved for this player
	int						m_iTargetVolume;// ideal sound volume. 
	
	int						m_rgItems[MAX_ITEMS];

	// these are time-sensitive things that we keep track of
	float					m_flTimeStepSound;	// when the last stepping sound was made
	float					m_flSwimTime;		// how long player has been underwater
	float					m_flDuckTime;		// how long we've been ducking
	float					m_flDuckJumpTime;	

	float					m_flSuitUpdate;					// when to play next suit update
	int						m_rgSuitPlayList[CSUITPLAYLIST];// next sentencenum to play for suit update
	int						m_iSuitPlayNext;				// next sentence slot for queue storage;
	int						m_rgiSuitNoRepeat[CSUITNOREPEAT];		// suit sentence no repeat list
	float					m_rgflSuitNoRepeatTime[CSUITNOREPEAT];	// how long to wait before allowing repeat

	float					m_flgeigerRange;		// range to nearest radiation source
	float					m_flgeigerDelay;		// delay per update of range msg to client
	int						m_igeigerRangePrev;

	bool					m_fInitHUD;				// True when deferred HUD restart msg needs to be sent
	bool					m_fGameHUDInitialized;
	bool					m_fWeapon;				// Set this to FALSE to force a reset of the current weapon HUD info

	int						m_iUpdateTime;		// stores the number of frame ticks before sending HUD update messages
	int						m_iClientBattery;	// the Battery currently known by the client.  If this changes, send a new

	// Autoaim data
	QAngle					m_vecAutoAim;
	int						m_lastx, m_lasty;	// These are the previous update's crosshair angles, DON"T SAVE/RESTORE

	int						m_iFrags;
	int						m_iDeaths;

	float					m_flNextDecalTime;// next time this player can spray a decal

	// Team Handling
	// char					m_szTeamName[TEAM_NAME_LENGTH];

	// Multiplayer handling
	PlayerConnectedState	m_iConnected;

	// from edict_t
	// CBasePlayer doesn't send this but CCSPlayer does.
	CNetworkVarForDerived( int, m_ArmorValue );
	float					m_AirFinished;
	float					m_PainFinished;

	// player locking
	int						m_iPlayerLocked;

		
protected:
	// the player's personal view model
	typedef CHandle<CBaseViewModel> CBaseViewModelHandle;
	CNetworkArray( CBaseViewModelHandle, m_hViewModel, MAX_VIEWMODELS );

	// Last received usercmd (in case we drop a lot of packets )
	CUserCmd				m_LastCmd;
	CUserCmd				*m_pCurrentCommand;

	float					m_flStepSoundTime;	// time to check for next footstep sound

private:

// Replicated to all clients
	CNetworkVar( float, m_flMaxspeed );
	
// Not transmitted
	float					m_flWaterJumpTime;  // used to be called teleport_time
	Vector					m_vecWaterJumpVel;
	int						m_nImpulse;
	float					m_flSwimSoundTime;
	Vector					m_vecLadderNormal;

	float					m_flFlashTime;
	int						m_nDrownDmgRate;		// Drowning damage in points per second without air.

	int						m_nNumCrouches;			// Number of times we've crouched (for hinting)
	bool					m_bDuckToggled;		// If true, the player is crouching via a toggle

public:
	bool					GetToggledDuckState( void ) { return m_bDuckToggled; }
	void					ToggleDuck( void );
	float					GetStickDist( void );

	float					m_flForwardMove;
	float					m_flSideMove;
	int						m_nNumCrateHudHints;

private:

	// Used in test code to teleport the player to random locations in the map.
	Vector					m_vForcedOrigin;
	bool					m_bForceOrigin;	

	// Clients try to run on their own realtime clock, this is this client's clock
	CNetworkVar( int, m_nTickBase );

	bool					m_bGamePaused;
	float					m_fLastPlayerTalkTime;
	
	CNetworkVar( CBaseCombatWeaponHandle, m_hLastWeapon );

#if !defined( NO_ENTITY_PREDICTION )
	CUtlVector< CHandle< CBaseEntity > > m_SimulatedByThisPlayer;
#endif

	float					m_flOldPlayerZ;
	float					m_flOldPlayerViewOffsetZ;

	bool					m_bPlayerUnderwater;

	EHANDLE					m_hViewEntity;

	// Movement constraints
	CNetworkHandle( CBaseEntity, m_hConstraintEntity );
	CNetworkVector( m_vecConstraintCenter );
	CNetworkVar( float, m_flConstraintRadius );
	CNetworkVar( float, m_flConstraintWidth );
	CNetworkVar( float, m_flConstraintSpeedFactor );

	friend class CPlayerMove;
	friend class CPlayerClass;

	// Player name
	char					m_szNetname[MAX_PLAYER_NAME_LENGTH];

protected:
	friend class CTFGameMovementRecon;
	friend class CGameMovement;
	friend class CTFGameMovement;
	friend class CHL1GameMovement;
	friend class CCSGameMovement;	
	friend class CHL2GameMovement;
	friend class CDODGameMovement;
	
	// Accessors for gamemovement
	bool IsDucked( void ) const { return m_Local.m_bDucked; }
	bool IsDucking( void ) const { return m_Local.m_bDucking; }
	float GetStepSize( void ) const { return m_Local.m_flStepSize; }


	CNetworkVar( float,  m_flLaggedMovementValue );

	// These are generated while running usercmds, then given to UpdateVPhysicsPosition after running all queued commands.
	Vector m_vNewVPhysicsPosition;
	Vector m_vNewVPhysicsVelocity;

	// last known navigation area of player - NULL if unknown
	CNavArea *m_lastNavArea;
	CNetworkString( m_szLastPlaceName, MAX_PLACE_NAME_LENGTH );

	char m_szNetworkIDString[MAX_NETWORKID_LENGTH];
	CPlayerInfo m_PlayerInfo;

	// Texture names and surface data, used by CGameMovement
	int				m_surfaceProps;
	surfacedata_t*	m_pSurfaceData;
	float			m_surfaceFriction;
	char			m_chTextureType;
	char			m_chPreviousTextureType;	// Separate from m_chTextureType. This is cleared if the player's not on the ground.

	bool			m_bSinglePlayerGameEnding;

public:

	float  GetLaggedMovementValue( void ){ return m_flLaggedMovementValue;	}
	void   SetLaggedMovementValue( float flValue ) { m_flLaggedMovementValue = flValue;	}

	inline bool IsAutoKickDisabled( void ) const;
	inline void DisableAutoKick( bool disabled );

private:
	bool m_autoKickDisabled;
};

typedef CHandle<CBasePlayer> CBasePlayerHandle;

EXTERN_SEND_TABLE(DT_BasePlayer)



//-----------------------------------------------------------------------------
// Inline methods
//-----------------------------------------------------------------------------
inline bool CBasePlayer::IsAutoKickDisabled( void ) const
{
	return m_autoKickDisabled;
}

inline void CBasePlayer::DisableAutoKick( bool disabled )
{
	m_autoKickDisabled = disabled;
}

inline void CBasePlayer::SetAdditionalPVSOrigin( const Vector &vecOrigin ) 
{ 
	m_vecAdditionalPVSOrigin = vecOrigin; 
}

inline void CBasePlayer::SetCameraPVSOrigin( const Vector &vecOrigin ) 
{ 
	m_vecCameraPVSOrigin = vecOrigin; 
}

inline void CBasePlayer::SetMuzzleFlashTime( float flTime ) 
{ 
	m_flFlashTime = flTime; 
}

inline void CBasePlayer::SetUseEntity( CBaseEntity *pUseEntity ) 
{ 
	m_hUseEntity = pUseEntity; 
}

inline CBaseEntity *CBasePlayer::GetUseEntity() 
{ 
	return m_hUseEntity;
}

// Bot accessors...
inline void CBasePlayer::SetTimeBase( float flTimeBase ) 
{ 
	m_nTickBase = TIME_TO_TICKS( flTimeBase ); 
}

inline void CBasePlayer::SetLastUserCommand( const CUserCmd &cmd ) 
{ 
	m_LastCmd = cmd; 
}

inline CUserCmd const *CBasePlayer::GetLastUserCommand( void )
{
	return &m_LastCmd;
}

inline bool CBasePlayer::IsPredictingWeapons( void ) const 
{
	return m_bPredictWeapons;
}

inline int CBasePlayer::CurrentCommandNumber() const
{
	Assert( m_pCurrentCommand );
	return m_pCurrentCommand->command_number;
}

inline const CUserCmd *CBasePlayer::GetCurrentUserCommand() const
{
	Assert( m_pCurrentCommand );
	return m_pCurrentCommand;
}

inline IServerVehicle *CBasePlayer::GetVehicle() 
{ 
	CBaseEntity *pVehicleEnt = m_hVehicle.Get();
	return pVehicleEnt ? pVehicleEnt->GetServerVehicle() : NULL;
}

inline CBaseEntity *CBasePlayer::GetVehicleEntity() 
{ 
	return m_hVehicle.Get();
}

inline bool CBasePlayer::IsInAVehicle( void ) const 
{ 
	return ( NULL != m_hVehicle.Get() ) ? true : false; 
}

inline void CBasePlayer::SetTouchedPhysics( bool bTouch ) 
{ 
	m_touchedPhysObject = bTouch; 
}

inline bool CBasePlayer::TouchedPhysics( void )			
{ 
	return m_touchedPhysObject; 
}

//-----------------------------------------------------------------------------
// Converts an entity to a player
//-----------------------------------------------------------------------------
inline CBasePlayer *ToBasePlayer( CBaseEntity *pEntity )
{
	if ( !pEntity || !pEntity->IsPlayer() )
		return NULL;
#if _DEBUG
	return dynamic_cast<CBasePlayer *>( pEntity );
#else
	return static_cast<CBasePlayer *>( pEntity );
#endif
}

//--------------------------------------------------------------------------------------------------------------
/**
 * Iterate over all active players in the game, invoking functor on each.
 * If functor returns false, stop iteration and return false.
 */
template < typename Functor >
bool ForEachPlayer( Functor &func )
{
        for( int i=1; i<=gpGlobals->maxClients; ++i )
        {
                CBasePlayer *player = static_cast<CBasePlayer *>( UTIL_PlayerByIndex( i ) );

                if (player == NULL)
                        continue;

                if (FNullEnt( player->edict() ))
                        continue;

                if (!player->IsPlayer())
                        continue;

                if (func( player ) == false)
                        return false;
        }

        return true;
}

enum
{
	VEHICLE_ANALOG_BIAS_NONE = 0,
	VEHICLE_ANALOG_BIAS_FORWARD,
	VEHICLE_ANALOG_BIAS_REVERSE,
};

#endif // PLAYER_H
