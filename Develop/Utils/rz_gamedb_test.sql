--
-- PostgreSQL database dump
--

-- Dumped from database version 9.5.0
-- Dumped by pg_dump version 9.5.0

-- Started on 2016-01-23 12:15:30

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- TOC entry 2435 (class 1262 OID 28617)
-- Dependencies: 2434
-- Name: rz_gamedb_test; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON DATABASE rz_gamedb_test IS 'RZ_GAMEDB:
  This DB stores game related info and each world specific info.
  You need to create one game DB for every worlds you have.';


--
-- TOC entry 6 (class 2615 OID 28618)
-- Name: const_def; Type: SCHEMA; Schema: -; Owner: -
--

CREATE SCHEMA const_def;


--
-- TOC entry 2436 (class 0 OID 0)
-- Dependencies: 6
-- Name: SCHEMA const_def; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON SCHEMA const_def IS 'This schema is used to define constant data (i.e. Read-only data).';


--
-- TOC entry 209 (class 3079 OID 12355)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2438 (class 0 OID 0)
-- Dependencies: 209
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = const_def, pg_catalog;

--
-- TOC entry 651 (class 1247 OID 28620)
-- Name: rz_default_equipment_place; Type: TYPE; Schema: const_def; Owner: -
--

CREATE TYPE rz_default_equipment_place AS ENUM (
    'EQUIPMENT',
    'INVENTORY'
);


SET search_path = public, pg_catalog;

--
-- TOC entry 654 (class 1247 OID 28627)
-- Name: rz_vector_2d; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_vector_2d AS (
	x real,
	y real
);


--
-- TOC entry 657 (class 1247 OID 28630)
-- Name: rz_character_preference; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_character_preference AS (
	race smallint,
	gender smallint,
	hair smallint,
	face smallint,
	hair_color smallint,
	skin_color smallint,
	eye_color smallint,
	tattoo smallint,
	tattoo_pos rz_vector_2d,
	tattoo_scale smallint,
	tattoo_color smallint,
	makeup smallint,
	voice smallint
);


--
-- TOC entry 660 (class 1247 OID 28632)
-- Name: rz_palette_entry_type; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_palette_entry_type AS ENUM (
    'ITEM',
    'TALENT'
);


--
-- TOC entry 663 (class 1247 OID 28638)
-- Name: rz_quest_state; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_quest_state AS ENUM (
    'UNAVAILABLE',
    'IN_PROGRESS',
    'COMPLETED',
    'FAILED'
);


--
-- TOC entry 2439 (class 0 OID 0)
-- Dependencies: 663
-- Name: TYPE rz_quest_state; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TYPE rz_quest_state IS 'Enum of Quest Status used in rz_quest.
This enum is also defined as QUEST_STATE_TYPE in GameServer code.

UNAVAILABLE: Quest is invalid and not existing.
IN_PROGRESS: Player is just acquired the quest.
COMPLETED: Player has achieved the quest and have to report to quest NPC.
FAILED: Player failed the quest and just needs to be discarded by player''s operation.';


--
-- TOC entry 666 (class 1247 OID 28648)
-- Name: rz_user_item_place; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_user_item_place AS ENUM (
    'INVENTORY',
    'EQUIPMENT',
    'STORAGE'
);


--
-- TOC entry 669 (class 1247 OID 28656)
-- Name: rz_user_item_status; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_user_item_status AS ENUM (
    'CIRCULATING',
    'CONSUMED',
    'DISCARDED',
    'SOLD',
    'REMOVED',
    'BROKEN',
    'UNUSABLE',
    'DETACHED',
    'EXPIRED',
    'LOST'
);


--
-- TOC entry 2440 (class 0 OID 0)
-- Dependencies: 669
-- Name: TYPE rz_user_item_status; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TYPE rz_user_item_status IS 'This data type is used to manipulate item status that represents what happening on user-generated items.
If the status is not CIRCULATING, then the item isn''t existing anymore.

  CIRCULATING: An item is currently on user''s hand.
  CONSUMED: User has consumed an item by enchanting (e.g. Beyciums), attaching jewels (e.g. Obsidians, Jewel Dusts), 
     converting gears to costumes (e.g. Costume Potions), healings (e.g. Health Potions), non-reusable mounts (e.g. Adrenaline Krorong Summoning Rings),
     entering into dynamic fields (e.g. Altering Eyes) etc...
  DISCARDED: User is dropped the item (by Drag''n Drop) from inventory or storage. or user is deleted a item-attached mail without taking the items.
  SOLD: User sold an item to NPC shop.
  REMOVED: The item was attached in mail, but the mail got deleted by system due to period expiration of kept.
  BROKEN: User is failed at enchanting an item and broke.
  UNUSABLE: The item was non-repairable and durability fallen to zero.
  DETACHED: The item was jewel (like Pyroxenes or Obsidians) and has been attached into gears. but user destroyed the jewel using Jewel Remover.
  EXPIRED: An item was available only for limited duration (non-permanent) and the period of use is expired.
  LOST: The item was on user''s hand, but it''s lost because user deleted the character.';


--
-- TOC entry 672 (class 1247 OID 28679)
-- Name: rz_vector_3d; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_vector_3d AS (
	x real,
	y real,
	z real
);


SET search_path = const_def, pg_catalog;

--
-- TOC entry 213 (class 1255 OID 28680)
-- Name: rz_default_equipment_generate(); Type: FUNCTION; Schema: const_def; Owner: -
--

CREATE FUNCTION rz_default_equipment_generate() RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
  i integer;
  
BEGIN
  TRUNCATE const_def.rz_default_equipment;

  CREATE TEMP TABLE eq (
    style_id smallint NOT NULL,
    item_id integer NOT NULL,
    slot_num smallint NOT NULL,
    PRIMARY KEY (style_id, slot_num)
  ) ON COMMIT DROP;

  -- armor equipment: common for all styles.
  FOREACH i IN ARRAY '{1,2,3,5,6}'::integer[] LOOP
    INSERT INTO eq VALUES (i, 9712, 4), -- Litner Adventurer's Tunic
                          (i, 9713, 5), -- Litner Adventurer's Trousers
                          (i, 9714, 2), -- Litner Adventurer's Gloves
                          (i, 9715, 3); -- Litner Adventurer's Shoes
  END LOOP;

  -- defender weapon (style id 1)
  INSERT INTO eq
  SELECT 1, * FROM (VALUES (1, 11), -- Standard Sword
                           (7, 10), -- Standard Shield
                           (2, 13), -- Standard Mace
                           (7, 12)) -- Standard Shield
  AS w;
  
  -- berserker weapon (style id 2)
  INSERT INTO eq
  SELECT 2, * FROM (VALUES (4, 11), -- Standard Greatsword
                           (8, 13)) -- Standard Warhammer
  AS w;
  
  -- assassin weapon (style id 3)
  INSERT INTO eq
  SELECT 3, * FROM (VALUES (3, 11)) -- Standard Dagger
  AS w;
  
  -- sorcerer weapon (style id 5)
  INSERT INTO eq
  SELECT 5, * FROM (VALUES (5, 11), -- Standard Staff
                           (1, 13), -- Standard Sword
                           (7, 12)) -- Standard Shield
  AS w;
  
  -- cleric weapon (style id 6)
  INSERT INTO eq
  SELECT 6, * FROM (VALUES (5, 11), -- Standard Staff
                           (2, 13), -- Standard Mace
                           (7, 12)) -- Standard Shield
  AS w;

  INSERT INTO const_def.rz_default_equipment (style_id, item_id, slot_num, place)
    SELECT *, 'EQUIPMENT' FROM eq;

  RETURN (SELECT COUNT(*) FROM const_def.rz_default_equipment);
END;$$;


--
-- TOC entry 2441 (class 0 OID 0)
-- Dependencies: 213
-- Name: FUNCTION rz_default_equipment_generate(); Type: COMMENT; Schema: const_def; Owner: -
--

COMMENT ON FUNCTION rz_default_equipment_generate() IS 'This function inserts pre-defined default equipments into rz_default_equipment table.
* Warning: this function will delete all previous entries from the table.

Generated row count will return as return value.';


--
-- TOC entry 214 (class 1255 OID 28681)
-- Name: rz_default_talent_generate(); Type: FUNCTION; Schema: const_def; Owner: -
--

CREATE FUNCTION rz_default_talent_generate() RETURNS integer
    LANGUAGE sql
    AS $$TRUNCATE const_def.rz_default_talent;

INSERT INTO const_def.rz_default_talent
  VALUES
    -- Defender: Rush 1
    (1, 0, 10701),
    -- Berserker: Neutralize 1
    (2, 0, 20401),
    -- Assassin: Cleave 1
    (3, 0, 33900),
    -- Sorcerer: Flame Arrow 1
    (5, 0, 50501),
    -- Cleric: Hammer of Light 1
    (6, 0, 60801);

SELECT COUNT(*) FROM const_def.rz_default_talent;$$;


--
-- TOC entry 227 (class 1255 OID 28682)
-- Name: rz_xitem_set(xml); Type: FUNCTION; Schema: const_def; Owner: -
--

CREATE FUNCTION rz_xitem_set(p_xitem_xml xml) RETURNS integer
    LANGUAGE sql
    AS $$TRUNCATE const_def.rz_xitem;

INSERT INTO const_def.rz_xitem (
  id,
  name,
  tier,
  "desc",
  contents_tag,
  mesh_name,
  node_name,
  node_name2,
  mesh_path,
  parent_name,
  parent_name2,
  texcolor,
  "Item_sound",
  type,
  condition,
  sex,
  slot,
  weapon_type,
  "DamageAttrib",
  searchable,
  buying_price,
  selling_price,
  repair_grade,
  stack_amount,
  max_durability,
  min_damage,
  max_damage,
  magic_min_damage,
  magic_max_damage,
  icon,
  "EnchantEffectList",
  "CostumeID",
  "CostumeItemDyeable",
  sub_slot,
  armor_type,
  "AP",
  "MagicAP",
  tradable,
  sellable,
  discardable,
  "unique",
  repairable,
  "EquipEnchantSlot",
  "ModSTR",
  "ModINT",
  "ModCON",
  "ModDEX",
  "ModCHA",
  parent_name_left,
  mesh_name_by_sex,
  validation,
  claim_required,
  "Dyeable",
  "ModHPRegen",
  "ModENRegen",
  "ModSTARegen",
  "ModHPMax",
  "ModENMax",
  "ModSTAMax",
  item_level,
  "Enchantable",
  "ProcEffectBuffId",
  quest_related,
  usable_type,
  usable_param,
  usable_consume,
  "MessagePopup",
  "EnchantCategory",
  "EnchantRate",
  "DropPenalty",
  "DescParam",
  qpvp_related,
  volatile,
  "ExpirationPeriod",
  gather_type,
  "ModDefSlashAmp",
  "ModDefBluntAmp",
  "ModDefPierceAmp",
  "ModMeleeAtkAmp",
  "ModRangeAtkAmp",
  "ModMagicAtkAmp",
  "ModDefHolyAmp",
  "ModMoveSpeed",
  "DyeColor",
  "ModPVPAP",
  "ModPVPDamage",
  "ModCriticalPhysic",
  "ModCriticalMagic",
  "ModCriticalAmp",
  "DecoEffectFemale",
  "DecoEffectMale",
  housing_npc,
  "UsagePeriod",
  "Cashitem",
  "GemEnchantBuff",
  "EnchantAllowEquipments",
  "EnchantAllowWeapons",
  "EnchantAllowArmors",
  "EnchantLimitLevel",
  "EnchantLimitWeapons",
  "EnchantLevel",
  "FireMinDamage",
  "FireMaxDamage",
  "FireMagicMinDamage",
  "FireMagicMaxDamage",
  "ColdMinDamage",
  "ColdMaxDamage",
  "ColdMagicMinDamage",
  "ColdMagicMaxDamage",
  "PoisonMinDamage",
  "PoisonMaxDamage",
  "PoisonMagicMinDamage",
  "PoisonMagicMaxDamage",
  "LightningMinDamage",
  "LightningMaxDamage",
  "LightningMagicMinDamage",
  "LightningMagicMaxDamage",
  "HolyMinDamage",
  "HolyMaxDamage",
  "HolyMagicMinDamage",
  "HolyMagicMaxDamage",
  "UnholyMinDamage",
  "UnholyMaxDamage",
  "UnholyMagicMinDamage",
  "UnholyMagicMaxDamage",
  "ModFireAP",
  "ModFireMagicAP",
  "ModColdAP",
  "ModColdMagicAP",
  "ModLightningAP",
  "ModLightningMagicAP",
  "ModPoisonAP",
  "ModPoisonMagicAP",
  "ModHolyAP",
  "ModHolyMagicAP",
  "ModUnholyAP",
  "ModUnholyMagicAP",
  "EnchantLimitEquipments"
) SELECT
  (xpath('/ITEM/@id', xi))[1]::text::integer,
  (xpath('/ITEM/@name', xi))[1]::text,
  (xpath('/ITEM/@tier', xi))[1]::text,
  (xpath('/ITEM/@desc', xi))[1]::text,
  (xpath('/ITEM/@contents_tag', xi))[1]::text,
  (xpath('/ITEM/@mesh_name', xi))[1]::text,
  (xpath('/ITEM/@node_name', xi))[1]::text,
  (xpath('/ITEM/@node_name2', xi))[1]::text,
  (xpath('/ITEM/@mesh_path', xi))[1]::text,
  (xpath('/ITEM/@parent_name', xi))[1]::text,
  (xpath('/ITEM/@parent_name2', xi))[1]::text,
  (xpath('/ITEM/@texcolor', xi))[1]::text,
  (xpath('/ITEM/@Item_sound', xi))[1]::text,
  (xpath('/ITEM/@type', xi))[1]::text,
  (xpath('/ITEM/@condition', xi))[1]::text::integer,
  (xpath('/ITEM/@sex', xi))[1]::text,
  (xpath('/ITEM/@slot', xi))[1]::text,
  (xpath('/ITEM/@weapon_type', xi))[1]::text,
  (xpath('/ITEM/@DamageAttrib', xi))[1]::text,
  (xpath('/ITEM/@searchable', xi))[1]::text::boolean,
  (xpath('/ITEM/@buying_price', xi))[1]::text::integer,
  (xpath('/ITEM/@selling_price', xi))[1]::text::integer,
  (xpath('/ITEM/@repair_grade', xi))[1]::text::integer,
  (xpath('/ITEM/@stack_amount', xi))[1]::text::integer,
  (xpath('/ITEM/@max_durability', xi))[1]::text::integer,
  (xpath('/ITEM/@min_damage', xi))[1]::text::integer,
  (xpath('/ITEM/@max_damage', xi))[1]::text::integer,
  (xpath('/ITEM/@magic_min_damage', xi))[1]::text::integer,
  (xpath('/ITEM/@magic_max_damage', xi))[1]::text::integer,
  (xpath('/ITEM/@icon', xi))[1]::text,
  (xpath('/ITEM/@EnchantEffectList', xi))[1]::text,
  (xpath('/ITEM/@CostumeID', xi))[1]::text::integer,
  (xpath('/ITEM/@CostumeItemDyeable', xi))[1]::text::boolean,
  (xpath('/ITEM/@sub_slot', xi))[1]::text,
  (xpath('/ITEM/@armor_type', xi))[1]::text,
  (xpath('/ITEM/@AP', xi))[1]::text::integer,
  (xpath('/ITEM/@MagicAP', xi))[1]::text::integer,
  (xpath('/ITEM/@tradable', xi))[1]::text::boolean,
  (xpath('/ITEM/@sellable', xi))[1]::text::boolean,
  (xpath('/ITEM/@discardable', xi))[1]::text::boolean,
  (xpath('/ITEM/@unique', xi))[1]::text::boolean,
  (xpath('/ITEM/@repairable', xi))[1]::text::boolean,
  (xpath('/ITEM/@EquipEnchantSlot', xi))[1]::text,
  (xpath('/ITEM/@ModSTR', xi))[1]::text::integer,
  (xpath('/ITEM/@ModINT', xi))[1]::text::integer,
  (xpath('/ITEM/@ModCON', xi))[1]::text::integer,
  (xpath('/ITEM/@ModDEX', xi))[1]::text::integer,
  (xpath('/ITEM/@ModCHA', xi))[1]::text::integer,
  (xpath('/ITEM/@parent_name_left', xi))[1]::text,
  (xpath('/ITEM/@mesh_name_by_sex', xi))[1]::text::boolean,
  (xpath('/ITEM/@validation', xi))[1]::text::boolean,
  (xpath('/ITEM/@claim_required', xi))[1]::text::boolean,
  (xpath('/ITEM/@Dyeable', xi))[1]::text::boolean,
  (xpath('/ITEM/@ModHPRegen', xi))[1]::text::integer,
  (xpath('/ITEM/@ModENRegen', xi))[1]::text::integer,
  (xpath('/ITEM/@ModSTARegen', xi))[1]::text::integer,
  (xpath('/ITEM/@ModHPMax', xi))[1]::text::integer,
  (xpath('/ITEM/@ModENMax', xi))[1]::text::integer,
  (xpath('/ITEM/@ModSTAMax', xi))[1]::text::integer,
  (xpath('/ITEM/@item_level', xi))[1]::text::integer,
  (xpath('/ITEM/@Enchantable', xi))[1]::text::boolean,
  (xpath('/ITEM/@ProcEffectBuffId', xi))[1]::text::integer,
  (xpath('/ITEM/@quest_related', xi))[1]::text::boolean,
  (xpath('/ITEM/@usable_type', xi))[1]::text,
  (xpath('/ITEM/@usable_param', xi))[1]::text,
  (xpath('/ITEM/@usable_consume', xi))[1]::text::boolean,
  (xpath('/ITEM/@MessagePopup', xi))[1]::text::boolean,
  (xpath('/ITEM/@EnchantCategory', xi))[1]::text,
  (xpath('/ITEM/@EnchantRate', xi))[1]::text::real,
  (xpath('/ITEM/@DropPenalty', xi))[1]::text::integer,
  (xpath('/ITEM/@DescParam', xi))[1]::text,
  (xpath('/ITEM/@qpvp_related', xi))[1]::text::boolean,
  (xpath('/ITEM/@volatile', xi))[1]::text::boolean,
  (xpath('/ITEM/@ExpirationPeriod', xi))[1]::text::integer,
  (xpath('/ITEM/@gather_type', xi))[1]::text,
  (xpath('/ITEM/@ModDefSlashAmp', xi))[1]::text::real,
  (xpath('/ITEM/@ModDefBluntAmp', xi))[1]::text::real,
  (xpath('/ITEM/@ModDefPierceAmp', xi))[1]::text::real,
  (xpath('/ITEM/@ModMeleeAtkAmp', xi))[1]::text::real,
  (xpath('/ITEM/@ModRangeAtkAmp', xi))[1]::text::real,
  (xpath('/ITEM/@ModMagicAtkAmp', xi))[1]::text::real,
  (xpath('/ITEM/@ModDefHolyAmp', xi))[1]::text::real,
  (xpath('/ITEM/@ModMoveSpeed', xi))[1]::text::real,
  (xpath('/ITEM/@DyeColor', xi))[1]::text,
  (xpath('/ITEM/@ModPVPAP', xi))[1]::text::real,
  (xpath('/ITEM/@ModPVPDamage', xi))[1]::text::real,
  (xpath('/ITEM/@ModCriticalPhysic', xi))[1]::text::integer,
  (xpath('/ITEM/@ModCriticalMagic', xi))[1]::text::integer,
  (xpath('/ITEM/@ModCriticalAmp', xi))[1]::text::real,
  (xpath('/ITEM/@DecoEffectFemale', xi))[1]::text,
  (xpath('/ITEM/@DecoEffectMale', xi))[1]::text,
  (xpath('/ITEM/@housing_npc', xi))[1]::text::integer,
  (xpath('/ITEM/@UsagePeriod', xi))[1]::text::integer,
  (xpath('/ITEM/@Cashitem', xi))[1]::text::boolean,
  (xpath('/ITEM/@GemEnchantBuff', xi))[1]::text::integer,
  (xpath('/ITEM/@EnchantAllowEquipments', xi))[1]::text,
  (xpath('/ITEM/@EnchantAllowWeapons', xi))[1]::text,
  (xpath('/ITEM/@EnchantAllowArmors', xi))[1]::text,
  (xpath('/ITEM/@EnchantLimitLevel', xi))[1]::text::integer,
  (xpath('/ITEM/@EnchantLimitWeapons', xi))[1]::text,
  (xpath('/ITEM/@EnchantLevel', xi))[1]::text,
  (xpath('/ITEM/@FireMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@FireMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@FireMagicMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@FireMagicMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@ColdMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@ColdMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@ColdMagicMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@ColdMagicMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@PoisonMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@PoisonMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@PoisonMagicMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@PoisonMagicMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@LightningMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@LightningMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@LightningMagicMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@LightningMagicMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@HolyMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@HolyMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@HolyMagicMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@HolyMagicMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@UnholyMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@UnholyMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@UnholyMagicMinDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@UnholyMagicMaxDamage', xi))[1]::text::integer,
  (xpath('/ITEM/@ModFireAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModFireMagicAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModColdAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModColdMagicAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModLightningAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModLightningMagicAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModPoisonAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModPoisonMagicAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModHolyAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModHolyMagicAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModUnholyAP', xi))[1]::text::integer,
  (xpath('/ITEM/@ModUnholyMagicAP', xi))[1]::text::integer,
  (xpath('/ITEM/@EnchantLimitEquipments', xi))[1]::text
FROM unnest(xpath('/maiet/ITEM', p_xitem_xml)) AS xi;

SELECT COUNT(*) FROM const_def.rz_xitem;$$;


--
-- TOC entry 2442 (class 0 OID 0)
-- Dependencies: 227
-- Name: FUNCTION rz_xitem_set(p_xitem_xml xml); Type: COMMENT; Schema: const_def; Owner: -
--

COMMENT ON FUNCTION rz_xitem_set(p_xitem_xml xml) IS 'This function parses xitem.xml passed as input parameter and stores into rz_xitem table (previous item entries will be deleted).
Returns a number of inserted rows as return value.';


SET search_path = public, pg_catalog;

--
-- TOC entry 295 (class 1255 OID 29142)
-- Name: rz_buff_remain_effect_get(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_buff_remain_effect_get(p_char_sn bigint, OUT "BUFF_ID" integer, OUT "STACK_COUNT" smallint, OUT "CRIT_PERCENT" real, OUT "CRIT_APPLY_RATE" real, OUT "MIN_DAMAGE" integer, OUT "MAX_DAMAGE" integer, OUT "MIN_HEAL_AMT" integer, OUT "MAX_HEAL_AMT" integer, OUT "MIN_ATTR_DAMAGE" integer, OUT "MAX_ATTR_DAMAGE" integer, OUT "REMAIN_TIME" real) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 100
    AS $$SELECT buff_id,
       stack_count,
       critical_percent,
       critical_apply_rate,
       min_damage,
       max_damage,
       min_heal,
       max_heal,
       min_attribute_damage,
       max_attribute_damage,
       (CASE WHEN fixed_duration THEN EXTRACT(EPOCH FROM ((added_time + remained_duration) - CURRENT_TIMESTAMP))
             ELSE EXTRACT(EPOCH FROM remained_duration) END)::real
  FROM rz_buff
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 296 (class 1255 OID 29141)
-- Name: rz_buff_remain_effect_update_all(bigint, jsonb); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_buff_remain_effect_update_all(p_char_sn bigint, p_json jsonb) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE js jsonb;
BEGIN
  DELETE FROM rz_buff WHERE char_sn = p_char_sn;
  
  FOR i IN 1 .. jsonb_array_length(p_json) LOOP
    js := p_json->(i - 1);
    
    INSERT INTO rz_buff (char_sn,
                         buff_id,
                         stack_count,
                         critical_percent,
                         critical_apply_rate,
                         min_damage,
                         max_damage,
                         min_heal,
                         max_heal,
                         min_attribute_damage,
                         max_attribute_damage,
                         remained_duration,
                         fixed_duration)
    VALUES (p_char_sn,
            (js->>'BUFF_ID')::integer,
            (js->>'STACK_COUNT')::smallint,
            (js->>'CRIT_PERCENT')::real,
            (js->>'CRIT_APPLY_RATE')::real,
            (js->>'MIN_DAMAGE')::integer,
            (js->>'MAX_DAMAGE')::integer,
            (js->>'MIN_HEAL_AMT')::integer,
            (js->>'MAX_HEAL_AMT')::integer,
            (js->>'MIN_ATTR_DAMAGE')::integer,
            (js->>'MAX_ATTR_DAMAGE')::integer,
            (js->>'REMAIN_TIME' || ' seconds')::interval, 
            (js->>'FIXED_DURATION')::boolean);
  END LOOP;
END;$$;


--
-- TOC entry 2443 (class 0 OID 0)
-- Dependencies: 296
-- Name: FUNCTION rz_buff_remain_effect_update_all(p_char_sn bigint, p_json jsonb); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_buff_remain_effect_update_all(p_char_sn bigint, p_json jsonb) IS 'Input JSON syntax:
  [
    {
      "BUFF_ID": <Buff ID>,
      "STACK_COUNT": <Number of Stacks>,
      "CRIT_PERCENT": <Pre-calculated Critical Percent for this Buff>,
      "CRIT_APPLY_RATE": <Pre-calculated Critical Damage Apply Rate>,
      "MIN_DAMAGE": <Pre-calculated Min Damage>,
      "MAX_DAMAGE": <Pre-calculated Max Damage>,
      "MIN_HEAL_AMT": <Pre-calculated Min Heal Amount>,
      "MAX_HEAL_AMT": <Pre-calculated Max Heal Amount>,
      "MIN_ATTR_DAMAGE": <Pre-calculated Min Attribute Damage>,
      "MAX_ATTR_DAMAGE": <Pre-calculated Max Attribute Damage>,
      "REMAIN_TIME": <Remaining Duration>,
      "FIXED_DURATION": <TRUE to consume remained duration regardless of in-game, FALSE otherwise.>
    }
    ...
  ]';


--
-- TOC entry 229 (class 1255 OID 28686)
-- Name: rz_char_cleanup(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_cleanup() RETURNS bigint
    LANGUAGE sql
    AS $$WITH dc AS (DELETE FROM rz_character WHERE delete_time IS NOT NULL RETURNING 1)
SELECT COUNT(*) FROM dc;$$;


--
-- TOC entry 2444 (class 0 OID 0)
-- Dependencies: 229
-- Name: FUNCTION rz_char_cleanup(); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_cleanup() IS 'This is maintenance function to cleanup all deleted characters (and associated) info.
(Deleted characters will stays alive on rz_character table with "name" column set to NULL, among other datas like item, quest, talent etc existing.)

These datas are still useful to track players or gather statistics, but executing this function is also an option to free your disk space, or improve performance.
Processed characters count is returned as a return value.

THIS OPERATION CANNOT BE REVERTED!!!';


--
-- TOC entry 230 (class 1255 OID 28687)
-- Name: rz_char_delete(bigint, boolean); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_delete(p_char_sn bigint, p_immediate boolean DEFAULT true) RETURNS integer
    LANGUAGE plpgsql
    AS $$BEGIN
  UPDATE rz_character SET delete_mark_time = CURRENT_TIMESTAMP WHERE char_sn = p_char_sn;

  IF p_immediate THEN
    UPDATE rz_character SET delete_time = CURRENT_TIMESTAMP WHERE char_sn = p_char_sn;
  END IF;

  RETURN 0;
END;$$;


--
-- TOC entry 2445 (class 0 OID 0)
-- Dependencies: 230
-- Name: FUNCTION rz_char_delete(p_char_sn bigint, p_immediate boolean); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_delete(p_char_sn bigint, p_immediate boolean) IS 'ToDo: delete char from guild, friend list of others.
ToDo 2: return non-zero value for no existing char.

Unless TRUE is passed to p_immediate, this function only mark a character as target of deletion, and the character name cannot be reused until the character is completely deleted (Use rz_char_delete_marked() function to do that).
This feature would protect you from ''pretend another player'' with deleted name, or ''reserve and sell the name'' business.

This function returns 0 on success.';


--
-- TOC entry 231 (class 1255 OID 28688)
-- Name: rz_char_delete_by_index(bigint, integer, boolean); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_delete_by_index(p_accn_sn bigint, p_index integer, p_immediate boolean DEFAULT true) RETURNS integer
    LANGUAGE sql
    AS $$WITH ci AS (
  SELECT row_number() OVER(ORDER BY char_sn) AS i,
         char_sn
    FROM rz_character
   WHERE accn_sn          =  p_accn_sn
     AND delete_mark_time IS NULL
) SELECT * FROM rz_char_delete((SELECT char_sn FROM ci WHERE i = p_index + 1),
                               p_immediate);$$;


--
-- TOC entry 2446 (class 0 OID 0)
-- Dependencies: 231
-- Name: FUNCTION rz_char_delete_by_index(p_accn_sn bigint, p_index integer, p_immediate boolean); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_delete_by_index(p_accn_sn bigint, p_index integer, p_immediate boolean) IS 'This function is called by LoginServer to delete a character, but with character''s index instead of char_sn.';


--
-- TOC entry 232 (class 1255 OID 28689)
-- Name: rz_char_delete_marked(interval); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_delete_marked(p_older_than interval) RETURNS bigint
    LANGUAGE plpgsql
    AS $$DECLARE
  v_affected integer;
  
BEGIN
  CREATE TEMP TABLE c (id bigint PRIMARY KEY) ON COMMIT DROP;
  
  INSERT INTO c (id)
    SELECT char_sn FROM rz_character
     WHERE delete_mark_time IS NOT NULL
       AND delete_time      IS NULL
       AND (delete_mark_time + p_older_than) <= CURRENT_TIMESTAMP;

  UPDATE rz_character
     SET delete_time = CURRENT_TIMESTAMP
   WHERE char_sn IN (SELECT id FROM c);

  GET DIAGNOSTICS v_affected = ROW_COUNT;
  RETURN v_affected;
END;$$;


--
-- TOC entry 2447 (class 0 OID 0)
-- Dependencies: 232
-- Name: FUNCTION rz_char_delete_marked(p_older_than interval); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_delete_marked(p_older_than interval) IS 'This function completely deletes deletion-marked characters.
Processed characters count will be returned as return value.

Call this function regularly and let users create a character again with deleted name.';


--
-- TOC entry 233 (class 1255 OID 28690)
-- Name: rz_char_enter_field(bigint, integer, smallint, integer, integer, integer, real, real, real, real, real, real); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_enter_field(p_char_sn bigint, p_exp integer, p_level smallint, p_money integer, p_play_time integer, p_shared_field_id integer, p_pos_x real, p_pos_y real, p_pos_z real, p_dir_x real, p_dir_y real, p_dir_z real) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_valid_character
   SET   exp             = p_exp,
         level           = p_level,
         money           = p_money,
         play_time       = (p_play_time || ' seconds')::interval,
         shared_field_id = p_shared_field_id,
         pos.x           = p_pos_x,
         pos.y           = p_pos_y,
         pos.z           = p_pos_z,
         dir.x           = p_dir_x,
         dir.y           = p_dir_y,
         dir.z           = p_dir_z
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 234 (class 1255 OID 28691)
-- Name: rz_char_get_cutscene(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_cutscene(p_char_sn bigint, OUT "CUTSCN_ID" integer) RETURNS SETOF integer
    LANGUAGE sql STABLE ROWS 100
    AS $$SELECT cutscn_id FROM rz_cutscene WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 235 (class 1255 OID 28692)
-- Name: rz_char_get_emblem(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_emblem(p_char_sn bigint, OUT "EMBLEM_ID" integer, OUT "SELECTED" boolean) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 500
    AS $$SELECT emblem_id, selected FROM rz_emblem WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 236 (class 1255 OID 28693)
-- Name: rz_char_get_equipment_info(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_equipment_info(p_accn_sn bigint, OUT "CHAR_SN" bigint, OUT "SLOT_TYPE" integer, OUT "SLOT_ID" smallint, OUT "ITEM_ID" integer, OUT "COLOR" integer, OUT "ENCH_ITEM_ID_1" integer, OUT "ENCH_GRADE" smallint) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 500
    AS $$SELECT iv.owner_sn,
       iv.slot_type,
       iv.slot_num,
       ui.item_id,
       ui.color,
       COALESCE(ge.attached_item_id[1], 0),
       ui.enchant_grade
  FROM rz_belongings AS iv
    INNER JOIN      rz_valid_character  AS ch ON ch.accn_sn     = p_accn_sn
                                             AND ch.char_sn     = iv.owner_sn
    INNER JOIN      rz_user_item        AS ui ON ui.item_sn     = iv.item_sn
    LEFT OUTER JOIN rz_gem_enchant      AS ge ON ge.gem_ench_id = ui.gem_ench_id
 WHERE iv.slot_type = rz_user_item_place_to_slot_type('EQUIPMENT');$$;


--
-- TOC entry 237 (class 1255 OID 28694)
-- Name: rz_char_get_faction(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_faction(p_char_sn bigint, OUT "FACT_ID" integer, OUT "VAL" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 100
    AS $$SELECT fact_id, val FROM rz_faction WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 238 (class 1255 OID 28695)
-- Name: rz_char_get_guidebook(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_guidebook(p_char_sn bigint, OUT "BOOK_ID" integer) RETURNS SETOF integer
    LANGUAGE sql STABLE ROWS 50
    AS $$SELECT book_id FROM rz_guidebook WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 240 (class 1255 OID 28696)
-- Name: rz_char_get_info(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_info(p_char_sn bigint, OUT "CHAR_SN" bigint, OUT "NAME" text, OUT "RACE" smallint, OUT "SEX" smallint, OUT "FEAT_HAIR" smallint, OUT "FEAT_FACE" smallint, OUT "FEAT_HAIR_COLOR" smallint, OUT "FEAT_SKIN_COLOR" smallint, OUT "FEAT_EYE_COLOR" smallint, OUT "TATOO" smallint, OUT "TATOO_POS_X" smallint, OUT "TATOO_POS_Y" smallint, OUT "TATOO_SCALE" smallint, OUT "TATOO_COLOR" smallint, OUT "MAKEUP" smallint, OUT "VOICE" smallint, OUT "LEV" smallint, OUT "EXP" integer, OUT "MONEY" integer, OUT "STORAGE_MONEY" integer, OUT "HP" integer, OUT "EN" integer, OUT "STA" integer, OUT "REMAIN_TP" smallint, OUT "REMAIN_TP_2ND" smallint, OUT "REMAIN_TP_PVP" smallint, OUT "WEAPON_SET" smallint, OUT "PRI_PALETTE_ID" smallint, OUT "SEC_PALETTE_ID" smallint, OUT "ACT_PALETTE_ID" smallint, OUT "FATIGUE" smallint, OUT "INN_ID" integer, OUT "INN_ROOM_ID" integer, OUT "GUILD_SN" bigint, OUT "GUILD_GRADE" smallint, OUT "POS_X" real, OUT "POS_Y" real, OUT "POS_Z" real, OUT "DIR_X" real, OUT "DIR_Y" real, OUT "DIR_Z" real, OUT "SHR_FIELD_ID" integer, OUT "ENT_POS_X" real, OUT "ENT_POS_Y" real, OUT "ENT_POS_Z" real, OUT "DYN_FIELD_GROUP_UID" bigint, OUT "DYN_FIELD_ID" integer, OUT "CHK_POINT_FIELD_ID" integer, OUT "CHK_POINT_MKR_ID" integer, OUT "PARTY_UID" bigint, OUT "ARENA_UID" bigint, OUT "ARENA_TEAM_UID" bigint, OUT "CHAR_PTM" integer, OUT "PLAYER_GRADE" smallint, OUT "ELAPS_DEAD_TIME" integer, OUT "BLANK_TIME" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 1
    AS $$SELECT char_sn,
       name,
       (preference).race,
       (preference).gender,
       (preference).hair,
       (preference).face, 
       (preference).hair_color,
       (preference).skin_color,
       (preference).eye_color, 
       (preference).tattoo,
       (preference).tattoo_pos.x::smallint,
       (preference).tattoo_pos.y::smallint,
       (preference).tattoo_scale,
       (preference).tattoo_color,
       (preference).makeup,
       (preference).voice,
       level,
       exp,
       money,
       storage_money,
       health,
       energy,
       stamina, 
       remain_tp[1],
       remain_tp[2],
       remain_tp[3],
       weapon_set,
       primary_palette_id,
       secondary_palette_id,
       active_palette_id,
       fatigue,
       inn_id,
       inn_room_id,
       0::bigint /*GUILD_SN*/,
       0::smallint /*GUILD_GRADE*/,
       (pos).x,
       (pos).y,
       (pos).z,
       (dir).x,
       (dir).y,
       (dir).z,
       shared_field_id, 
       (entrance_pos).x,
       (entrance_pos).y,
       (entrance_pos).z,
       dynamic_field_group_uid,
       dynamic_field_id,
       checkpoint_field_id,
       checkpoint_marker_id,
       party_uid, arena_uid,
       arena_team_uid, 
       (EXTRACT(EPOCH FROM play_time))::integer,
       player_grade,
       (EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - dead_time)))::integer, 
       (EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - disconn_time)))::integer
  FROM rz_valid_character
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 2448 (class 0 OID 0)
-- Dependencies: 240
-- Name: FUNCTION rz_char_get_info(p_char_sn bigint, OUT "CHAR_SN" bigint, OUT "NAME" text, OUT "RACE" smallint, OUT "SEX" smallint, OUT "FEAT_HAIR" smallint, OUT "FEAT_FACE" smallint, OUT "FEAT_HAIR_COLOR" smallint, OUT "FEAT_SKIN_COLOR" smallint, OUT "FEAT_EYE_COLOR" smallint, OUT "TATOO" smallint, OUT "TATOO_POS_X" smallint, OUT "TATOO_POS_Y" smallint, OUT "TATOO_SCALE" smallint, OUT "TATOO_COLOR" smallint, OUT "MAKEUP" smallint, OUT "VOICE" smallint, OUT "LEV" smallint, OUT "EXP" integer, OUT "MONEY" integer, OUT "STORAGE_MONEY" integer, OUT "HP" integer, OUT "EN" integer, OUT "STA" integer, OUT "REMAIN_TP" smallint, OUT "REMAIN_TP_2ND" smallint, OUT "REMAIN_TP_PVP" smallint, OUT "WEAPON_SET" smallint, OUT "PRI_PALETTE_ID" smallint, OUT "SEC_PALETTE_ID" smallint, OUT "ACT_PALETTE_ID" smallint, OUT "FATIGUE" smallint, OUT "INN_ID" integer, OUT "INN_ROOM_ID" integer, OUT "GUILD_SN" bigint, OUT "GUILD_GRADE" smallint, OUT "POS_X" real, OUT "POS_Y" real, OUT "POS_Z" real, OUT "DIR_X" real, OUT "DIR_Y" real, OUT "DIR_Z" real, OUT "SHR_FIELD_ID" integer, OUT "ENT_POS_X" real, OUT "ENT_POS_Y" real, OUT "ENT_POS_Z" real, OUT "DYN_FIELD_GROUP_UID" bigint, OUT "DYN_FIELD_ID" integer, OUT "CHK_POINT_FIELD_ID" integer, OUT "CHK_POINT_MKR_ID" integer, OUT "PARTY_UID" bigint, OUT "ARENA_UID" bigint, OUT "ARENA_TEAM_UID" bigint, OUT "CHAR_PTM" integer, OUT "PLAYER_GRADE" smallint, OUT "ELAPS_DEAD_TIME" integer, OUT "BLANK_TIME" integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_get_info(p_char_sn bigint, OUT "CHAR_SN" bigint, OUT "NAME" text, OUT "RACE" smallint, OUT "SEX" smallint, OUT "FEAT_HAIR" smallint, OUT "FEAT_FACE" smallint, OUT "FEAT_HAIR_COLOR" smallint, OUT "FEAT_SKIN_COLOR" smallint, OUT "FEAT_EYE_COLOR" smallint, OUT "TATOO" smallint, OUT "TATOO_POS_X" smallint, OUT "TATOO_POS_Y" smallint, OUT "TATOO_SCALE" smallint, OUT "TATOO_COLOR" smallint, OUT "MAKEUP" smallint, OUT "VOICE" smallint, OUT "LEV" smallint, OUT "EXP" integer, OUT "MONEY" integer, OUT "STORAGE_MONEY" integer, OUT "HP" integer, OUT "EN" integer, OUT "STA" integer, OUT "REMAIN_TP" smallint, OUT "REMAIN_TP_2ND" smallint, OUT "REMAIN_TP_PVP" smallint, OUT "WEAPON_SET" smallint, OUT "PRI_PALETTE_ID" smallint, OUT "SEC_PALETTE_ID" smallint, OUT "ACT_PALETTE_ID" smallint, OUT "FATIGUE" smallint, OUT "INN_ID" integer, OUT "INN_ROOM_ID" integer, OUT "GUILD_SN" bigint, OUT "GUILD_GRADE" smallint, OUT "POS_X" real, OUT "POS_Y" real, OUT "POS_Z" real, OUT "DIR_X" real, OUT "DIR_Y" real, OUT "DIR_Z" real, OUT "SHR_FIELD_ID" integer, OUT "ENT_POS_X" real, OUT "ENT_POS_Y" real, OUT "ENT_POS_Z" real, OUT "DYN_FIELD_GROUP_UID" bigint, OUT "DYN_FIELD_ID" integer, OUT "CHK_POINT_FIELD_ID" integer, OUT "CHK_POINT_MKR_ID" integer, OUT "PARTY_UID" bigint, OUT "ARENA_UID" bigint, OUT "ARENA_TEAM_UID" bigint, OUT "CHAR_PTM" integer, OUT "PLAYER_GRADE" smallint, OUT "ELAPS_DEAD_TIME" integer, OUT "BLANK_TIME" integer) IS 'TODO: guild info...';


--
-- TOC entry 241 (class 1255 OID 28697)
-- Name: rz_char_get_item(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_item(p_char_sn bigint, OUT "ITEM_SN" bigint, OUT "SLOT_TYPE" integer, OUT "SLOT_ID" smallint, OUT "ITEM_ID" integer, OUT "STACK_AMT" smallint, OUT "DURA" smallint, OUT "MAX_DURA" smallint, OUT "COLOR" integer, OUT "ENCH_GRADE" smallint, OUT "CLAIMED" boolean, OUT "CHAR_PTM" integer, OUT "PERIOD" integer, OUT "USAGE_PERIOD" integer, OUT "REG_DATE" timestamp with time zone, OUT "EFF_START_DATE" timestamp with time zone, OUT "EFF_END_DATE" timestamp with time zone, OUT "EXPI_TIME" integer, OUT "ENCH_ITEM_ID_1" integer, OUT "ENCH_ITEM_ID_2" integer, OUT "ENCH_ITEM_ID_3" integer, OUT "ENCH_ITEM_ID_4" integer, OUT "ENCH_ITEM_ID_5" integer, OUT "ENCH_ITEM_ID_6" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 500
    AS $$SELECT bl.item_sn,
       bl.slot_type,
       bl.slot_num,
       ui.item_id,
       ui.stack_amount,
       ui.durability,
       ui.max_durability,
       ui.color,
       ui.enchant_grade,
       ui.tied,
       0 /*CHAR_PTM*/,
       CASE WHEN ui.expire_in IS NULL THEN 0 ELSE 1 END,
       EXTRACT(EPOCH FROM ui.expire_in)::integer,
       ui.reg_time,
       ui.activate_time,
       (ui.activate_time + ui.expire_in),
       EXTRACT(EPOCH FROM ((ui.activate_time + ui.expire_in) - CURRENT_TIMESTAMP))::integer,
       COALESCE(ge.attached_item_id[1], 0),
       COALESCE(ge.attached_item_id[2], 0),
       COALESCE(ge.attached_item_id[3], 0),
       COALESCE(ge.attached_item_id[4], 0),
       COALESCE(ge.attached_item_id[5], 0),
       COALESCE(ge.attached_item_id[6], 0)
  FROM rz_belongings AS bl
    INNER JOIN      rz_user_item   AS ui ON ui.item_sn     = bl.item_sn
    LEFT OUTER JOIN rz_gem_enchant AS ge ON ge.gem_ench_id = ui.gem_ench_id
 WHERE bl.owner_sn = p_char_sn;$$;


--
-- TOC entry 242 (class 1255 OID 28698)
-- Name: rz_char_get_palette(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_palette(p_char_sn bigint, OUT "PALETTE_SET" smallint, OUT "SLOT_ID" smallint, OUT "ID" integer, OUT "ID_TYPE" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 250
    AS $$SELECT palette_set,
       regist_place,
       entry_id,
       (CASE entry_type
         WHEN 'ITEM'   THEN 1
         WHEN 'TALENT' THEN 2
        ELSE 0 END)
  FROM rz_palette
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 243 (class 1255 OID 28699)
-- Name: rz_char_get_quest(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_quest(p_char_sn bigint, OUT "SLOT_ID" smallint, OUT "QUEST_ID" integer, OUT "VAR" integer, OUT "STATE" integer, OUT "ELAPS_TIME" integer, OUT "ACCP_DATE" timestamp with time zone, OUT "EXPI_DATE" timestamp with time zone, OUT "OBJ1_PROG" integer, OUT "OBJ2_PROG" integer, OUT "OBJ3_PROG" integer, OUT "OBJ4_PROG" integer, OUT "OBJ5_PROG" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 50
    AS $$SELECT entry_num,
       quest_id,
       variable,
       rz_quest_state_to_num(state),
       EXTRACT(EPOCH FROM (CURRENT_TIMESTAMP - accept_time))::integer,
       accept_time,
       (accept_time + time_limit),
       objective_progress[1],
       objective_progress[2],
       objective_progress[3],
       objective_progress[4],
       objective_progress[5]
  FROM rz_quest
 WHERE char_sn  = p_char_sn
   AND state   <> 'UNAVAILABLE'
   AND (time_limit IS NULL OR (accept_time + time_limit) > CURRENT_TIMESTAMP);$$;


--
-- TOC entry 244 (class 1255 OID 28700)
-- Name: rz_char_get_quest_history(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_quest_history(p_char_sn bigint, OUT "QUEST_ID" integer) RETURNS SETOF integer
    LANGUAGE sql STABLE
    AS $$SELECT quest_id FROM rz_quest_history
 WHERE char_sn = p_char_sn
   AND (   next_reset_time IS NULL
        OR next_reset_time < CURRENT_TIMESTAMP);$$;


--
-- TOC entry 245 (class 1255 OID 28701)
-- Name: rz_char_get_recipe(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_recipe(p_char_sn bigint, OUT "RECP_ID" integer) RETURNS SETOF integer
    LANGUAGE sql STABLE ROWS 50
    AS $$SELECT recp_id FROM rz_recipe WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 246 (class 1255 OID 28702)
-- Name: rz_char_get_simple_info(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_simple_info(p_accn_sn bigint, OUT "CHAR_SN" bigint, OUT "NAME" text, OUT "LEV" smallint, OUT "RACE" smallint, OUT "SEX" smallint, OUT "CHAR_PTM" integer, OUT "PLAYER_GRADE" smallint, OUT "FEAT_HAIR" smallint, OUT "FEAT_FACE" smallint, OUT "FEAT_HAIR_COLOR" smallint, OUT "FEAT_SKIN_COLOR" smallint, OUT "FEAT_EYE_COLOR" smallint, OUT "TATOO" smallint, OUT "TATOO_POS_X" smallint, OUT "TATOO_POS_Y" smallint, OUT "TATOO_SCALE" smallint, OUT "TATOO_COLOR" smallint, OUT "MAKEUP" smallint, OUT "VOICE" smallint, OUT "SHR_FIELD_ID" integer, OUT "DYN_FIELD_GROUP_UID" bigint, OUT "DYN_FIELD_ID" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 5
    AS $$SELECT char_sn,
       name,
       level,
       (preference).race,
       (preference).gender,
       EXTRACT(EPOCH FROM play_time)::integer,
       player_grade,
       (preference).hair,
       (preference).face,
       (preference).hair_color,
       (preference).skin_color,
       (preference).eye_color,
       (preference).tattoo,
       (preference).tattoo_pos.x::smallint,
       (preference).tattoo_pos.y::smallint,
       (preference).tattoo_scale,
       (preference).tattoo_color,
       (preference).makeup,
       (preference).voice,
       shared_field_id,
       dynamic_field_group_uid,
       dynamic_field_id
  FROM rz_valid_character
 WHERE accn_sn = p_accn_sn
   ORDER BY char_sn;$$;


--
-- TOC entry 2449 (class 0 OID 0)
-- Dependencies: 246
-- Name: FUNCTION rz_char_get_simple_info(p_accn_sn bigint, OUT "CHAR_SN" bigint, OUT "NAME" text, OUT "LEV" smallint, OUT "RACE" smallint, OUT "SEX" smallint, OUT "CHAR_PTM" integer, OUT "PLAYER_GRADE" smallint, OUT "FEAT_HAIR" smallint, OUT "FEAT_FACE" smallint, OUT "FEAT_HAIR_COLOR" smallint, OUT "FEAT_SKIN_COLOR" smallint, OUT "FEAT_EYE_COLOR" smallint, OUT "TATOO" smallint, OUT "TATOO_POS_X" smallint, OUT "TATOO_POS_Y" smallint, OUT "TATOO_SCALE" smallint, OUT "TATOO_COLOR" smallint, OUT "MAKEUP" smallint, OUT "VOICE" smallint, OUT "SHR_FIELD_ID" integer, OUT "DYN_FIELD_GROUP_UID" bigint, OUT "DYN_FIELD_ID" integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_get_simple_info(p_accn_sn bigint, OUT "CHAR_SN" bigint, OUT "NAME" text, OUT "LEV" smallint, OUT "RACE" smallint, OUT "SEX" smallint, OUT "CHAR_PTM" integer, OUT "PLAYER_GRADE" smallint, OUT "FEAT_HAIR" smallint, OUT "FEAT_FACE" smallint, OUT "FEAT_HAIR_COLOR" smallint, OUT "FEAT_SKIN_COLOR" smallint, OUT "FEAT_EYE_COLOR" smallint, OUT "TATOO" smallint, OUT "TATOO_POS_X" smallint, OUT "TATOO_POS_Y" smallint, OUT "TATOO_SCALE" smallint, OUT "TATOO_COLOR" smallint, OUT "MAKEUP" smallint, OUT "VOICE" smallint, OUT "SHR_FIELD_ID" integer, OUT "DYN_FIELD_GROUP_UID" bigint, OUT "DYN_FIELD_ID" integer) IS 'Original MSSQL RZ_GAMEDB (BR 2013) requires WORLD_ID as input parameter.
but I have removed that parameter because it''s not needed.
(My game DB is supposed to made for every each worlds. So WORLD_ID will always be same value.)';


--
-- TOC entry 247 (class 1255 OID 28703)
-- Name: rz_char_get_talent(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_get_talent(p_char_sn bigint, OUT "TALENT_SET" smallint, OUT "TALENT_ID" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 100
    AS $$SELECT talent_set, talent_id FROM rz_talent WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 249 (class 1255 OID 28704)
-- Name: rz_char_insert(bigint, text, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, smallint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_insert(p_accn_sn bigint, p_name text, p_race smallint, p_gender smallint, p_hair smallint, p_face smallint, p_hair_color smallint, p_skin_color smallint, p_eye_color smallint, p_tattoo smallint, p_tattoo_pos_x smallint, p_tattoo_pos_y smallint, p_tattoo_scale smallint, p_tattoo_color smallint, p_makeup smallint, p_voice smallint, p_style_id smallint, p_armor_color_num integer, OUT "CHAR_SN" bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $$DECLARE
  v_char_sn bigint;
  
BEGIN
  IF ((p_name IS NULL) OR (strpos(p_name, ' ') <> 0)) THEN
    SELECT -2 INTO "CHAR_SN";
    RETURN;
  END IF;
  
  -- search for existing character names including deletion-marked chars.
  IF EXISTS (SELECT 1 FROM rz_character WHERE name ILIKE p_name) THEN
    SELECT -1 INTO "CHAR_SN";
    RETURN;
  END IF;

  INSERT INTO rz_character (accn_sn,
                            name,
                            style,
                            preference.race,
                            preference.gender,
                            preference.hair,
                            preference.face,
                            preference.hair_color,
                            preference.skin_color,
                            preference.eye_color,
                            preference.tattoo,
                            preference.tattoo_pos.x,
                            preference.tattoo_pos.y,
                            preference.tattoo_scale,
                            preference.tattoo_color,
                            preference.makeup,
                            preference.voice,
                            health,
                            energy,
                            stamina)
  VALUES (p_accn_sn,
          p_name,
          p_style_id,
          p_race,
          p_gender,
          p_hair,
          p_face,
          p_hair_color,
          p_skin_color,
          p_eye_color,
          p_tattoo,
          p_tattoo_pos_x::real,
          p_tattoo_pos_y::real,
          p_tattoo_scale,
          p_tattoo_color,
          p_makeup,
          p_voice,
          300,
          200,
          100)
  RETURNING char_sn INTO STRICT v_char_sn;

  PERFORM rz_char_insert_default_equipment(v_char_sn, p_style_id, p_armor_color_num);
  PERFORM rz_char_insert_default_talent(v_char_sn, p_style_id);

  -- 2015/12/22: add tutorial quest as completed state because I didn't implement tutorial system.
  INSERT INTO rz_quest (  char_sn, entry_num, quest_id,       state, objective_progress)
                VALUES (v_char_sn,         0,   109064, 'COMPLETED',      '{1,0,0,0,0}');

  -- 2015/01/08: FOR TESTING PURPOSE: give some advantages for newely made character.
  PERFORM rz_test_char_property_set(v_char_sn);
  
  SELECT v_char_sn INTO "CHAR_SN";
END;$$;


--
-- TOC entry 2450 (class 0 OID 0)
-- Dependencies: 249
-- Name: FUNCTION rz_char_insert(p_accn_sn bigint, p_name text, p_race smallint, p_gender smallint, p_hair smallint, p_face smallint, p_hair_color smallint, p_skin_color smallint, p_eye_color smallint, p_tattoo smallint, p_tattoo_pos_x smallint, p_tattoo_pos_y smallint, p_tattoo_scale smallint, p_tattoo_color smallint, p_makeup smallint, p_voice smallint, p_style_id smallint, p_armor_color_num integer, OUT "CHAR_SN" bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_insert(p_accn_sn bigint, p_name text, p_race smallint, p_gender smallint, p_hair smallint, p_face smallint, p_hair_color smallint, p_skin_color smallint, p_eye_color smallint, p_tattoo smallint, p_tattoo_pos_x smallint, p_tattoo_pos_y smallint, p_tattoo_scale smallint, p_tattoo_color smallint, p_makeup smallint, p_voice smallint, p_style_id smallint, p_armor_color_num integer, OUT "CHAR_SN" bigint) IS 'This function returns -1 CHAR_SN on character name duplication, -2 on invalid character name (i.e. containing spaces) passed.';


--
-- TOC entry 250 (class 1255 OID 28705)
-- Name: rz_char_insert_default_equipment(bigint, smallint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_insert_default_equipment(p_char_sn bigint, p_style_id smallint, p_arm_color integer) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE
  v_item_sn        bigint;
  v_item_id        integer;
  v_slot_num       smallint;
  v_stack_amount   smallint;
  v_max_durability smallint;
  v_item_place     const_def.rz_default_equipment_place;
  v_color          integer;
  
BEGIN
  FOR         v_item_id,  v_slot_num,  v_stack_amount,  v_max_durability,  v_item_place
   IN SELECT de.item_id, de.slot_num,       de.amount, xi.max_durability,      de.place
        FROM       const_def.rz_default_equipment AS de
        INNER JOIN const_def.rz_xitem             AS xi ON xi.id = de.item_id
       WHERE de.style_id = p_style_id
  LOOP
    SELECT
        (CASE WHEN v_slot_num IN (10, 11, 12, 13)
              THEN -1 -- no dye for weapons.
         ELSE p_arm_color END)::integer
      INTO v_color;
    
    INSERT INTO rz_user_item
                  (  item_id,  owner_sn,   stack_amount,       durability,   max_durability,   color, tied)
         VALUES   (v_item_id, p_char_sn, v_stack_amount, v_max_durability, v_max_durability, v_color, TRUE)
      RETURNING item_sn INTO STRICT v_item_sn;

    IF v_item_place = 'EQUIPMENT' THEN
      INSERT INTO rz_equipment
                  ( owner_sn,   slot_num,   item_sn)
           VALUES (p_char_sn, v_slot_num, v_item_sn);
    ELSEIF v_item_place = 'INVENTORY' THEN
      INSERT INTO rz_inventory
                  ( owner_sn,   slot_num,   item_sn)
           VALUES (p_char_sn, v_slot_num, v_item_sn);
    ELSE
      RAISE EXCEPTION 'rz_char_insert_default_equipment() - unknown item place: %.', v_item_place::text;
    END IF;
  END LOOP;
END;$$;


--
-- TOC entry 2451 (class 0 OID 0)
-- Dependencies: 250
-- Name: FUNCTION rz_char_insert_default_equipment(p_char_sn bigint, p_style_id smallint, p_arm_color integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_insert_default_equipment(p_char_sn bigint, p_style_id smallint, p_arm_color integer) IS 'This function automatically called inside rz_char_insert() function.';


--
-- TOC entry 251 (class 1255 OID 28706)
-- Name: rz_char_insert_default_talent(bigint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_insert_default_talent(p_char_sn bigint, p_style_id smallint) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_talent (char_sn,
                       talent_set,
                       talent_id)
  SELECT p_char_sn,
         1,
         talent_id
    FROM const_def.rz_default_talent
   WHERE style_id = p_style_id;

INSERT INTO rz_palette (char_sn,
                        palette_set,
                        regist_place,
                        entry_id,
                        entry_type)
  SELECT p_char_sn,
         0,
         palette_place,
         talent_id,
         'TALENT'
    FROM const_def.rz_default_talent
   WHERE style_id = p_style_id;$$;


--
-- TOC entry 252 (class 1255 OID 28707)
-- Name: rz_char_level_up(bigint, integer, smallint, integer, smallint, real, real, real); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_level_up(p_char_sn bigint, p_exp integer, p_level smallint, p_money integer, p_increase_tp smallint, p_pos_x real, p_pos_y real, p_pos_z real) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_valid_character
   SET   exp          = p_exp,
         level        = p_level,
         money        = p_money,
         remain_tp[1] = remain_tp[1] + p_increase_tp,
         remain_tp[2] = remain_tp[2] + p_increase_tp,
         remain_tp[3] = remain_tp[3] + p_increase_tp,
         pos.x        = p_pos_x,
         pos.y        = p_pos_y,
         pos.z        = p_pos_z
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 253 (class 1255 OID 28708)
-- Name: rz_char_logout(bigint, integer, smallint, integer, integer, integer, integer, smallint, smallint, smallint, smallint, smallint, real, real, real, real, real, real, integer, real, real, real, bigint, integer, integer, integer, bigint, bigint, bigint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_logout(p_char_sn bigint, p_exp integer, p_level smallint, p_money integer, p_health integer, p_energy integer, p_stamina integer, p_fatigue smallint, p_weapon_set smallint, p_active_palette_id smallint, p_primary_palette_id smallint, p_secondary_palette_id smallint, p_pos_x real, p_pos_y real, p_pos_z real, p_dir_x real, p_dir_y real, p_dir_z real, p_shared_field_id integer, p_entrance_pos_x real, p_entrance_pos_y real, p_entrance_pos_z real, p_dynamic_field_group_uid bigint, p_dynamic_field_id integer, p_checkpoint_field_id integer, p_checkpoint_marker_id integer, p_party_uid bigint, p_arena_uid bigint, p_arena_team_uid bigint, p_play_time_sec integer, p_elapsed_dead_time_sec integer) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_valid_character
   SET   exp                     = p_exp,
         level                   = p_level,
         money                   = p_money,
         health                  = p_health,
         energy                  = p_energy,
         stamina                 = p_stamina,
         fatigue                 = p_fatigue,
         weapon_set              = p_weapon_set,
         active_palette_id       = p_active_palette_id,
         primary_palette_id      = p_primary_palette_id,
         secondary_palette_id    = p_secondary_palette_id,
         pos.x                   = p_pos_x,
         pos.y                   = p_pos_y,
         pos.z                   = p_pos_z,
         dir.x                   = p_dir_x,
         dir.y                   = p_dir_y,
         dir.z                   = p_dir_z,
         shared_field_id         = p_shared_field_id,
         entrance_pos.x          = p_entrance_pos_x,
         entrance_pos.y          = p_entrance_pos_y,
         entrance_pos.z          = p_entrance_pos_z,
         dynamic_field_group_uid = p_dynamic_field_group_uid,
         dynamic_field_id        = p_dynamic_field_id,
         checkpoint_field_id     = p_checkpoint_field_id,
         checkpoint_marker_id    = p_checkpoint_marker_id,
         party_uid               = p_party_uid,
         arena_uid               = p_arena_uid,
         arena_team_uid          = p_arena_team_uid,
         play_time               = (p_play_time_sec || ' seconds')::interval,
         dead_time               = (CASE WHEN p_elapsed_dead_time_sec = -1 THEN NULL
                                         ELSE (CURRENT_TIMESTAMP -
                                               (p_elapsed_dead_time_sec || ' seconds')::interval)
                                    END)
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 254 (class 1255 OID 28709)
-- Name: rz_char_on_delete(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_on_delete() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
  IF NEW.delete_mark_time IS NULL THEN
    RAISE EXCEPTION
      'rz_char_on_delete(): you have to fill delete_mark_date before deleting this character. (CHAR_SN:%)',
        NEW.char_sn;
  END IF;

  UPDATE rz_user_item SET status = 'LOST' WHERE owner_sn = NEW.char_sn;

  SELECT NULL, NEW.name INTO NEW.name, NEW.delete_name;
  RETURN NEW;
END;$$;


--
-- TOC entry 2452 (class 0 OID 0)
-- Dependencies: 254
-- Name: FUNCTION rz_char_on_delete(); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_on_delete() IS 'This function will be called automatically by rz_character trigger when before character is going to be deleted completely.';


--
-- TOC entry 255 (class 1255 OID 28710)
-- Name: rz_char_on_mark_delete(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_on_mark_delete() RETURNS trigger
    LANGUAGE plpgsql
    AS $$BEGIN
  IF NEW.delete_time IS NOT NULL THEN
    RAISE EXCEPTION
      'rz_char_on_mark_delete(): you have to unset delete_time before restoring this character. (CHAR_SN:%)',
        NEW.char_sn;
  END IF;
  
  /* ToDo: check if the character is guild leader or not. */

  /* ToDo: delete from guild. */

  /* ToDo: delete from friend list of others. */
  
  RETURN NEW;
END;$$;


--
-- TOC entry 2453 (class 0 OID 0)
-- Dependencies: 255
-- Name: FUNCTION rz_char_on_mark_delete(); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_char_on_mark_delete() IS 'This function will be called automatically by rz_character trigger when before character is marked as target of deletion.';


--
-- TOC entry 256 (class 1255 OID 28711)
-- Name: rz_char_sync(bigint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_sync(p_char_sn bigint, p_exp integer, p_money integer) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_valid_character SET exp = p_exp, money = p_money WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 285 (class 1255 OID 29064)
-- Name: rz_char_update_exp(bigint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_char_update_exp(p_char_sn bigint, p_exp integer, p_money integer) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_character SET exp = p_exp, money = p_money WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 290 (class 1255 OID 29069)
-- Name: rz_cutscene_insert(bigint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_cutscene_insert(p_char_sn bigint, p_cutscn_id integer) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_cutscene (char_sn, cutscn_id) VALUES (p_char_sn, p_cutscn_id)
  ON CONFLICT DO NOTHING;$$;


--
-- TOC entry 288 (class 1255 OID 29066)
-- Name: rz_faction_set(bigint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_faction_set(p_char_sn bigint, p_fact_id integer, p_val integer) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_faction (char_sn, fact_id, val) VALUES (p_char_sn, p_fact_id, p_val)
  ON CONFLICT (char_sn, fact_id) DO UPDATE SET val = EXCLUDED.val;$$;


--
-- TOC entry 286 (class 1255 OID 29065)
-- Name: rz_gm_char_update_player_grade(bigint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_gm_char_update_player_grade(p_char_sn bigint, p_player_grade smallint) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_character SET player_grade = p_player_grade WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 283 (class 1255 OID 29062)
-- Name: rz_gm_char_update_tp(bigint, smallint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_gm_char_update_tp(p_char_sn bigint, p_talent_set smallint, p_remain_tp smallint) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_character SET remain_tp[p_talent_set] = p_remain_tp WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 284 (class 1255 OID 29063)
-- Name: rz_inn_room_regist(bigint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_inn_room_regist(p_char_sn bigint, p_room_id integer) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_character SET inn_room_id = p_room_id WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 239 (class 1255 OID 28712)
-- Name: rz_item_check_durability(smallint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_check_durability(p_durability smallint, p_max_durability smallint) RETURNS boolean
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (p_max_durability IS NULL
        OR (    p_durability IS NOT NULL
            AND p_durability > 0)
       )::boolean;$$;


--
-- TOC entry 2454 (class 0 OID 0)
-- Dependencies: 239
-- Name: FUNCTION rz_item_check_durability(p_durability smallint, p_max_durability smallint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_check_durability(p_durability smallint, p_max_durability smallint) IS 'Returns FALSE for broken item durability.';


--
-- TOC entry 292 (class 1255 OID 29061)
-- Name: rz_item_delete(bigint, integer, smallint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_delete(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_exp integer DEFAULT NULL::integer, p_inc_money integer DEFAULT NULL::integer) RETURNS void
    LANGUAGE plpgsql
    AS $_$DECLARE
  v_item_sn bigint;
  
BEGIN
  BEGIN
    EXECUTE format('DELETE FROM %I WHERE owner_sn = $1 AND slot_num = $2 RETURNING item_sn;',
                    rz_user_item_slot_type_to_table_name(p_slot_type))
    INTO STRICT v_item_sn
      USING p_owner_sn, p_slot_num;
      
  EXCEPTION WHEN NO_DATA_FOUND THEN
    RAISE EXCEPTION
      'rz_item_delete(): the user does not hold an item in slot. (CHAR_SN:%, SLOT_TYPE:%, SLOT_ID:%)',
        p_owner_sn, p_slot_type, p_slot_num;
  END;
    
  UPDATE rz_user_item SET owner_sn = NULL, status = 'DISCARDED' WHERE item_sn = v_item_sn;

  IF p_exp IS NOT NULL AND p_inc_money IS NOT NULL THEN
    UPDATE rz_character
       SET exp   = p_exp,
           money = money + p_inc_money
     WHERE char_sn = p_owner_sn;
  END IF;
END;$_$;


--
-- TOC entry 2455 (class 0 OID 0)
-- Dependencies: 292
-- Name: FUNCTION rz_item_delete(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_exp integer, p_inc_money integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_delete(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_exp integer, p_inc_money integer) IS 'TODO: item delete reason

This function deletes an item from user''s inventory.
ITEM_SN will selected by depending on SLOT_TYPE and SLOT_ID input parameter.';


--
-- TOC entry 257 (class 1255 OID 28714)
-- Name: rz_item_equip(bigint, smallint, smallint, smallint, boolean, smallint, smallint, smallint, bigint, bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_equip(p_owner_sn bigint, p_eq_from_slot_num smallint, p_eq_to_slot_num smallint, p_eq_item_durability smallint, p_eq_item_tied boolean, p_uneq_from_slot_num smallint DEFAULT '-1'::integer, p_uneq_to_slot_num smallint DEFAULT '-1'::integer, p_uneq_item_durability smallint DEFAULT 0, p_eq_item_sn bigint DEFAULT NULL::bigint, p_uneq_item_sn bigint DEFAULT NULL::bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE
  c_rz_inventory CONSTANT integer NOT NULL DEFAULT rz_user_item_place_to_slot_type('INVENTORY');
  c_rz_equipment CONSTANT integer NOT NULL DEFAULT rz_user_item_place_to_slot_type('EQUIPMENT');

  v_eq_item_sn    bigint DEFAULT CASE WHEN p_eq_item_sn    = 0 THEN NULL ELSE p_eq_item_sn    END;
  v_uneq_item_sn  bigint DEFAULT CASE WHEN p_uneq_item_sn  = 0 THEN NULL ELSE p_uneq_item_sn  END;
  
BEGIN
  PERFORM rz_item_move(p_owner_sn,
                       c_rz_inventory,
                       p_eq_from_slot_num,
                       p_owner_sn,
                       c_rz_equipment,
                       p_eq_to_slot_num,
                       p_eq_item_sn,
                       NULL);
                        
  UPDATE rz_user_item
     SET durability = p_eq_item_durability,
         tied       = p_eq_item_tied
   WHERE item_sn = (SELECT CASE WHEN p_eq_item_sn IS NULL
                                 THEN rz_item_sn_get(p_owner_sn, c_rz_equipment, p_eq_to_slot_num)
                                 ELSE p_eq_item_sn END);
                                   
  -- 255: SH_ITEM_SLOT::ITEMSLOT_NONE
  IF p_uneq_from_slot_num <> 255 THEN
    PERFORM rz_item_move(p_owner_sn,
                         c_rz_equipment,
                         p_uneq_from_slot_num,
                         p_owner_sn,
                         c_rz_inventory,
                         p_uneq_to_slot_num,
                         p_uneq_item_sn,
                         NULL);
                         
    UPDATE rz_user_item
       SET durability = p_uneq_item_durability
     WHERE item_sn = (SELECT CASE WHEN p_uneq_item_sn IS NULL
                                   THEN rz_item_sn_get(p_owner_sn, c_rz_inventory, p_uneq_to_slot_num)
                                   ELSE p_uneq_item_sn END);
  END IF;
END;$$;


--
-- TOC entry 2456 (class 0 OID 0)
-- Dependencies: 257
-- Name: FUNCTION rz_item_equip(p_owner_sn bigint, p_eq_from_slot_num smallint, p_eq_to_slot_num smallint, p_eq_item_durability smallint, p_eq_item_tied boolean, p_uneq_from_slot_num smallint, p_uneq_to_slot_num smallint, p_uneq_item_durability smallint, p_eq_item_sn bigint, p_uneq_item_sn bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_equip(p_owner_sn bigint, p_eq_from_slot_num smallint, p_eq_to_slot_num smallint, p_eq_item_durability smallint, p_eq_item_tied boolean, p_uneq_from_slot_num smallint, p_uneq_to_slot_num smallint, p_uneq_item_durability smallint, p_eq_item_sn bigint, p_uneq_item_sn bigint) IS 'Equip the item (or swap the item).

Moves item to equipment slot from inventory slot. If destination equipment slot already contains some item, that will be swapped with source inventory item and destination equipment item.
If equipping item is two-slot item (e.g. 2H Sword, Robe etc) and separated slot item (e.g. 1H Sword, Plate Body and Leg) already exists on equipment slot, you have to supply the additional separated item info as unequipment item info parameter.

You can pass p_eq_item_sn and/or p_uneq_item_sn with NULL to this function. In such case, the item_sn will be queried by item slot number.
However, if the item_sn is already known, you can pass your item_sn here for slightly performance improvement.';


--
-- TOC entry 291 (class 1255 OID 29058)
-- Name: rz_item_insert(bigint, integer, smallint, integer, smallint, smallint, smallint, integer, boolean, boolean, interval, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_insert(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_item_id integer, p_stack_amount smallint, p_durability smallint, p_max_durability smallint, p_color integer, p_tied boolean, p_has_period boolean, p_expire_in interval, p_exp integer DEFAULT NULL::integer, p_inc_money integer DEFAULT NULL::integer, OUT "ITEM_SN" bigint) RETURNS bigint
    LANGUAGE plpgsql
    AS $_$DECLARE
  v_item_sn bigint;
    
BEGIN
  INSERT INTO rz_user_item (item_id,
                            owner_sn,
                            stack_amount,
                            durability,
                            max_durability,
                            color,
                            tied,
                            activate_time,
                            expire_in)
  VALUES (p_item_id,
          p_owner_sn,
          p_stack_amount,
          p_durability,
          p_max_durability,
          p_color,
          p_tied,
          CASE WHEN p_tied THEN CURRENT_TIMESTAMP
               ELSE NULL END,
          CASE WHEN p_has_period AND p_tied THEN p_expire_in
               ELSE NULL END)
  RETURNING item_sn INTO STRICT v_item_sn;
  
  EXECUTE format('INSERT INTO %I (owner_sn, slot_num, item_sn) VALUES ($1, $2, $3)',
                  rz_user_item_slot_type_to_table_name(p_slot_type))
    USING p_owner_sn, p_slot_num, v_item_sn;

  SELECT v_item_sn INTO "ITEM_SN";

  -- extra process: update char exp and money, if both wasn't NULL.
  IF p_exp IS NOT NULL AND p_inc_money IS NOT NULL THEN
    UPDATE rz_character
       SET exp   = p_exp,
           money = money + p_inc_money
     WHERE char_sn = p_owner_sn;
  END IF;
END;$_$;


--
-- TOC entry 293 (class 1255 OID 29070)
-- Name: rz_item_move(bigint, integer, smallint, bigint, integer, smallint, bigint, bigint, smallint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_move(p_from_owner_sn bigint, p_from_slot_type integer, p_from_slot_num smallint, p_to_owner_sn bigint, p_to_slot_type integer, p_to_slot_num smallint, p_from_item_sn bigint DEFAULT NULL::bigint, p_to_item_sn bigint DEFAULT NULL::bigint, p_from_item_inc_amount smallint DEFAULT NULL::smallint, p_to_item_inc_amount smallint DEFAULT NULL::smallint) RETURNS void
    LANGUAGE plpgsql
    AS $_$DECLARE
  v_from_item_sn bigint DEFAULT CASE WHEN p_from_item_sn = 0 THEN NULL ELSE p_from_item_sn END;
  v_to_item_sn   bigint DEFAULT CASE WHEN p_to_item_sn   = 0 THEN NULL ELSE p_to_item_sn   END;
  
BEGIN
  IF v_from_item_sn IS NULL THEN
    BEGIN
      SELECT rz_item_sn_get(p_from_owner_sn, p_from_slot_type, p_from_slot_num)
        INTO STRICT v_from_item_sn;

    EXCEPTION WHEN NO_DATA_FOUND THEN
      RAISE EXCEPTION
        'rz_item_move(): source user does not have the item to move. (FROM_CHAR_SN:%, FROM_SLOT_TYPE:%, FROM_SLOT_ID:%)',
          p_from_owner_sn, p_from_slot_type, p_from_slot_num;
    END;
  END IF;

  IF v_to_item_sn IS NULL THEN
    SELECT rz_item_sn_get(p_to_owner_sn, p_to_slot_type, p_to_slot_num)
      INTO v_to_item_sn;
  END IF;

  -- before moving or swapping, update source item stack amount.
  -- NOTE: currently, zero stack amount will not be checked or deleted!
  IF p_from_item_inc_amount IS NOT NULL THEN
    UPDATE rz_user_item
       SET stack_amount = stack_amount + p_from_item_inc_amount
     WHERE item_sn = v_from_item_sn;
  END IF;
  
  IF v_to_item_sn IS NOT NULL THEN
    -- perform swap.
    SET CONSTRAINTS ALL DEFERRED;

    -- before swapping, update destination item stack amount.
    -- NOTE: currently, zero stack amount will not be checked or deleted!
    IF p_to_item_inc_amount IS NOT NULL THEN
      UPDATE rz_user_item
         SET stack_amount = stack_amount + p_to_item_inc_amount
       WHERE item_sn = v_to_item_sn;
    END IF;
    
    EXECUTE format('UPDATE %I SET item_sn = $1 WHERE owner_sn = $2 AND slot_num = $3;',
                    rz_user_item_slot_type_to_table_name(p_to_slot_type))
      USING v_from_item_sn, p_to_owner_sn, p_to_slot_num;

    EXECUTE format('UPDATE %I SET item_sn = $1 WHERE owner_sn = $2 AND slot_num = $3;',
                    rz_user_item_slot_type_to_table_name(p_from_slot_type))
      USING v_to_item_sn, p_from_owner_sn, p_from_slot_num;
  ELSE
    -- perform move.
    EXECUTE format('WITH di AS (DELETE FROM %I WHERE owner_sn = $1 AND slot_num = $2 RETURNING *)
                    INSERT INTO %I (owner_sn, slot_num, item_sn)
                    SELECT $3, $4, di.item_sn FROM di;',
                    rz_user_item_slot_type_to_table_name(p_from_slot_type),
                    rz_user_item_slot_type_to_table_name(p_to_slot_type))
      USING p_from_owner_sn, p_from_slot_num, p_to_owner_sn, p_to_slot_num;
  END IF;
END;$_$;


--
-- TOC entry 2457 (class 0 OID 0)
-- Dependencies: 293
-- Name: FUNCTION rz_item_move(p_from_owner_sn bigint, p_from_slot_type integer, p_from_slot_num smallint, p_to_owner_sn bigint, p_to_slot_type integer, p_to_slot_num smallint, p_from_item_sn bigint, p_to_item_sn bigint, p_from_item_inc_amount smallint, p_to_item_inc_amount smallint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_move(p_from_owner_sn bigint, p_from_slot_type integer, p_from_slot_num smallint, p_to_owner_sn bigint, p_to_slot_type integer, p_to_slot_num smallint, p_from_item_sn bigint, p_to_item_sn bigint, p_from_item_inc_amount smallint, p_to_item_inc_amount smallint) IS 'If destination item slot already has another item, it will be swapped with from and to.
Otherwise, just performs move.

You can pass p_from_item_sn and/or p_to_item_sn with NULL to this function. In such case, those item_sn will be queried by item slot number.
However, if those item_sn is already known, you can pass your item_sn here for slightly performance improvement.';


--
-- TOC entry 258 (class 1255 OID 28717)
-- Name: rz_item_repair(bigint, bigint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_repair(p_owner_sn bigint, p_item_sn bigint, p_exp integer, p_inc_money integer) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_user_item SET durability = max_durability WHERE item_sn = p_item_sn;
UPDATE rz_character SET exp = p_exp, money = p_inc_money WHERE char_sn = p_owner_sn;$$;


--
-- TOC entry 259 (class 1255 OID 28718)
-- Name: rz_item_repair_all(bigint, jsonb, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_repair_all(p_owner_sn bigint, p_item jsonb, p_exp integer, p_inc_money integer) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE
  js                jsonb;
  v_slot_type       integer;
  v_slot_num        smallint;
  v_item_sn         bigint;
  
BEGIN
  FOR i IN 1 .. jsonb_array_length(p_item) LOOP
    js := p_item->(i - 1);

    v_slot_type       := (js->>'SLOT_TYPE')     ::integer;
    v_slot_num        := (js->>'SLOT_ID')       ::smallint;
    v_item_sn         := (js->>'ITEM_SN')       ::bigint;

    UPDATE rz_user_item
       SET durability = max_durability
     WHERE item_sn = v_item_sn;
  END LOOP;

  UPDATE rz_character
     SET exp   = p_exp,
         money = money + p_inc_money
   WHERE char_sn = p_owner_sn;
END;$$;


--
-- TOC entry 2458 (class 0 OID 0)
-- Dependencies: 259
-- Name: FUNCTION rz_item_repair_all(p_owner_sn bigint, p_item jsonb, p_exp integer, p_inc_money integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_repair_all(p_owner_sn bigint, p_item jsonb, p_exp integer, p_inc_money integer) IS 'Input JSON syntax:
  [
    {
      "SLOT_TYPE": <currently unused, but specify for just in case>,
      "SLOT_ID": <currently unused, but specify for just in case>,
      "ITEM_SN": <item_sn of rz_user_item table>
    }
    ...
  ]';


--
-- TOC entry 260 (class 1255 OID 28719)
-- Name: rz_item_sell_very_common(bigint, jsonb, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_sell_very_common(p_owner_sn bigint, p_item jsonb, p_exp integer, p_inc_money integer) RETURNS void
    LANGUAGE plpgsql
    AS $_$DECLARE
  js                jsonb;
  v_slot_type       integer;
  v_slot_num        smallint;
  v_item_sn         bigint;
  
BEGIN
  FOR i IN 1 .. jsonb_array_length(p_item) LOOP
    js := p_item->(i - 1);

    v_slot_type       := (js->>'SLOT_TYPE')     ::integer;
    v_slot_num        := (js->>'SLOT_ID')       ::smallint;
    v_item_sn         := (js->>'ITEM_SN')       ::bigint;

    UPDATE rz_user_item
       SET status = 'SOLD'
     WHERE item_sn = v_item_sn;

    EXECUTE format('DELETE FROM %I WHERE owner_sn = $1 AND slot_num = $2;',
                    rz_user_item_slot_type_to_table_name(v_slot_type))
      USING p_owner_sn, v_slot_num;
  END LOOP;

  UPDATE rz_character
     SET exp   = p_exp,
         money = money + p_inc_money
   WHERE char_sn = p_owner_sn;
END;$_$;


--
-- TOC entry 2459 (class 0 OID 0)
-- Dependencies: 260
-- Name: FUNCTION rz_item_sell_very_common(p_owner_sn bigint, p_item jsonb, p_exp integer, p_inc_money integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_sell_very_common(p_owner_sn bigint, p_item jsonb, p_exp integer, p_inc_money integer) IS 'Input JSON syntax:
  [
    {
      "SLOT_TYPE": <item slot type to indicate where the item is stored>,
      "SLOT_ID": <slot number and will be sold/deleted>,
      "ITEM_SN": <item_sn of rz_user_item table>
    }
    ...
  ]';


--
-- TOC entry 261 (class 1255 OID 28720)
-- Name: rz_item_sn_get(bigint, integer, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_sn_get(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint) RETURNS SETOF bigint
    LANGUAGE sql STABLE ROWS 1
    AS $$SELECT item_sn
  FROM rz_belongings
 WHERE owner_sn  = p_owner_sn
   AND slot_type = p_slot_type
   AND slot_num  = p_slot_num;$$;


--
-- TOC entry 262 (class 1255 OID 28721)
-- Name: rz_item_sync(bigint, integer, smallint, smallint, smallint, bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_sync(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_stack_amount smallint, p_durability smallint, p_item_sn bigint DEFAULT NULL::bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE
  v_item_sn bigint DEFAULT CASE WHEN p_item_sn = 0 THEN NULL ELSE p_item_sn END;
  
BEGIN
  IF v_item_sn IS NULL THEN
    BEGIN
      SELECT rz_item_sn_get(p_owner_sn, p_slot_type, p_slot_num)
        INTO STRICT v_item_sn;

    EXCEPTION WHEN NO_DATA_FOUND THEN
      RAISE EXCEPTION
        'rz_item_sync(): target item not found. (CHAR_SN:%, SLOT_TYPE:%, SLOT_ID:%)',
          p_owner_sn, p_slot_type, p_slot_num;
    END;
  END IF;
     
  UPDATE rz_user_item
     SET stack_amount = p_stack_amount,
         durability   = p_durability
   WHERE item_sn = v_item_sn;
END;$$;


--
-- TOC entry 2460 (class 0 OID 0)
-- Dependencies: 262
-- Name: FUNCTION rz_item_sync(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_stack_amount smallint, p_durability smallint, p_item_sn bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_sync(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_stack_amount smallint, p_durability smallint, p_item_sn bigint) IS 'You can pass p_item_sn with NULL to this function. In such case, the item_sn will be queried by item slot number.
However, if the item_sn is already known, you can pass your item_sn here for slightly performance improvement.';


--
-- TOC entry 263 (class 1255 OID 28722)
-- Name: rz_item_unequip(bigint, smallint, smallint, smallint, bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_unequip(p_owner_sn bigint, p_from_slot_num smallint, p_to_slot_num smallint, p_item_durability smallint, p_item_sn bigint DEFAULT NULL::bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE
  c_rz_inventory CONSTANT integer NOT NULL DEFAULT rz_user_item_place_to_slot_type('INVENTORY');
  c_rz_equipment CONSTANT integer NOT NULL DEFAULT rz_user_item_place_to_slot_type('EQUIPMENT');
  
  v_item_sn bigint DEFAULT CASE WHEN p_item_sn  = 0 THEN NULL ELSE p_item_sn END;
  
BEGIN
  PERFORM rz_item_move(p_owner_sn,
                       c_rz_equipment,
                       p_from_slot_num,
                       p_owner_sn,
                       c_rz_inventory,
                       p_to_slot_num,
                       p_item_sn,
                       NULL);
                         
  UPDATE rz_user_item
     SET durability = p_item_durability
   WHERE item_sn = (SELECT CASE WHEN p_item_sn IS NULL
                                 THEN rz_item_sn_get(p_owner_sn, c_rz_equipment, p_to_slot_num)
                                ELSE p_item_sn END);
END;$$;


--
-- TOC entry 2461 (class 0 OID 0)
-- Dependencies: 263
-- Name: FUNCTION rz_item_unequip(p_owner_sn bigint, p_from_slot_num smallint, p_to_slot_num smallint, p_item_durability smallint, p_item_sn bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_unequip(p_owner_sn bigint, p_from_slot_num smallint, p_to_slot_num smallint, p_item_durability smallint, p_item_sn bigint) IS 'Move an item to inventory from equipment.

You can pass p_item_sn with NULL to this function. In such case, the item_sn will be queried by item slot number.
However, if the item_sn is already known, you can pass your item_sn here for slightly performance improvement.';


--
-- TOC entry 264 (class 1255 OID 28723)
-- Name: rz_item_update_durability(bigint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_update_durability(p_item_sn bigint, p_durability smallint) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_user_item SET durability = p_durability WHERE item_sn = p_item_sn;$$;


--
-- TOC entry 294 (class 1255 OID 29074)
-- Name: rz_item_update_stack_amount(bigint, integer, smallint, smallint, bigint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_item_update_stack_amount(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_inc_stack_amount smallint, p_item_sn bigint DEFAULT NULL::bigint, p_exp integer DEFAULT NULL::integer, p_inc_money integer DEFAULT NULL::integer) RETURNS void
    LANGUAGE plpgsql
    AS $_$DECLARE
  v_item_sn      bigint    DEFAULT CASE WHEN p_item_sn = 0 THEN NULL ELSE p_item_sn END;
  v_after_amount smallint;
  
BEGIN
  IF v_item_sn IS NULL THEN
    BEGIN
      SELECT rz_item_sn_get(p_owner_sn, p_slot_type, p_slot_num)
        INTO STRICT v_item_sn;

    EXCEPTION WHEN NO_DATA_FOUND THEN
      RAISE EXCEPTION
        'rz_item_update_stack_amount(): target item not found. (CHAR_SN:%, SLOT_TYPE:%, SLOT_ID:%)',
          p_owner_sn, p_slot_type, p_slot_num;
    END;
  END IF;
     
  UPDATE rz_user_item
     SET stack_amount = stack_amount + p_inc_stack_amount
   WHERE item_sn = v_item_sn
  RETURNING stack_amount INTO v_after_amount;

  IF v_after_amount <= 0 THEN
    EXECUTE format('DELETE FROM %I WHERE owner_sn = $1 AND slot_num = $2;',
                    rz_user_item_slot_type_to_table_name(p_slot_type))
      USING p_owner_sn, p_slot_num;
            
    UPDATE rz_user_item
       SET owner_sn = NULL,
           status   = 'DISCARDED'
     WHERE item_sn = v_item_sn;
  END IF;

  -- update exp and money.
  IF p_exp IS NOT NULL AND p_inc_money IS NOT NULL THEN
    UPDATE rz_character
       SET exp   = p_exp,
           money = money + p_inc_money
     WHERE char_sn = p_owner_sn;
  END IF;
END;$_$;


--
-- TOC entry 2462 (class 0 OID 0)
-- Dependencies: 294
-- Name: FUNCTION rz_item_update_stack_amount(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_inc_stack_amount smallint, p_item_sn bigint, p_exp integer, p_inc_money integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_item_update_stack_amount(p_owner_sn bigint, p_slot_type integer, p_slot_num smallint, p_inc_stack_amount smallint, p_item_sn bigint, p_exp integer, p_inc_money integer) IS 'You can pass p_item_sn with NULL to this function. In such case, the item_sn will be queried by item slot number.
However, if the item_sn is already known, you can pass your item_sn here for slightly performance improvement.

p_inc_stack_amount can be minus to perform decrease operation.';


--
-- TOC entry 265 (class 1255 OID 28725)
-- Name: rz_mail_get_summary(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_mail_get_summary(p_char_sn bigint, OUT "UNREAD_MAIL_COUNT" integer, OUT "TOTAL_MAIL_COUNT" integer, OUT "TOP_MAIL_SN" bigint, OUT "BOTTOM_MAIL_SN" bigint) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 1
    AS $$SELECT 0, 0, 0::bigint, 0::bigint LIMIT 0;$$;


--
-- TOC entry 2463 (class 0 OID 0)
-- Dependencies: 265
-- Name: FUNCTION rz_mail_get_summary(p_char_sn bigint, OUT "UNREAD_MAIL_COUNT" integer, OUT "TOTAL_MAIL_COUNT" integer, OUT "TOP_MAIL_SN" bigint, OUT "BOTTOM_MAIL_SN" bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_mail_get_summary(p_char_sn bigint, OUT "UNREAD_MAIL_COUNT" integer, OUT "TOTAL_MAIL_COUNT" integer, OUT "TOP_MAIL_SN" bigint, OUT "BOTTOM_MAIL_SN" bigint) IS 'TODO: complete this function';


--
-- TOC entry 266 (class 1255 OID 28726)
-- Name: rz_palette_exchange(bigint, smallint, smallint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_palette_exchange(p_char_sn bigint, p_palette_set smallint, p_regist_place_from smallint, p_regist_place_to smallint) RETURNS void
    LANGUAGE sql
    AS $$WITH pf AS (DELETE FROM rz_palette WHERE char_sn      = p_char_sn
                                     AND palette_set  = p_palette_set
                                     AND regist_place = p_regist_place_from
            RETURNING p_regist_place_to AS rp,
                      entry_id          AS ei,
                      entry_type        AS et),
     pt AS (DELETE FROM rz_palette WHERE char_sn      = p_char_sn
                                     AND palette_set  = p_palette_set
                                     AND regist_place = p_regist_place_to
            RETURNING p_regist_place_from AS rp,
                      entry_id            AS ei,
                      entry_type          AS et)
  INSERT INTO rz_palette (  char_sn,   palette_set,   regist_place, entry_id, entry_type)
                  SELECT  p_char_sn, p_palette_set,             rp,       ei,         et FROM pf
        UNION ALL SELECT  p_char_sn, p_palette_set,             rp,       ei,         et FROM pt;$$;


--
-- TOC entry 267 (class 1255 OID 28727)
-- Name: rz_palette_set(bigint, smallint, smallint, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_palette_set(p_char_sn bigint, p_palette_set smallint, p_regist_place smallint, p_entry_id integer, p_entry_type integer) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_palette (char_sn, palette_set, regist_place, entry_id, entry_type)
     VALUES (p_char_sn,
             p_palette_set,
             p_regist_place,
             p_entry_id,
             (CASE p_entry_type WHEN 1 THEN 'ITEM' WHEN 2 THEN 'TALENT' END )::rz_palette_entry_type)
ON CONFLICT (char_sn, palette_set, regist_place)
DO UPDATE SET entry_id   = EXCLUDED.entry_id,
              entry_type = EXCLUDED.entry_type;$$;


--
-- TOC entry 2464 (class 0 OID 0)
-- Dependencies: 267
-- Name: FUNCTION rz_palette_set(p_char_sn bigint, p_palette_set smallint, p_regist_place smallint, p_entry_id integer, p_entry_type integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_palette_set(p_char_sn bigint, p_palette_set smallint, p_regist_place smallint, p_entry_id integer, p_entry_type integer) IS 'TODO: replace statement with UPSERT of PostgreSQL 9.5';


--
-- TOC entry 268 (class 1255 OID 28728)
-- Name: rz_palette_unset(bigint, smallint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_palette_unset(p_char_sn bigint, p_palette_set smallint, p_unregist_place smallint) RETURNS void
    LANGUAGE sql
    AS $$DELETE FROM rz_palette WHERE char_sn      = p_char_sn
                         AND palette_set  = p_palette_set
                         AND regist_place = p_unregist_place;$$;


--
-- TOC entry 282 (class 1255 OID 29055)
-- Name: rz_quest_accept(bigint, smallint, integer, boolean, interval, jsonb, jsonb); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_accept(p_char_sn bigint, p_entry_num smallint, p_quest_id integer, p_quest_complete boolean, p_expire_in interval, p_insert_item jsonb, p_delete_item jsonb, OUT "SLOT_ID" smallint, OUT "NEW_ITEM_SN" bigint) RETURNS SETOF record
    LANGUAGE sql ROWS 5
    AS $$INSERT INTO rz_quest (char_sn, entry_num, quest_id, state, time_limit)
              VALUES (p_char_sn,
                      p_entry_num,
                      p_quest_id,
                      (CASE WHEN p_quest_complete THEN 'COMPLETED'
                                                  ELSE 'IN_PROGRESS' END)::rz_quest_state,
                      p_expire_in);

SELECT rz_quest_item_delete(p_char_sn, p_delete_item);
SELECT rz_quest_item_insert(p_char_sn, p_insert_item);$$;


--
-- TOC entry 248 (class 1255 OID 28730)
-- Name: rz_quest_complete(bigint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_complete(p_char_sn bigint, p_entry_num smallint) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_quest
   SET state = (CASE WHEN state = 'IN_PROGRESS' THEN 'FAILED'
                     ELSE state END)::rz_quest_state
 WHERE char_sn   = p_char_sn
   AND entry_num = p_entry_num;$$;


--
-- TOC entry 269 (class 1255 OID 28731)
-- Name: rz_quest_done(bigint, smallint, smallint, integer, integer, jsonb, jsonb); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_done(p_char_sn bigint, p_entry_num smallint, p_level smallint, p_exp integer, p_money integer, p_insert_item jsonb, p_delete_item jsonb, OUT "SLOT_ID" smallint, OUT "NEW_ITEM_SN" bigint) RETURNS SETOF record
    LANGUAGE sql ROWS 5
    AS $$WITH dq AS (DELETE FROM rz_quest WHERE char_sn   = p_char_sn
                                   AND entry_num = p_entry_num
              RETURNING quest_id AS qi)
 INSERT INTO rz_quest_history (char_sn, quest_id)
  SELECT p_char_sn, dq.qi FROM dq;

SELECT rz_quest_item_delete(p_char_sn, p_delete_item);
SELECT rz_quest_item_insert(p_char_sn, p_insert_item);$$;


--
-- TOC entry 270 (class 1255 OID 28732)
-- Name: rz_quest_fail(bigint, smallint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_fail(p_char_sn bigint, p_entry_num smallint) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_quest
   SET state = (CASE WHEN (state = 'IN_PROGRESS' OR state = 'COMPLETED') THEN 'FAILED'
                     ELSE state END)::rz_quest_state
 WHERE char_sn   = p_char_sn
   AND entry_num = p_entry_num;$$;


--
-- TOC entry 271 (class 1255 OID 28733)
-- Name: rz_quest_giveup(bigint, smallint, jsonb); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_giveup(p_char_sn bigint, p_entry_num smallint, p_delete_item jsonb) RETURNS void
    LANGUAGE sql
    AS $$DELETE FROM rz_quest WHERE char_sn   = p_char_sn
                       AND entry_num = p_entry_num;

SELECT rz_quest_item_delete(p_char_sn, p_delete_item);$$;


--
-- TOC entry 272 (class 1255 OID 28734)
-- Name: rz_quest_item_delete(bigint, jsonb); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_item_delete(p_char_sn bigint, p_del_item jsonb) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE
  js                jsonb;
  v_slot_type       integer;
  v_slot_num        smallint;
  v_decrease_amount smallint;
  
BEGIN
  FOR i IN 1 .. jsonb_array_length(p_del_item) LOOP
    js := p_del_item->(i - 1);

    v_slot_type       := (js->>'SLOT_TYPE')     ::integer;
    v_slot_num        := (js->>'SLOT_ID')       ::smallint;
    v_decrease_amount := (js->>'DEC_STACK_AMT') ::smallint;

    PERFORM rz_item_decrease_stack_amount(p_char_sn, v_slot_type, v_slot_num, v_decrease_amount);
  END LOOP;
END;$$;


--
-- TOC entry 2465 (class 0 OID 0)
-- Dependencies: 272
-- Name: FUNCTION rz_quest_item_delete(p_char_sn bigint, p_del_item jsonb); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_quest_item_delete(p_char_sn bigint, p_del_item jsonb) IS 'Input JSON syntax:
  [
    {
      "SLOT_TYPE": <Inventory, Equipment or Storage>,
      "SLOT_ID": <Item Slot number>,
      "DEC_STACK_AMOUNT": <Item Amount to decrease>
    }
    ...
  ]';


--
-- TOC entry 281 (class 1255 OID 29056)
-- Name: rz_quest_item_insert(bigint, jsonb); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_item_insert(p_char_sn bigint, p_item jsonb, OUT "SLOT_ID" smallint, OUT "NEW_ITEM_SN" bigint) RETURNS SETOF record
    LANGUAGE plpgsql ROWS 5
    AS $$DECLARE
  js                jsonb;
  v_slot_type       integer;
  v_slot_num        smallint;
  v_item_sn         bigint;
  v_item_id         integer;
  v_increase_amount smallint;
  v_max_durability  smallint;
  v_color           integer;
  v_tied            boolean;
  v_has_period      boolean;
  v_expire_in       interval;
  
BEGIN
  FOR i IN 1 .. jsonb_array_length(p_item) LOOP
    js := p_item->(i - 1);

    v_slot_type       := (js->>'SLOT_TYPE')     ::integer;
    v_slot_num        := (js->>'SLOT_ID')       ::smallint;
    v_item_sn         := (js->>'ITEM_SN')       ::bigint;
    v_item_id         := (js->>'ITEM_ID')       ::integer;
    v_increase_amount := (js->>'INC_STACK_AMT') ::smallint;
    v_max_durability  := (js->>'MAX_DURA')      ::smallint;
    v_color           := (js->>'COLOR')         ::integer;
    v_tied            := (js->>'CLAIMED')       ::boolean;
    v_has_period      := (js->>'HAS_PERIOD')    ::boolean;
    v_expire_in       := (js->>'EXPIRE_IN')     ::interval;
    
    IF COALESCE(v_item_sn, 0) = 0 THEN
      -- insert as new item.
      SELECT v_slot_num,
             rz_item_insert(p_char_sn,
                            v_slot_type,
                            v_slot_num,
                            v_item_id,
                            v_increase_amount,
                            v_max_durability,
                            v_max_durability,
                            v_color,
                            v_tied,
                            v_has_period,
                            v_expire_in)
        INTO "SLOT_ID", "NEW_ITEM_SN";
      RETURN NEXT;
    ELSE
      -- update existing stack amount.
      PERFORM rz_increase_stack_amount(p_char_sn,
                                       v_slot_type,
                                       v_slot_num,
                                       v_increase_amount);
    END IF;
  END LOOP;
END;$$;


--
-- TOC entry 2466 (class 0 OID 0)
-- Dependencies: 281
-- Name: FUNCTION rz_quest_item_insert(p_char_sn bigint, p_item jsonb, OUT "SLOT_ID" smallint, OUT "NEW_ITEM_SN" bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_quest_item_insert(p_char_sn bigint, p_item jsonb, OUT "SLOT_ID" smallint, OUT "NEW_ITEM_SN" bigint) IS 'Input JSON syntax:
  [
    {
      "SLOT_TYPE": <Indicating inventory, equipment or storage. see rz_user_item_place_to_slot_type() function.>,
      "SLOT_ID": <Item Slot number.>,
      "ITEM_SN": <If existing ITEM_SN is specified, only stack amount will be changed.>,
      "INC_STACK_AMT": <Stack Amount to set or increase.>,
      "MAX_DURA": <Max Durability.>,
      "COLOR": <Item Color.>,
      "CLAIMED": <Item is bound to user or not.>,
      "HAS_PERIOD": <Item has expiration period or not.>,
      "EXPIRE_IN": <Item period of expiration.>
    }
    ...
  ]

If ITEM_SN wasn''t zero, only SLOT_TYPE, SLOT_ID, INC_STACK_AMOUNT are used.';


--
-- TOC entry 273 (class 1255 OID 28736)
-- Name: rz_quest_state_by_num(integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_state_by_num(p_num integer) RETURNS rz_quest_state
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (CASE p_num
          WHEN 1 THEN 'IN_PROGRESS'
          WHEN 2 THEN 'COMPLETED'
          WHEN 3 THEN 'FAILED'
        ELSE 'UNAVAILABLE' END)::rz_quest_state;$$;


--
-- TOC entry 2467 (class 0 OID 0)
-- Dependencies: 273
-- Name: FUNCTION rz_quest_state_by_num(p_num integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_quest_state_by_num(p_num integer) IS 'NOTE: If you altered values of this function, then alter rz_quest_state_to_num() too!';


--
-- TOC entry 274 (class 1255 OID 28737)
-- Name: rz_quest_state_to_num(rz_quest_state); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_state_to_num(p_state rz_quest_state) RETURNS integer
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (CASE p_state
          WHEN 'IN_PROGRESS' THEN 1
          WHEN 'COMPLETED'   THEN 2
          WHEN 'FAILED'      THEN 3
        ELSE 0 END)::integer;$$;


--
-- TOC entry 2468 (class 0 OID 0)
-- Dependencies: 274
-- Name: FUNCTION rz_quest_state_to_num(p_state rz_quest_state); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_quest_state_to_num(p_state rz_quest_state) IS 'NOTE: If you altered values of this function, then alter rz_quest_state_by_num() too!';


--
-- TOC entry 275 (class 1255 OID 28738)
-- Name: rz_quest_update_objective(bigint, smallint, integer, integer, boolean); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_update_objective(p_char_sn bigint, p_entry_num smallint, p_obj_id integer, p_obj_progress integer, p_quest_complete boolean) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_quest
   SET objective_progress[p_obj_id] = p_obj_progress,
       state                        = CASE WHEN p_quest_complete AND state = 'IN_PROGRESS'
                                           THEN 'COMPLETED'
                                           ELSE state END
 WHERE char_sn   = p_char_sn
   AND entry_num = p_entry_num;$$;


--
-- TOC entry 276 (class 1255 OID 28739)
-- Name: rz_quest_update_var(bigint, smallint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_quest_update_var(p_char_sn bigint, p_entry_num smallint, p_var integer) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_quest
   SET variable = p_var
 WHERE char_sn   = p_char_sn
   AND entry_num = p_entry_num;$$;


--
-- TOC entry 289 (class 1255 OID 29068)
-- Name: rz_recipe_delete(bigint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_recipe_delete(p_char_sn bigint, p_recp_id integer) RETURNS void
    LANGUAGE sql
    AS $$DELETE FROM rz_recipe WHERE char_sn = p_char_sn AND recp_id = p_recp_id;$$;


--
-- TOC entry 287 (class 1255 OID 29067)
-- Name: rz_recipe_insert(bigint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_recipe_insert(p_char_sn bigint, p_recp_id integer) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_recipe (char_sn, recp_id) VALUES (p_char_sn, p_recp_id)
  ON CONFLICT DO NOTHING;$$;


--
-- TOC entry 277 (class 1255 OID 28740)
-- Name: rz_talent_cooltime_get(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_talent_cooltime_get(p_char_sn bigint, OUT "TALENT_ID" integer, OUT "REMAIN_TIME" real) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 100
    AS $$SELECT talent_id,
       EXTRACT(EPOCH FROM ((added_time + remained_duration) - CURRENT_TIMESTAMP))::real
  FROM rz_talent_cooltime
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 280 (class 1255 OID 28741)
-- Name: rz_talent_cooltime_update_all(bigint, jsonb); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_talent_cooltime_update_all(p_char_sn bigint, p_json jsonb) RETURNS void
    LANGUAGE plpgsql
    AS $$DECLARE js jsonb;
BEGIN
  DELETE FROM rz_talent_cooltime WHERE char_sn = p_char_sn;
  
  FOR i IN 1 .. jsonb_array_length(p_json) LOOP
    js := p_json->(i - 1);
    
    INSERT INTO rz_talent_cooltime (char_sn,
                                    talent_id,
                                    remained_duration)
      VALUES (p_char_sn,
              (js->>'TALENT_ID')::integer,
              (js->>'REMAIN_TIME' || ' seconds')::interval);
  END LOOP;
END;$$;


--
-- TOC entry 2469 (class 0 OID 0)
-- Dependencies: 280
-- Name: FUNCTION rz_talent_cooltime_update_all(p_char_sn bigint, p_json jsonb); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_talent_cooltime_update_all(p_char_sn bigint, p_json jsonb) IS 'Input JSON syntax:
  [
    {
      "TALENT_ID" : <Talent ID>,
      "REMAIN_TIME" : <Cool Time in seconds>
    }
    ...
  ]';


--
-- TOC entry 278 (class 1255 OID 28742)
-- Name: rz_talent_learn(bigint, smallint, integer, smallint, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_talent_learn(p_char_sn bigint, p_talent_set smallint, p_talent_id integer, p_consume_tp smallint, p_talent_id_prev integer, OUT "RESULT" integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$DECLARE
  v_remain_tp smallint NOT NULL
    DEFAULT (SELECT remain_tp[p_talent_set] FROM rz_valid_character WHERE char_sn = p_char_sn);
BEGIN
  v_remain_tp := v_remain_tp - p_consume_tp;

  IF v_remain_tp < 0 THEN
    -- Not enough TP.
    SELECT 1 INTO "RESULT";
    RETURN;
  END IF;

  UPDATE rz_valid_character SET remain_tp[p_talent_set] = v_remain_tp WHERE char_sn = p_char_sn;
  
  INSERT INTO rz_talent (  char_sn,   talent_set,   talent_id)
                 VALUES (p_char_sn, p_talent_set, p_talent_id);

  -- If a talent was upgrade of a previous talent, upgrade the palette's talent id too.
  IF COALESCE(p_talent_id_prev, 0) <> 0 THEN
    UPDATE rz_palette
       SET entry_id = p_talent_id
     WHERE char_sn     = p_char_sn
       AND palette_set = p_talent_set
       AND entry_id    = p_talent_id_prev
       AND entry_type  = 'TALENT';
  END IF;

  SELECT 0 INTO "RESULT";
END;$$;


--
-- TOC entry 2470 (class 0 OID 0)
-- Dependencies: 278
-- Name: FUNCTION rz_talent_learn(p_char_sn bigint, p_talent_set smallint, p_talent_id integer, p_consume_tp smallint, p_talent_id_prev integer, OUT "RESULT" integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_talent_learn(p_char_sn bigint, p_talent_set smallint, p_talent_id integer, p_consume_tp smallint, p_talent_id_prev integer, OUT "RESULT" integer) IS 'Learn a new talent. One of the following values returned as result code:
  0: Successfully learned a talent.
  1: You don''t have enough talent points to learn this talent.';


--
-- TOC entry 210 (class 1255 OID 28743)
-- Name: rz_test_char_property_set(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_test_char_property_set(p_char_sn bigint) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_character
   SET level        = 40,
       money        = 100000000,
       remain_tp    = '{999,999,999}',
       player_grade = 1
 WHERE char_sn = p_char_sn;$$;


--
-- TOC entry 2471 (class 0 OID 0)
-- Dependencies: 210
-- Name: FUNCTION rz_test_char_property_set(p_char_sn bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_test_char_property_set(p_char_sn bigint) IS 'FOR TESTING PURPOSE: Set Character to Level 40, 10000GP, 999TP, and promote to GM.';


--
-- TOC entry 211 (class 1255 OID 28744)
-- Name: rz_user_item_place_by_slot_type(integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_user_item_place_by_slot_type(p_slot_type integer) RETURNS rz_user_item_place
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (CASE p_slot_type
          WHEN 1 THEN 'INVENTORY' -- 1: Inventory
          WHEN 2 THEN 'EQUIPMENT' -- 2: Equipment
          WHEN 3 THEN 'STORAGE'   -- 3: Storage
        END)::rz_user_item_place;$$;


--
-- TOC entry 2472 (class 0 OID 0)
-- Dependencies: 211
-- Name: FUNCTION rz_user_item_place_by_slot_type(p_slot_type integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_user_item_place_by_slot_type(p_slot_type integer) IS 'NOTE: If you altered values of this function, then alter rz_user_item_place_to_slot_type() too!';


--
-- TOC entry 212 (class 1255 OID 28745)
-- Name: rz_user_item_place_to_slot_type(rz_user_item_place); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_user_item_place_to_slot_type(p_item_place rz_user_item_place) RETURNS integer
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (CASE p_item_place
          WHEN 'INVENTORY' THEN 1 -- Inventory: 1
          WHEN 'EQUIPMENT' THEN 2 -- Equipment: 2
          WHEN 'STORAGE'   THEN 3 -- Storage: 3
        END)::integer;$$;


--
-- TOC entry 2473 (class 0 OID 0)
-- Dependencies: 212
-- Name: FUNCTION rz_user_item_place_to_slot_type(p_item_place rz_user_item_place); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_user_item_place_to_slot_type(p_item_place rz_user_item_place) IS 'NOTE: If you altered values of this function, then alter rz_user_item_place_by_slot_type() too!';


--
-- TOC entry 228 (class 1255 OID 28746)
-- Name: rz_user_item_place_to_table_name(rz_user_item_place); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_user_item_place_to_table_name(p_item_place rz_user_item_place) RETURNS text
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (CASE p_item_place
          WHEN 'INVENTORY' THEN 'rz_inventory' -- Inventory: rz_inventory
          WHEN 'EQUIPMENT' THEN 'rz_equipment' -- Equipment: rz_equipment
          WHEN 'STORAGE'   THEN 'rz_storage'   -- Storage: rz_storage
        END)::text;$$;


--
-- TOC entry 279 (class 1255 OID 28747)
-- Name: rz_user_item_slot_type_to_table_name(integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_user_item_slot_type_to_table_name(p_slot_type integer) RETURNS text
    LANGUAGE sql IMMUTABLE
    AS $$SELECT rz_user_item_place_to_table_name(rz_user_item_place_by_slot_type(p_slot_type));$$;


SET search_path = const_def, pg_catalog;

SET default_with_oids = false;

--
-- TOC entry 184 (class 1259 OID 28748)
-- Name: rz_default_equipment; Type: TABLE; Schema: const_def; Owner: -
--

CREATE TABLE rz_default_equipment (
    style_id smallint NOT NULL,
    item_id integer NOT NULL,
    slot_num smallint NOT NULL,
    amount smallint DEFAULT 1 NOT NULL,
    place rz_default_equipment_place NOT NULL
);


--
-- TOC entry 2474 (class 0 OID 0)
-- Dependencies: 184
-- Name: TABLE rz_default_equipment; Type: COMMENT; Schema: const_def; Owner: -
--

COMMENT ON TABLE rz_default_equipment IS 'This table defines a set of equipments used on character creation.
You can use rz_char_default_equipment_generate() to insert pre-defined defaults.';


--
-- TOC entry 2475 (class 0 OID 0)
-- Dependencies: 184
-- Name: COLUMN rz_default_equipment.style_id; Type: COMMENT; Schema: const_def; Owner: -
--

COMMENT ON COLUMN rz_default_equipment.style_id IS '1: Defender
2: Berserker
3: Assassin
4: Ranger
5: Sorcerer
6: Cleric';


--
-- TOC entry 185 (class 1259 OID 28752)
-- Name: rz_default_talent; Type: TABLE; Schema: const_def; Owner: -
--

CREATE TABLE rz_default_talent (
    style_id smallint NOT NULL,
    palette_place smallint NOT NULL,
    talent_id integer NOT NULL
);


--
-- TOC entry 2476 (class 0 OID 0)
-- Dependencies: 185
-- Name: TABLE rz_default_talent; Type: COMMENT; Schema: const_def; Owner: -
--

COMMENT ON TABLE rz_default_talent IS 'This table defines talent IDs for newly made characters.
Call rz_default_talent_generate() function to generate predefined defaults.';


--
-- TOC entry 186 (class 1259 OID 28755)
-- Name: rz_xitem; Type: TABLE; Schema: const_def; Owner: -
--

CREATE TABLE rz_xitem (
    id integer NOT NULL,
    name text,
    tier text,
    "desc" text,
    contents_tag text,
    mesh_name text,
    node_name text,
    node_name2 text,
    mesh_path text,
    parent_name text,
    parent_name2 text,
    texcolor text,
    "Item_sound" text,
    type text,
    condition integer,
    sex text,
    slot text,
    weapon_type text,
    "DamageAttrib" text,
    searchable boolean,
    buying_price integer,
    selling_price integer,
    repair_grade integer,
    stack_amount integer,
    max_durability integer,
    min_damage integer,
    max_damage integer,
    magic_min_damage integer,
    magic_max_damage integer,
    icon text,
    "EnchantEffectList" text,
    "CostumeID" integer,
    "CostumeItemDyeable" boolean,
    sub_slot text,
    armor_type text,
    "AP" integer,
    "MagicAP" integer,
    tradable boolean,
    sellable boolean,
    discardable boolean,
    "unique" boolean,
    repairable boolean,
    "EquipEnchantSlot" text,
    "ModSTR" integer,
    "ModINT" integer,
    "ModCON" integer,
    "ModDEX" integer,
    "ModCHA" integer,
    parent_name_left text,
    mesh_name_by_sex boolean,
    validation boolean,
    claim_required boolean,
    "Dyeable" boolean,
    "ModHPRegen" integer,
    "ModENRegen" integer,
    "ModSTARegen" integer,
    "ModHPMax" integer,
    "ModENMax" integer,
    "ModSTAMax" integer,
    item_level integer,
    "Enchantable" boolean,
    "ProcEffectBuffId" integer,
    quest_related boolean,
    usable_type text,
    usable_param text,
    usable_consume boolean,
    "MessagePopup" boolean,
    "EnchantCategory" text,
    "EnchantRate" real,
    "DropPenalty" integer,
    "DescParam" text,
    qpvp_related boolean,
    volatile boolean,
    "ExpirationPeriod" integer,
    gather_type text,
    "ModDefSlashAmp" real,
    "ModDefBluntAmp" real,
    "ModDefPierceAmp" real,
    "ModMeleeAtkAmp" real,
    "ModRangeAtkAmp" real,
    "ModMagicAtkAmp" real,
    "ModDefHolyAmp" real,
    "ModMoveSpeed" real,
    "DyeColor" text,
    "ModPVPAP" real,
    "ModPVPDamage" real,
    "ModCriticalPhysic" integer,
    "ModCriticalMagic" integer,
    "ModCriticalAmp" real,
    "DecoEffectFemale" text,
    "DecoEffectMale" text,
    housing_npc integer,
    "UsagePeriod" integer,
    "Cashitem" boolean,
    "GemEnchantBuff" integer,
    "EnchantAllowEquipments" text,
    "EnchantAllowWeapons" text,
    "EnchantAllowArmors" text,
    "EnchantLimitLevel" integer,
    "EnchantLimitWeapons" text,
    "EnchantLevel" text,
    "FireMinDamage" integer,
    "FireMaxDamage" integer,
    "FireMagicMinDamage" integer,
    "FireMagicMaxDamage" integer,
    "ColdMinDamage" integer,
    "ColdMaxDamage" integer,
    "ColdMagicMinDamage" integer,
    "ColdMagicMaxDamage" integer,
    "PoisonMinDamage" integer,
    "PoisonMaxDamage" integer,
    "PoisonMagicMinDamage" integer,
    "PoisonMagicMaxDamage" integer,
    "LightningMinDamage" integer,
    "LightningMaxDamage" integer,
    "LightningMagicMinDamage" integer,
    "LightningMagicMaxDamage" integer,
    "HolyMinDamage" integer,
    "HolyMaxDamage" integer,
    "HolyMagicMinDamage" integer,
    "HolyMagicMaxDamage" integer,
    "UnholyMinDamage" integer,
    "UnholyMaxDamage" integer,
    "UnholyMagicMinDamage" integer,
    "UnholyMagicMaxDamage" integer,
    "ModFireAP" integer,
    "ModFireMagicAP" integer,
    "ModColdAP" integer,
    "ModColdMagicAP" integer,
    "ModLightningAP" integer,
    "ModLightningMagicAP" integer,
    "ModPoisonAP" integer,
    "ModPoisonMagicAP" integer,
    "ModHolyAP" integer,
    "ModHolyMagicAP" integer,
    "ModUnholyAP" integer,
    "ModUnholyMagicAP" integer,
    "EnchantLimitEquipments" text
);


--
-- TOC entry 2477 (class 0 OID 0)
-- Dependencies: 186
-- Name: TABLE rz_xitem; Type: COMMENT; Schema: const_def; Owner: -
--

COMMENT ON TABLE rz_xitem IS 'This table holds a complete copy of xitem.xml and used by DB to work with such data.
Use rz_xitem_set() function to update contents of this table.';


SET search_path = public, pg_catalog;

--
-- TOC entry 187 (class 1259 OID 28761)
-- Name: rz_equipment; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_equipment (
    owner_sn bigint NOT NULL,
    slot_num smallint NOT NULL,
    item_sn bigint NOT NULL,
    reg_time timestamp with time zone DEFAULT now() NOT NULL
);


--
-- TOC entry 2478 (class 0 OID 0)
-- Dependencies: 187
-- Name: TABLE rz_equipment; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_equipment IS 'Refer a comment of rz_inventory for more info.';


--
-- TOC entry 188 (class 1259 OID 28765)
-- Name: rz_inventory; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_inventory (
    owner_sn bigint NOT NULL,
    slot_num smallint NOT NULL,
    item_sn bigint NOT NULL,
    reg_time timestamp with time zone DEFAULT now() NOT NULL
);


--
-- TOC entry 2479 (class 0 OID 0)
-- Dependencies: 188
-- Name: TABLE rz_inventory; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_inventory IS 'Original RZ_GAMEDB was storing inventory items and storage items into one table.
but I''ve separated them to multiple tables.

This table just stores inventory slot index and corresponding item SN.
Refer rz_user_item table for actual item informations.';


--
-- TOC entry 189 (class 1259 OID 28769)
-- Name: rz_storage; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_storage (
    owner_sn bigint NOT NULL,
    slot_num smallint NOT NULL,
    item_sn bigint NOT NULL,
    reg_time timestamp with time zone DEFAULT now() NOT NULL
);


--
-- TOC entry 2480 (class 0 OID 0)
-- Dependencies: 189
-- Name: TABLE rz_storage; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_storage IS 'Refer comment of rz_inventory for more info.';


--
-- TOC entry 190 (class 1259 OID 28773)
-- Name: rz_belongings; Type: VIEW; Schema: public; Owner: -
--

CREATE VIEW rz_belongings AS
 SELECT rz_inventory.owner_sn,
    rz_inventory.slot_num,
    rz_inventory.item_sn,
    rz_inventory.reg_time,
    rz_user_item_place_to_slot_type('INVENTORY'::rz_user_item_place) AS slot_type
   FROM rz_inventory
UNION ALL
 SELECT rz_equipment.owner_sn,
    rz_equipment.slot_num,
    rz_equipment.item_sn,
    rz_equipment.reg_time,
    rz_user_item_place_to_slot_type('EQUIPMENT'::rz_user_item_place) AS slot_type
   FROM rz_equipment
UNION ALL
 SELECT rz_storage.owner_sn,
    rz_storage.slot_num,
    rz_storage.item_sn,
    rz_storage.reg_time,
    rz_user_item_place_to_slot_type('STORAGE'::rz_user_item_place) AS slot_type
   FROM rz_storage;


--
-- TOC entry 2481 (class 0 OID 0)
-- Dependencies: 190
-- Name: VIEW rz_belongings; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON VIEW rz_belongings IS 'This view is used to union inventory items and storage items of players.
It will also adds extra column named slot_type. The value will be 1 for inventory items, 2 for equipped items and 3 for storage items.
This column is compliant with original RZ_GAMEDB''s design.';


--
-- TOC entry 208 (class 1259 OID 29143)
-- Name: rz_buff; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_buff (
    char_sn bigint NOT NULL,
    buff_id integer NOT NULL,
    stack_count smallint DEFAULT 1 NOT NULL,
    critical_percent real DEFAULT 0 NOT NULL,
    critical_apply_rate real DEFAULT 0 NOT NULL,
    min_damage integer DEFAULT 0 NOT NULL,
    max_damage integer DEFAULT 0 NOT NULL,
    min_heal integer DEFAULT 0 NOT NULL,
    max_heal integer DEFAULT 0 NOT NULL,
    min_attribute_damage integer DEFAULT 0 NOT NULL,
    max_attribute_damage integer DEFAULT 0 NOT NULL,
    added_time timestamp with time zone DEFAULT now() NOT NULL,
    remained_duration interval NOT NULL,
    fixed_duration boolean DEFAULT false NOT NULL
);


--
-- TOC entry 191 (class 1259 OID 28787)
-- Name: rz_character; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_character (
    char_sn bigint NOT NULL,
    accn_sn bigint NOT NULL,
    style smallint NOT NULL,
    preference rz_character_preference NOT NULL,
    player_grade smallint DEFAULT 0 NOT NULL,
    name text,
    level smallint DEFAULT 1 NOT NULL,
    exp integer DEFAULT 0 NOT NULL,
    money integer DEFAULT 0 NOT NULL,
    storage_money integer DEFAULT 0 NOT NULL,
    health integer DEFAULT 1 NOT NULL,
    energy integer DEFAULT 0 NOT NULL,
    stamina integer DEFAULT 0 NOT NULL,
    remain_tp smallint[] DEFAULT '{0,0,0}'::smallint[] NOT NULL,
    active_skill_set smallint DEFAULT 1 NOT NULL,
    fatigue smallint DEFAULT 0 NOT NULL,
    inn_id integer DEFAULT 0 NOT NULL,
    inn_room_id integer DEFAULT 0 NOT NULL,
    primary_palette_id smallint DEFAULT 5 NOT NULL,
    secondary_palette_id smallint DEFAULT 5 NOT NULL,
    active_palette_id smallint DEFAULT 0 NOT NULL,
    weapon_set smallint DEFAULT 0 NOT NULL,
    pos rz_vector_3d DEFAULT ROW((0)::real, (0)::real, (0)::real) NOT NULL,
    dir rz_vector_3d DEFAULT ROW((0)::real, (0)::real, (0)::real) NOT NULL,
    shared_field_id integer DEFAULT 0 NOT NULL,
    entrance_pos rz_vector_3d DEFAULT ROW((0)::real, (0)::real, (0)::real) NOT NULL,
    dynamic_field_group_uid bigint DEFAULT 0 NOT NULL,
    dynamic_field_id integer DEFAULT 0 NOT NULL,
    checkpoint_field_id integer DEFAULT 0 NOT NULL,
    checkpoint_marker_id integer DEFAULT 0 NOT NULL,
    party_uid bigint DEFAULT 0 NOT NULL,
    arena_uid bigint DEFAULT 0 NOT NULL,
    arena_team_uid bigint DEFAULT 0 NOT NULL,
    play_time interval DEFAULT '00:00:00'::interval NOT NULL,
    dead_time timestamp with time zone,
    disconn_time timestamp with time zone,
    create_time timestamp with time zone DEFAULT now() NOT NULL,
    delete_mark_time timestamp with time zone,
    delete_name text,
    delete_time timestamp with time zone,
    restore_time timestamp with time zone
);


--
-- TOC entry 2482 (class 0 OID 0)
-- Dependencies: 191
-- Name: TABLE rz_character; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_character IS 'I''ve deleted DIE column which was originally exist in MAIET''s RZ_GAMEDB.
So use health value to determine DIE or not.

Use rz_char_insert() to create a character, rz_char_delete() to delete a character.
Optionally, you may want to run rz_char_delete_marked() regularly (refer comment of the function for more info).

active_palette_id: One ID from three selectable palette tabs that user is currently using. (i.e. Shift+1, Shift+2 and Shift+3)
primary_palette_id and secondary_palette_id:
  This ID automatically overwrite active_palette_id when user was associated palette tab to primary/secondary weapon
  and user switched the weapon.
  Set this value to 5 (PALETTE_NUM::PALETTENUM_MAX) to deassociate.';


--
-- TOC entry 192 (class 1259 OID 28823)
-- Name: rz_character_char_sn_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE rz_character_char_sn_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- TOC entry 2483 (class 0 OID 0)
-- Dependencies: 192
-- Name: rz_character_char_sn_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE rz_character_char_sn_seq OWNED BY rz_character.char_sn;


--
-- TOC entry 193 (class 1259 OID 28825)
-- Name: rz_cutscene; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_cutscene (
    char_sn bigint NOT NULL,
    cutscn_id integer NOT NULL
);


--
-- TOC entry 194 (class 1259 OID 28828)
-- Name: rz_emblem; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_emblem (
    char_sn bigint NOT NULL,
    emblem_id integer NOT NULL,
    selected boolean DEFAULT false NOT NULL
);


--
-- TOC entry 195 (class 1259 OID 28832)
-- Name: rz_faction; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_faction (
    char_sn bigint NOT NULL,
    fact_id integer NOT NULL,
    val integer DEFAULT 28000 NOT NULL
);


--
-- TOC entry 196 (class 1259 OID 28836)
-- Name: rz_gem_enchant; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_gem_enchant (
    gem_ench_id bigint NOT NULL,
    attached_item_id integer[] DEFAULT '{0,0,0,0,0,0}'::integer[] NOT NULL
);


--
-- TOC entry 2484 (class 0 OID 0)
-- Dependencies: 196
-- Name: TABLE rz_gem_enchant; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_gem_enchant IS 'This table stores gear-attached jewels. attached_item_id is array of attached jewel IDs.

e.g.
  Teneth''s Dagger has 1 special jewel socket and 4 jewel sockets.
  I have attached Pyroxene of Agility to all normal pieces, and Shanith''s Diamond to a special piece.

  So the array now must be:
  ''{32511302, 30000701, 30000701, 30000701, 30000701}''';


--
-- TOC entry 197 (class 1259 OID 28843)
-- Name: rz_guidebook; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_guidebook (
    char_sn bigint NOT NULL,
    book_id integer NOT NULL
);


--
-- TOC entry 198 (class 1259 OID 28846)
-- Name: rz_palette; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_palette (
    char_sn bigint NOT NULL,
    palette_set smallint NOT NULL,
    regist_place smallint NOT NULL,
    entry_id integer NOT NULL,
    entry_type rz_palette_entry_type NOT NULL
);


--
-- TOC entry 199 (class 1259 OID 28849)
-- Name: rz_quest; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_quest (
    char_sn bigint NOT NULL,
    entry_num smallint NOT NULL,
    quest_id integer NOT NULL,
    variable integer DEFAULT 0 NOT NULL,
    state rz_quest_state DEFAULT 'IN_PROGRESS'::rz_quest_state NOT NULL,
    accept_time timestamp with time zone DEFAULT now() NOT NULL,
    time_limit interval,
    objective_progress integer[] DEFAULT '{0,0,0,0,0}'::integer[] NOT NULL
);


--
-- TOC entry 2485 (class 0 OID 0)
-- Dependencies: 199
-- Name: TABLE rz_quest; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_quest IS 'variable column: is a global variable associated with a quest, and handled by GameServer.';


--
-- TOC entry 200 (class 1259 OID 28859)
-- Name: rz_quest_history; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_quest_history (
    quest_sn bigint NOT NULL,
    char_sn bigint NOT NULL,
    quest_id integer NOT NULL,
    achieve_time timestamp with time zone DEFAULT now() NOT NULL,
    next_reset_time timestamp with time zone
);


--
-- TOC entry 201 (class 1259 OID 28863)
-- Name: rz_quest_history_quest_sn_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE rz_quest_history_quest_sn_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- TOC entry 2486 (class 0 OID 0)
-- Dependencies: 201
-- Name: rz_quest_history_quest_sn_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE rz_quest_history_quest_sn_seq OWNED BY rz_quest_history.quest_sn;


--
-- TOC entry 202 (class 1259 OID 28865)
-- Name: rz_recipe; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_recipe (
    char_sn bigint NOT NULL,
    recp_id integer NOT NULL
);


--
-- TOC entry 203 (class 1259 OID 28868)
-- Name: rz_talent; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_talent (
    char_sn bigint NOT NULL,
    talent_set smallint NOT NULL,
    talent_id integer NOT NULL,
    master_time timestamp with time zone DEFAULT now() NOT NULL
);


--
-- TOC entry 204 (class 1259 OID 28872)
-- Name: rz_talent_cooltime; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_talent_cooltime (
    char_sn bigint NOT NULL,
    talent_id integer NOT NULL,
    added_time timestamp with time zone DEFAULT now() NOT NULL,
    remained_duration interval NOT NULL
);


--
-- TOC entry 205 (class 1259 OID 28876)
-- Name: rz_user_item; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_user_item (
    item_sn bigint NOT NULL,
    item_id integer,
    owner_sn bigint,
    stack_amount smallint DEFAULT 1 NOT NULL,
    durability smallint DEFAULT 0 NOT NULL,
    max_durability smallint,
    color integer DEFAULT '-1'::integer NOT NULL,
    enchant_grade smallint DEFAULT 0 NOT NULL,
    tied boolean DEFAULT false NOT NULL,
    gem_ench_id bigint,
    status rz_user_item_status DEFAULT 'CIRCULATING'::rz_user_item_status NOT NULL,
    reg_time timestamp with time zone DEFAULT now() NOT NULL,
    activate_time timestamp with time zone,
    expire_in interval
);


--
-- TOC entry 2487 (class 0 OID 0)
-- Dependencies: 205
-- Name: TABLE rz_user_item; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_user_item IS 'This table stores all user-generated items. (i.e. by Crafting, Looting or Quest Rewards etc...)

tied column means the item is bound to character or not.
The item expires on calculation of (activate_time + expire_in). Leave expire_in NULL for unlimited duration.

Use rz_item_check_durability() function to check the item is broken or not.
(Set max_durability NULL for unlimited durability.)';


--
-- TOC entry 206 (class 1259 OID 28886)
-- Name: rz_user_item_item_sn_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE rz_user_item_item_sn_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- TOC entry 2488 (class 0 OID 0)
-- Dependencies: 206
-- Name: rz_user_item_item_sn_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE rz_user_item_item_sn_seq OWNED BY rz_user_item.item_sn;


--
-- TOC entry 207 (class 1259 OID 28888)
-- Name: rz_valid_character; Type: VIEW; Schema: public; Owner: -
--

CREATE VIEW rz_valid_character AS
 SELECT rz_character.char_sn,
    rz_character.accn_sn,
    rz_character.style,
    rz_character.preference,
    rz_character.player_grade,
    rz_character.name,
    rz_character.level,
    rz_character.exp,
    rz_character.money,
    rz_character.storage_money,
    rz_character.health,
    rz_character.energy,
    rz_character.stamina,
    rz_character.remain_tp,
    rz_character.active_skill_set,
    rz_character.fatigue,
    rz_character.inn_id,
    rz_character.inn_room_id,
    rz_character.primary_palette_id,
    rz_character.secondary_palette_id,
    rz_character.active_palette_id,
    rz_character.weapon_set,
    rz_character.pos,
    rz_character.dir,
    rz_character.shared_field_id,
    rz_character.entrance_pos,
    rz_character.dynamic_field_group_uid,
    rz_character.dynamic_field_id,
    rz_character.checkpoint_field_id,
    rz_character.checkpoint_marker_id,
    rz_character.party_uid,
    rz_character.arena_uid,
    rz_character.arena_team_uid,
    rz_character.play_time,
    rz_character.dead_time,
    rz_character.disconn_time,
    rz_character.create_time,
    rz_character.delete_mark_time,
    rz_character.delete_name,
    rz_character.delete_time,
    rz_character.restore_time
   FROM rz_character
  WHERE (rz_character.delete_mark_time IS NULL);


--
-- TOC entry 2489 (class 0 OID 0)
-- Dependencies: 207
-- Name: VIEW rz_valid_character; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON VIEW rz_valid_character IS 'This view wraps access to rz_character table with deletion-marked characters are removed from result set.';


--
-- TOC entry 2210 (class 2604 OID 28893)
-- Name: char_sn; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_character ALTER COLUMN char_sn SET DEFAULT nextval('rz_character_char_sn_seq'::regclass);


--
-- TOC entry 2219 (class 2604 OID 28894)
-- Name: quest_sn; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_quest_history ALTER COLUMN quest_sn SET DEFAULT nextval('rz_quest_history_quest_sn_seq'::regclass);


--
-- TOC entry 2229 (class 2604 OID 28895)
-- Name: item_sn; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_user_item ALTER COLUMN item_sn SET DEFAULT nextval('rz_user_item_item_sn_seq'::regclass);


SET search_path = const_def, pg_catalog;

--
-- TOC entry 2242 (class 2606 OID 28897)
-- Name: rz_default_equipment_pkey; Type: CONSTRAINT; Schema: const_def; Owner: -
--

ALTER TABLE ONLY rz_default_equipment
    ADD CONSTRAINT rz_default_equipment_pkey PRIMARY KEY (style_id, slot_num, place);


--
-- TOC entry 2244 (class 2606 OID 28899)
-- Name: rz_default_talent_pkey; Type: CONSTRAINT; Schema: const_def; Owner: -
--

ALTER TABLE ONLY rz_default_talent
    ADD CONSTRAINT rz_default_talent_pkey PRIMARY KEY (style_id, palette_place);


--
-- TOC entry 2246 (class 2606 OID 28901)
-- Name: rz_default_talent_style_id_talent_id_key; Type: CONSTRAINT; Schema: const_def; Owner: -
--

ALTER TABLE ONLY rz_default_talent
    ADD CONSTRAINT rz_default_talent_style_id_talent_id_key UNIQUE (style_id, talent_id);


--
-- TOC entry 2248 (class 2606 OID 28903)
-- Name: rz_xitem_pkey; Type: CONSTRAINT; Schema: const_def; Owner: -
--

ALTER TABLE ONLY rz_xitem
    ADD CONSTRAINT rz_xitem_pkey PRIMARY KEY (id);


SET search_path = public, pg_catalog;

--
-- TOC entry 2292 (class 2606 OID 29158)
-- Name: rz_buff_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_buff
    ADD CONSTRAINT rz_buff_pkey PRIMARY KEY (char_sn, buff_id);


--
-- TOC entry 2262 (class 2606 OID 28907)
-- Name: rz_character_name_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_character
    ADD CONSTRAINT rz_character_name_key UNIQUE (name);


--
-- TOC entry 2264 (class 2606 OID 28909)
-- Name: rz_character_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_character
    ADD CONSTRAINT rz_character_pkey PRIMARY KEY (char_sn);


--
-- TOC entry 2266 (class 2606 OID 28911)
-- Name: rz_cutscene_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_cutscene
    ADD CONSTRAINT rz_cutscene_pkey PRIMARY KEY (char_sn, cutscn_id);


--
-- TOC entry 2268 (class 2606 OID 28913)
-- Name: rz_emblem_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_emblem
    ADD CONSTRAINT rz_emblem_pkey PRIMARY KEY (char_sn, emblem_id);


--
-- TOC entry 2250 (class 2606 OID 28915)
-- Name: rz_equipment_item_sn_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_equipment
    ADD CONSTRAINT rz_equipment_item_sn_key UNIQUE (item_sn) DEFERRABLE;


--
-- TOC entry 2252 (class 2606 OID 28918)
-- Name: rz_equipment_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_equipment
    ADD CONSTRAINT rz_equipment_pkey PRIMARY KEY (owner_sn, slot_num);


--
-- TOC entry 2270 (class 2606 OID 28920)
-- Name: rz_faction_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_faction
    ADD CONSTRAINT rz_faction_pkey PRIMARY KEY (char_sn, fact_id);


--
-- TOC entry 2272 (class 2606 OID 28929)
-- Name: rz_gem_enchant_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_gem_enchant
    ADD CONSTRAINT rz_gem_enchant_pkey PRIMARY KEY (gem_ench_id);


--
-- TOC entry 2274 (class 2606 OID 28922)
-- Name: rz_guidebook_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_guidebook
    ADD CONSTRAINT rz_guidebook_pkey PRIMARY KEY (char_sn, book_id);


--
-- TOC entry 2254 (class 2606 OID 28924)
-- Name: rz_inventory_item_sn_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_inventory
    ADD CONSTRAINT rz_inventory_item_sn_key UNIQUE (item_sn) DEFERRABLE;


--
-- TOC entry 2256 (class 2606 OID 28927)
-- Name: rz_inventory_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_inventory
    ADD CONSTRAINT rz_inventory_pkey PRIMARY KEY (owner_sn, slot_num);


--
-- TOC entry 2276 (class 2606 OID 28931)
-- Name: rz_palette_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_palette
    ADD CONSTRAINT rz_palette_pkey PRIMARY KEY (char_sn, palette_set, regist_place);


--
-- TOC entry 2278 (class 2606 OID 28933)
-- Name: rz_quest_char_sn_quest_id_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_quest
    ADD CONSTRAINT rz_quest_char_sn_quest_id_key UNIQUE (char_sn, quest_id);


--
-- TOC entry 2282 (class 2606 OID 28935)
-- Name: rz_quest_history_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_quest_history
    ADD CONSTRAINT rz_quest_history_pkey PRIMARY KEY (quest_sn);


--
-- TOC entry 2280 (class 2606 OID 28937)
-- Name: rz_quest_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_quest
    ADD CONSTRAINT rz_quest_pkey PRIMARY KEY (char_sn, entry_num);


--
-- TOC entry 2284 (class 2606 OID 28939)
-- Name: rz_recipe_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_recipe
    ADD CONSTRAINT rz_recipe_pkey PRIMARY KEY (char_sn, recp_id);


--
-- TOC entry 2258 (class 2606 OID 28941)
-- Name: rz_storage_item_sn_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_storage
    ADD CONSTRAINT rz_storage_item_sn_key UNIQUE (item_sn) DEFERRABLE;


--
-- TOC entry 2260 (class 2606 OID 28944)
-- Name: rz_storage_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_storage
    ADD CONSTRAINT rz_storage_pkey PRIMARY KEY (owner_sn, slot_num);


--
-- TOC entry 2288 (class 2606 OID 28948)
-- Name: rz_talent_cooltime_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_talent_cooltime
    ADD CONSTRAINT rz_talent_cooltime_pkey PRIMARY KEY (char_sn, talent_id);


--
-- TOC entry 2286 (class 2606 OID 28946)
-- Name: rz_talent_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_talent
    ADD CONSTRAINT rz_talent_pkey PRIMARY KEY (char_sn, talent_set, talent_id);


--
-- TOC entry 2290 (class 2606 OID 28950)
-- Name: rz_user_item_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_user_item
    ADD CONSTRAINT rz_user_item_pkey PRIMARY KEY (item_sn);


--
-- TOC entry 2312 (class 2620 OID 28951)
-- Name: rz_character_on_delete_trigger; Type: TRIGGER; Schema: public; Owner: -
--

CREATE TRIGGER rz_character_on_delete_trigger BEFORE UPDATE OF delete_time ON rz_character FOR EACH ROW WHEN (((old.delete_time IS NULL) AND (new.delete_time IS NOT NULL))) EXECUTE PROCEDURE rz_char_on_delete();


--
-- TOC entry 2313 (class 2620 OID 28952)
-- Name: rz_character_on_mark_delete_trigger; Type: TRIGGER; Schema: public; Owner: -
--

CREATE TRIGGER rz_character_on_mark_delete_trigger BEFORE UPDATE OF delete_mark_time ON rz_character FOR EACH ROW WHEN (((old.delete_mark_time IS NULL) AND (new.delete_mark_time IS NOT NULL))) EXECUTE PROCEDURE rz_char_on_mark_delete();


--
-- TOC entry 2311 (class 2606 OID 29159)
-- Name: rz_buff_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_buff
    ADD CONSTRAINT rz_buff_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2299 (class 2606 OID 28958)
-- Name: rz_cutscene_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_cutscene
    ADD CONSTRAINT rz_cutscene_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2300 (class 2606 OID 28963)
-- Name: rz_emblem_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_emblem
    ADD CONSTRAINT rz_emblem_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2293 (class 2606 OID 28968)
-- Name: rz_equipment_item_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_equipment
    ADD CONSTRAINT rz_equipment_item_sn_fkey FOREIGN KEY (item_sn) REFERENCES rz_user_item(item_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2294 (class 2606 OID 28973)
-- Name: rz_equipment_owner_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_equipment
    ADD CONSTRAINT rz_equipment_owner_sn_fkey FOREIGN KEY (owner_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2301 (class 2606 OID 28978)
-- Name: rz_faction_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_faction
    ADD CONSTRAINT rz_faction_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2302 (class 2606 OID 28983)
-- Name: rz_guidebook_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_guidebook
    ADD CONSTRAINT rz_guidebook_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2295 (class 2606 OID 28988)
-- Name: rz_inventory_item_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_inventory
    ADD CONSTRAINT rz_inventory_item_sn_fkey FOREIGN KEY (item_sn) REFERENCES rz_user_item(item_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2296 (class 2606 OID 28993)
-- Name: rz_inventory_owner_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_inventory
    ADD CONSTRAINT rz_inventory_owner_sn_fkey FOREIGN KEY (owner_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2303 (class 2606 OID 28998)
-- Name: rz_palette_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_palette
    ADD CONSTRAINT rz_palette_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2304 (class 2606 OID 29003)
-- Name: rz_quest_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_quest
    ADD CONSTRAINT rz_quest_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2305 (class 2606 OID 29008)
-- Name: rz_quest_history_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_quest_history
    ADD CONSTRAINT rz_quest_history_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2306 (class 2606 OID 29013)
-- Name: rz_recipe_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_recipe
    ADD CONSTRAINT rz_recipe_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2297 (class 2606 OID 29018)
-- Name: rz_storage_item_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_storage
    ADD CONSTRAINT rz_storage_item_sn_fkey FOREIGN KEY (item_sn) REFERENCES rz_user_item(item_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2298 (class 2606 OID 29023)
-- Name: rz_storage_owner_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_storage
    ADD CONSTRAINT rz_storage_owner_sn_fkey FOREIGN KEY (owner_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2307 (class 2606 OID 29028)
-- Name: rz_talent_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_talent
    ADD CONSTRAINT rz_talent_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2308 (class 2606 OID 29033)
-- Name: rz_talent_cooltime_char_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_talent_cooltime
    ADD CONSTRAINT rz_talent_cooltime_char_sn_fkey FOREIGN KEY (char_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE CASCADE;


--
-- TOC entry 2309 (class 2606 OID 29038)
-- Name: rz_user_item_gem_ench_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_user_item
    ADD CONSTRAINT rz_user_item_gem_ench_id_fkey FOREIGN KEY (gem_ench_id) REFERENCES rz_gem_enchant(gem_ench_id) ON UPDATE CASCADE ON DELETE SET NULL;


--
-- TOC entry 2310 (class 2606 OID 29043)
-- Name: rz_user_item_owner_sn_fkey; Type: FK CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_user_item
    ADD CONSTRAINT rz_user_item_owner_sn_fkey FOREIGN KEY (owner_sn) REFERENCES rz_character(char_sn) ON UPDATE CASCADE ON DELETE SET NULL;


-- Completed on 2016-01-23 12:15:32

--
-- PostgreSQL database dump complete
--

