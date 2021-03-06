#ifndef ZONEDB_H_
#define ZONEDB_H_

#include "../common/shareddb.h"
#include "../common/eq_packet_structs.h"
#include "position.h"
#include "../common/faction.h"
#include "../common/eqemu_logsys.h"

class Client;
class Corpse;
class NPC;
class Petition;
class Spawn2;
class SpawnGroupList;
class ItemInst;
class Trap;
struct CharacterEventLog_Struct;
struct Door;
struct ExtendedProfile_Struct;
struct NPCType;
struct PlayerCorpse_Struct;
struct ZonePoint;
struct npcDecayTimes_Struct;
template <class TYPE> class LinkedList;

//#include "doors.h"

struct wplist {
	int index;
	float x;
	float y;
	float z;
	int pause;
	float heading;
};

#pragma pack(1)
struct DBnpcspells_entries_Struct {
	int16	spellid;
	uint16	type;
	uint8	minlevel;
	uint8	maxlevel;
	int16	manacost;
	int32	recast_delay;
	int16	priority;
	int16	resist_adjust;
};
#pragma pack()

#pragma pack(1)
struct DBnpcspellseffects_entries_Struct {
	int16	spelleffectid;
	uint8	minlevel;
	uint8	maxlevel;
	int32	base;
	int32	limit;
	int32	max;
};
#pragma pack()

struct DBnpcspells_Struct {
	uint32	parent_list;
	uint16	attack_proc;
	uint8	proc_chance;
	uint16	range_proc;
	int16	rproc_chance;
	uint16	defensive_proc;
	int16	dproc_chance;
	uint32	numentries;
	uint32	fail_recast;
	uint32	engaged_no_sp_recast_min;
	uint32	engaged_no_sp_recast_max;
	uint8	engaged_beneficial_self_chance;
	uint8	engaged_beneficial_other_chance;
	uint8	engaged_detrimental_chance;
	uint32  pursue_no_sp_recast_min;
	uint32  pursue_no_sp_recast_max;
	uint8   pursue_detrimental_chance;
	uint32  idle_no_sp_recast_min;
	uint32  idle_no_sp_recast_max;
	uint8	idle_beneficial_chance;
	DBnpcspells_entries_Struct entries[0];
};

struct DBnpcspellseffects_Struct {
	uint32	parent_list;
	uint32	numentries;
	DBnpcspellseffects_entries_Struct entries[0];
};

struct DBTradeskillRecipe_Struct {
	SkillUseTypes tradeskill;
	int16 skill_needed;
	uint16 trivial;
	bool nofail;
	bool replace_container;
	std::vector< std::pair<uint32,uint8> > onsuccess;
	std::vector< std::pair<uint32,uint8> > onfail;
	std::vector< std::pair<uint32,uint8> > salvage;
	std::string name;
	uint8 must_learn;
	bool has_learnt;
	uint32 madecount;
	uint32 recipe_id;
	bool quest;
};

struct PetRecord {
	uint32 npc_type;	// npc_type id for the pet data to use
	bool temporary;
	int16 petpower;
	uint8 petcontrol;	// What kind of control over the pet is possible (Animation, familiar, ...)
	uint8 petnaming;		// How to name the pet (Warder, pet, random name, familiar, ...)
	bool monsterflag;	// flag for if a random monster appearance should get picked
	uint32 equipmentset;	// default equipment for the pet
};

// Actual pet info for a client.
struct PetInfo {
	uint16	SpellID;
	int16	petpower;
	uint32	HP;
	uint32	Mana;
	float	size;
	SpellBuff_Struct	Buffs[BUFF_COUNT];
	uint32	Items[EmuConstants::EQUIPMENT_SIZE];
	char	Name[64];
};

struct ZoneSpellsBlocked {
	uint32 spellid;
	int8 type;
	glm::vec3 m_Location;
	glm::vec3 m_Difference;
	char message[256];
};

struct TraderCharges_Struct {
	uint32 ItemID[80];
	int32 SerialNumber[80];
	uint32 ItemCost[80];
	int32 Charges[80];
};

class ZoneDatabase : public SharedDatabase {
	typedef std::list<ServerLootItem_Struct*> ItemList;
public:
	ZoneDatabase();
	ZoneDatabase(const char* host, const char* user, const char* passwd, const char* database,uint32 port);
	virtual ~ZoneDatabase();

	/* Objects and World Containers  */
	void	LoadWorldContainer(uint32 parentid, ItemInst* container);
	void	SaveWorldContainer(uint32 zone_id, uint32 parent_id, const ItemInst* container);
	void	DeleteWorldContainer(uint32 parent_id,uint32 zone_id);
	uint32	AddObject(uint32 type, uint32 icon, const Object_Struct& object, const ItemInst* inst);
	void	UpdateObject(uint32 id, uint32 type, uint32 icon, const Object_Struct& object, const ItemInst* inst);
	void	DeleteObject(uint32 id);
	Ground_Spawns*	LoadGroundSpawns(uint32 zone_id, int16 version, Ground_Spawns* gs);

	/* Traders  */
	void	SaveTraderItem(uint32 char_id,uint32 itemid,uint32 uniqueid, int32 charges,uint32 itemcost,uint8 slot);
	void	UpdateTraderItemCharges(int char_id, uint32 ItemInstID, int32 charges);
	void	UpdateTraderItemPrice(int CharID, uint32 ItemID, uint32 Charges, uint32 NewPrice);
	void	DeleteTraderItem(uint32 char_id);
	void	DeleteTraderItem(uint32 char_id,uint16 slot_id);

	ItemInst* LoadSingleTraderItem(uint32 char_id, int uniqueid);
	Trader_Struct* LoadTraderItem(uint32 char_id);
	TraderCharges_Struct* LoadTraderItemWithCharges(uint32 char_id);
	int8 ItemQuantityType(int16 item_id);

	/* General Character Related Stuff  */
	bool	SetServerFilters(char* name, ServerSideFilters_Struct *ssfs);
	uint32	GetServerFilters(char* name, ServerSideFilters_Struct *ssfs);

	void LogCommands(const char* char_name, const char* acct_name, float y, float x, float z, const char* command, const char* targetType, const char* target, float tar_y, float tar_x, float tar_z, uint32 zone_id, const char* zone_name);
	uint8 GetCommandAccess(const char* command);
	void SaveBuffs(Client *c);
	void LoadBuffs(Client *c);
	void LoadPetInfo(Client *c);
	void SavePetInfo(Client *c);
	void RemoveTempFactions(Client *c);

	/* Character Data Loaders  */
	bool	LoadCharacterFactionValues(uint32 character_id, faction_map & val_list);
	bool	LoadCharacterSpellBook(uint32 character_id, PlayerProfile_Struct* pp);
	bool	LoadCharacterMemmedSpells(uint32 character_id, PlayerProfile_Struct* pp);
	bool	LoadCharacterLanguages(uint32 character_id, PlayerProfile_Struct* pp);
	bool	LoadCharacterSkills(uint32 character_id, PlayerProfile_Struct* pp);
	bool	LoadCharacterData(uint32 character_id, PlayerProfile_Struct* pp, ExtendedProfile_Struct* m_epp);
	bool	LoadCharacterCurrency(uint32 character_id, PlayerProfile_Struct* pp);
	bool	LoadCharacterBindPoint(uint32 character_id, PlayerProfile_Struct* pp);
	bool	LoadCharacterMaterialColor(uint32 character_id, PlayerProfile_Struct* pp);
	bool	LoadCharacterLeadershipAA(uint32 character_id, PlayerProfile_Struct* pp);

	/* Character Data Saves  */
	bool	SaveCharacterBindPoint(uint32 character_id, uint32 zone_id, uint32 instance_id, const glm::vec4& position, uint8 is_home);
	bool	SaveCharacterCurrency(uint32 character_id, PlayerProfile_Struct* pp);
	bool	SaveCharacterData(uint32 character_id, uint32 account_id, PlayerProfile_Struct* pp, ExtendedProfile_Struct* m_epp);
	bool	SaveCharacterAA(uint32 character_id, uint32 aa_id, uint32 current_level);
	bool	SaveCharacterSpell(uint32 character_id, uint32 spell_id, uint32 slot_id);
	bool	SaveCharacterMemorizedSpell(uint32 character_id, uint32 spell_id, uint32 slot_id);
	bool	SaveCharacterMaterialColor(uint32 character_id, uint32 slot_id, uint32 color);
	bool	SaveCharacterSkill(uint32 character_id, uint32 skill_id, uint32 value);
	bool	SaveCharacterLanguage(uint32 character_id, uint32 lang_id, uint32 value);
	bool	SaveCharacterLeadershipAA(uint32 character_id, PlayerProfile_Struct* pp);
	bool	SaveCharacterConsent(uint32 character_id, char name[64]);

	/* Character Data Deletes   */
	bool	DeleteCharacterSpell(uint32 character_id, uint32 spell_id, uint32 slot_id);
	bool	DeleteCharacterMemorizedSpell(uint32 character_id, uint32 spell_id, uint32 slot_id);
	bool	DeleteCharacterAAs(uint32 character_id);
	bool	DeleteCharacterDye(uint32 character_id);
	bool	DeleteCharacterConsent(uint32 character_id, char name[64]);

	/* Character Inventory  */
	bool	NoRentExpired(const char* name);
	bool	SaveSoulboundItems(Client* client, std::list<ItemInst*>::const_iterator &start, std::list<ItemInst*>::const_iterator &end);

	/* Corpses  */
	bool		DeleteItemOffCharacterCorpse(uint32 db_id, uint32 equip_slot, uint32 item_id);
	uint32		GetCharacterCorpseItemCount(uint32 corpse_id);
	bool		LoadCharacterCorpseData(uint32 corpse_id, PlayerCorpse_Struct* pcs);
	Corpse*		LoadCharacterCorpse(uint32 player_corpse_id);
	Corpse*		SummonBuriedCharacterCorpses(uint32 char_id, uint32 dest_zoneid, uint16 dest_instanceid, const glm::vec4& position);
	Corpse*		SummonCharacterCorpse(uint32 corpse_id, uint32 char_id, uint32 dest_zoneid, uint16 dest_instanceid, const glm::vec4& position);
	void		MarkCorpseAsRezzed(uint32 dbid);
	bool		GetDecayTimes(npcDecayTimes_Struct* npcCorpseDecayTimes);
	bool		BuryCharacterCorpse(uint32 dbid);
	bool		BuryAllCharacterCorpses(uint32 charid);
	bool		DeleteCharacterCorpse(uint32 dbid);
	bool		SummonAllCharacterCorpses(uint32 char_id, uint32 dest_zoneid, uint16 dest_instanceid, const glm::vec4& position);
	bool		SummonAllGraveyardCorpses(uint32 cur_zoneid, uint32 dest_zoneid, uint16 dest_instanceid, const glm::vec4& position);
	bool		UnburyCharacterCorpse(uint32 dbid, uint32 new_zoneid, uint16 dest_instanceid, const glm::vec4& position);
	bool		LoadCharacterCorpses(uint32 iZoneID, uint16 iInstanceID);
	bool		DeleteGraveyard(uint32 zone_id, uint32 graveyard_id);
	uint32		GetCharacterCorpseDecayTimer(uint32 corpse_db_id);
	uint32		GetCharacterBuriedCorpseCount(uint32 char_id);
	uint32		SendCharacterCorpseToGraveyard(uint32 dbid, uint32 zoneid, uint16 instanceid, const glm::vec4& position);
	uint32		CreateGraveyardRecord(uint32 graveyard_zoneid, const glm::vec4& position);
	uint32		AddGraveyardIDToZone(uint32 zone_id, uint32 graveyard_id);
	uint32		SaveCharacterCorpse(uint32 charid, const char* charname, uint32 zoneid, uint16 instanceid, PlayerCorpse_Struct* dbpc, const glm::vec4& position);
	bool		SaveCharacterCorpseBackup(uint32 corpse_id, uint32 charid, const char* charname, uint32 zoneid, uint16 instanceid, PlayerCorpse_Struct* dbpc, const glm::vec4& position);
	uint32		UpdateCharacterCorpse(uint32 dbid, uint32 charid, const char* charname, uint32 zoneid, uint16 instanceid, PlayerCorpse_Struct* dbpc, const glm::vec4& position, bool rezzed = false);
	bool		UpdateCharacterCorpseBackup(uint32 dbid, uint32 charid, const char* charname, uint32 zoneid, uint16 instanceid, PlayerCorpse_Struct* dbpc, const glm::vec4& position, bool rezzed = false);
	uint32		GetFirstCorpseID(uint32 char_id);
	uint32		GetCharacterCorpseCount(uint32 char_id);
	uint32		GetCharacterCorpseID(uint32 char_id, uint8 corpse);
	uint32		GetCharacterCorpseItemAt(uint32 corpse_id, uint16 slotid);
	uint32		GetPlayerCorpseTimeLeft(uint8 corpse, uint8 type);
	bool		IsValidCorpseBackup(uint32 corpse_id);
	bool		IsValidCorpse(uint32 corpse_id);
	bool		CopyBackupCorpse(uint32 corpse_id);
	bool		IsCorpseBackupOwner(uint32 corpse_id, uint32 char_id);

	/* Faction   */
	bool		GetNPCFactionList(uint32 npcfaction_id, int32* faction_id, int32* value, uint8* temp, int32* primary_faction = 0);
	bool		GetFactionData(FactionMods* fd, uint32 class_mod, uint32 race_mod, uint32 deity_mod, int32 faction_id, uint8 texture_mod, uint8 gender_mod); //needed for factions Dec, 16 2001
	bool		GetFactionName(int32 faction_id, char* name, uint32 buflen); // needed for factions Dec, 16 2001
	bool		GetFactionIdsForNPC(uint32 nfl_id, std::list<struct NPCFaction*> *faction_list, int32* primary_faction = 0); // improve faction handling
	bool		SetCharacterFactionLevel(uint32 char_id, int32 faction_id, int32 value, uint8 temp, faction_map &val_list); // needed for factions Dec, 16 2001
	bool		LoadFactionData();

	/* AAs */
	bool	LoadAAEffects();
	bool	LoadAAEffects2();
	bool	LoadSwarmSpells();
	SendAA_Struct*	GetAASkillVars(uint32 skill_id);
	uint8	GetTotalAALevels(uint32 skill_id);
	uint32	GetMacToEmuAA(uint8 eqmacid);
	uint32	CountAAs();
	void	LoadAAs(SendAA_Struct **load);
	uint32 CountAAEffects();
	void FillAAEffects(SendAA_Struct* aa_struct);

	/* Zone related */
	bool	GetZoneCFG(uint32 zoneid, uint16 instance_id, NewZone_Struct *data, bool &can_bind, bool &can_combat, bool &can_levitate, bool &can_castoutdoor, bool &is_city, bool &is_hotzone, uint8 &zone_type, int &ruleset, char **map_filename, bool &can_bind_others, bool &skip_los);
	bool	SaveZoneCFG(uint32 zoneid, uint16 instance_id, NewZone_Struct* zd);
	bool	LoadStaticZonePoints(LinkedList<ZonePoint*>* zone_point_list,const char* zonename, uint32 version);
	bool		UpdateZoneSafeCoords(const char* zonename, const glm::vec3& location);
	uint8	GetUseCFGSafeCoords();
	int		getZoneShutDownDelay(uint32 zoneID, uint32 version);

	/* Spawns and Spawn Points  */
	bool		LoadSpawnGroups(const char* zone_name, uint16 version, SpawnGroupList* spawn_group_list);
	bool		LoadSpawnGroupsByID(int spawngroupid, SpawnGroupList* spawn_group_list);
	bool		PopulateZoneSpawnList(uint32 zoneid, LinkedList<Spawn2*> &spawn2_list, int16 version, uint32 repopdelay = 0);
	Spawn2*		LoadSpawn2(LinkedList<Spawn2*> &spawn2_list, uint32 spawn2id, uint32 timeleft);
	bool		CreateSpawn2(Client *c, uint32 spawngroup, const char* zone, const glm::vec4& position, uint32 respawn, uint32 variance, uint16 condition, int16 cond_value);
	void		UpdateRespawnTime(uint32 id, uint16 instance_id,uint32 timeleft);
	uint32		GetSpawnTimeLeft(uint32 id, uint16 instance_id);
	void		UpdateSpawn2Status(uint32 id, uint8 new_status);

	/* Grids/Paths  */
	uint32		GetFreeGrid(uint16 zoneid);
	void		DeleteGrid(Client *c, uint32 sg2, uint32 grid_num, bool grid_too, uint16 zoneid);
	void		DeleteWaypoint(Client *c, uint32 grid_num, uint32 wp_num, uint16 zoneid);
	void		AddWP(Client *c, uint32 gridid, uint32 wpnum, const glm::vec4& position, uint32 pause, uint16 zoneid);
	uint32		AddWPForSpawn(Client *c, uint32 spawn2id, const glm::vec4& position, uint32 pause, int type1, int type2, uint16 zoneid);
	void		ModifyGrid(Client *c, bool remove, uint32 id, uint8 type = 0, uint8 type2 = 0, uint16 zoneid = 0);
	void		ModifyWP(Client *c, uint32 grid_id, uint32 wp_num, const glm::vec3& location, uint32 script = 0, uint16 zoneid = 0);
	uint8		GetGridType(uint32 grid, uint32 zoneid);
	uint8		GetGridType2(uint32 grid, uint16 zoneid);
	bool		GetWaypoints(uint32 grid, uint16 zoneid, uint32 num, wplist* wp);
	void        AssignGrid(Client *client, int grid, int spawn2id);
	int			GetHighestGrid(uint32 zoneid);
	int			GetHighestWaypoint(uint32 zoneid, uint32 gridid);

	/* NPCs  */

	uint32		NPCSpawnDB(uint8 command, const char* zone, uint32 zone_version, Client *c, NPC* spawn = 0, uint32 extra = 0); // 0 = Create 1 = Add; 2 = Update; 3 = Remove; 4 = Delete
	uint32		CreateNewNPCCommand(const char* zone, uint32 zone_version, Client *client, NPC* spawn, uint32 extra);
	uint32		AddNewNPCSpawnGroupCommand(const char* zone, uint32 zone_version, Client *client, NPC* spawn, uint32 respawnTime);
	uint32		DeleteSpawnLeaveInNPCTypeTable(const char* zone, Client *client, NPC* spawn);
	uint32		DeleteSpawnRemoveFromNPCTypeTable(const char* zone, uint32 zone_version, Client *client, NPC* spawn);
	uint32		AddSpawnFromSpawnGroup(const char* zone, uint32 zone_version, Client *client, NPC* spawn, uint32 spawnGroupID);
	uint32		AddNPCTypes(const char* zone, uint32 zone_version, Client *client, NPC* spawn, uint32 spawnGroupID);
	uint32		UpdateNPCTypeAppearance(Client *client, NPC* spawn);
	bool		SetSpecialAttkFlag(uint8 id, const char* flag);
	bool		GetPetEntry(const char *pet_type, PetRecord *into);
	bool		GetPoweredPetEntry(const char *pet_type, int16 petpower, PetRecord *into);
	bool		GetBasePetItems(int32 equipmentset, uint32 *items);
	void		AddLootTableToNPC(NPC* npc, uint32 loottable_id, ItemList* itemlist, uint32* copper, uint32* silver, uint32* gold, uint32* plat);
	void		AddLootDropToNPC(NPC* npc, uint32 lootdrop_id, ItemList* itemlist, uint8 droplimit, uint8 mindrop);
	uint32		GetMaxNPCSpellsID();
	uint32		GetMaxNPCSpellsEffectsID();

	DBnpcspells_Struct*				GetNPCSpells(uint32 iDBSpellsID);
	DBnpcspellseffects_Struct*		GetNPCSpellsEffects(uint32 iDBSpellsEffectsID);
	const NPCType*					GetNPCType(uint32 id);
	NPCType*					    GetNPCTypeTemp(uint32 id);

	/* Petitions   */
	void	UpdateBug(BugStruct* bug, uint32 clienttype);
	void	UpdateFeedback(Feedback_Struct* feedback);
	void	DeletePetitionFromDB(Petition* wpet);
	void	UpdatePetitionToDB(Petition* wpet);
	void	InsertPetitionToDB(Petition* wpet);
	void	RefreshPetitionsFromDB();
	void	AddSoulMark(uint32 charid, const char* charname, const char* accname, const char* gmname, const char* gmacctname, uint32 utime, uint32 type, const char* desc);
	int		RemoveSoulMark(uint32 charid);

	/* Merchants  */
	void	SaveMerchantTemp(uint32 npcid, uint32 slot, uint32 item, uint32 charges, uint32 quantity);
	void	DeleteMerchantTemp(uint32 npcid, uint32 slot);

	/* Tradeskills  */
	bool	GetTradeRecipe(const ItemInst* container, uint8 c_type, uint32 some_id, uint32 char_id, DBTradeskillRecipe_Struct *spec);
	bool	GetTradeRecipe(uint32 recipe_id, uint8 c_type, uint32 some_id, uint32 char_id, DBTradeskillRecipe_Struct *spec);
	uint32	GetZoneForage(uint32 ZoneID, uint8 skill); /* for foraging */
	uint32	GetZoneFishing(uint32 ZoneID, uint8 skill, uint32 &npc_id, uint8 &npc_chance);
	void	UpdateRecipeMadecount(uint32 recipe_id, uint32 char_id, uint32 madecount);
	bool	EnableRecipe(uint32 recipe_id);
	bool	DisableRecipe(uint32 recipe_id);
	bool	UpdateSkillDifficulty(uint16 skillid, float difficulty);

	/*
	* Doors
	*/
	bool	DoorIsOpen(uint8 door_id,const char* zone_name);
	void	SetDoorPlace(uint8 value,uint8 door_id,const char* zone_name);
	bool	LoadDoors(int32 iDoorCount, Door *into, const char *zone_name, int16 version);
	int32	GetDoorsCount(uint32* oMaxID, const char *zone_name, int16 version);
	int32	GetDoorsCountPlusOne(const char *zone_name, int16 version);
	int32	GetDoorsDBCountPlusOne(const char *zone_name, int16 version);
	void	InsertDoor(uint32 did, uint16 ddoorid, const char* ddoor_name, const glm::vec4& position, uint8 dopentype, uint16 dguildid, uint32 dlockpick, uint32 dkeyitem, uint8 ddoor_param, uint8 dinvert, int dincline, uint16 dsize);

	/* Blocked Spells   */
	int32	GetBlockedSpellsCount(uint32 zoneid);
	bool	LoadBlockedSpells(int32 blockedSpellsCount, ZoneSpellsBlocked* into, uint32 zoneid);

	/* Traps   */
	bool	LoadTraps(const char* zonename, int16 version);
	bool	SetTrapData(Trap* trap, bool repopnow = false);

	/* Time   */
	uint32	GetZoneTZ(uint32 zoneid, uint32 version);
	bool	SetZoneTZ(uint32 zoneid, uint32 version, uint32 tz);

	/* Group   */
	void RefreshGroupFromDB(Client *c);
	uint8 GroupCount(uint32 groupid);

	/* Raid   */
	uint8 RaidGroupCount(uint32 raidid, uint32 groupid);

	/* Instancing   */
	void ListAllInstances(Client* c, uint32 charid);

	/* QGlobals   */
	void QGlobalPurge();

	/*MBMessages*/
	bool RetrieveMBMessages(uint16 category, std::vector<MBMessageRetrievalGen_Struct>& outData);
	bool PostMBMessage(uint32 charid, const char* charName, MBMessageRetrievalGen_Struct* inData);
	bool EraseMBMessage(uint32 id, uint32 charid);
	bool ViewMBMessage(uint32 id, char* outData);

	/*
		* Misc stuff.
		* PLEASE DO NOT ADD TO THIS COLLECTION OF CRAP UNLESS YOUR METHOD
		* REALLY HAS NO BETTER SECTION
	*/
	bool	logevents(const char* accountname,uint32 accountid,uint8 status,const char* charname,const char* target, const char* descriptiontype, const char* description,int event_nid);
	void	GetEventLogs(const char* name,char* target,uint32 account_id=0,uint8 eventid=0,char* detail=0,char* timestamp=0, CharacterEventLog_Struct* cel=0);
	uint32	GetKarma(uint32 acct_id);
	void	UpdateKarma(uint32 acct_id, uint32 amount);


protected:
	void ZDBInitVars();

	uint32				max_faction;
	Faction**			faction_array;
	uint32 npc_spells_maxid;
	uint32 npc_spellseffects_maxid;
	DBnpcspells_Struct** npc_spells_cache;
	bool*				npc_spells_loadtried;
	DBnpcspellseffects_Struct** npc_spellseffects_cache;
	bool*				npc_spellseffects_loadtried;
	uint8 door_isopen_array[255];
};

extern ZoneDatabase database;

#endif /*ZONEDB_H_*/

