--
-- PostgreSQL database dump
--

-- Dumped from database version 9.5.0
-- Dumped by pg_dump version 9.5.0

-- Started on 2016-01-23 12:15:01

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- TOC entry 2176 (class 1262 OID 16396)
-- Dependencies: 2175
-- Name: rz_accountdb_test; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON DATABASE rz_accountdb_test IS 'RZ_ACCOUNTDB:
  This DB stores all user''s account info and all server''s info; and commonly used by all separate worlds.
  (Just register an account into this DB to gain accesses to all worlds.)
';


--
-- TOC entry 186 (class 3079 OID 12355)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2178 (class 0 OID 0)
-- Dependencies: 186
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 567 (class 1247 OID 16780)
-- Name: rz_account_status; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_account_status AS ENUM (
    'ACTIVE',
    'INACTIVE',
    'SUSPENDED',
    'BLOCKED',
    'UNREGISTERED'
);


--
-- TOC entry 2179 (class 0 OID 0)
-- Dependencies: 567
-- Name: TYPE rz_account_status; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TYPE rz_account_status IS 'ACTIVE: Account is fine and ready to use.
INACTIVE: Account is waiting for activation (e.g. Confirm by email).
SUSPENDED: Account is invalidated because not been used for a long (Reactivation is required).
BLOCKED: Account is banned for violation against rules.
UNREGISTERED: Account is deleted due to user''s request';


--
-- TOC entry 570 (class 1247 OID 16792)
-- Name: rz_server_state; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_server_state AS ENUM (
    'FINE',
    'MAINTENANCE',
    'CLOSED'
);


--
-- TOC entry 2180 (class 0 OID 0)
-- Dependencies: 570
-- Name: TYPE rz_server_state; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TYPE rz_server_state IS 'FINE: the server is publicly available.
MAINTENANCE: the server is under maintenance.
CLOSED: the server is shut down and no longer available.';


--
-- TOC entry 573 (class 1247 OID 16800)
-- Name: rz_server_type; Type: TYPE; Schema: public; Owner: -
--

CREATE TYPE rz_server_type AS ENUM (
    'LoginServer',
    'MasterServer',
    'GameServer',
    'AppServer'
);


--
-- TOC entry 187 (class 1255 OID 16809)
-- Name: rz_account_insert(text, text, text); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_account_insert(p_username text, p_password text, p_email text DEFAULT NULL::text, OUT "IS_MADE" boolean, OUT "ACCN_SN" bigint, OUT "CONN_SN" bigint) RETURNS record
    LANGUAGE plpgsql
    AS $$DECLARE
  v_accn_sn bigint;
  c_conn_sn CONSTANT bigint NOT NULL DEFAULT rz_gen_conn_sn();
    
BEGIN
  IF strpos(p_username, ' ') <> 0 THEN
    SELECT FALSE, 0, 0 INTO "IS_MADE", "ACCN_SN", "CONN_SN";
    RETURN;
  END IF;
  
  p_username := lower(p_username);
  
  SELECT accn_sn INTO v_accn_sn FROM rz_account WHERE username = p_username;

  IF FOUND THEN
    SELECT FALSE, v_accn_sn, c_conn_sn INTO "IS_MADE", "ACCN_SN", "CONN_SN";
    RETURN;
  END IF;

  INSERT INTO rz_account (username, password, email) VALUES (p_username, p_password, p_email)
    RETURNING accn_sn INTO v_accn_sn;

  SELECT TRUE, v_accn_sn, c_conn_sn INTO "IS_MADE", "ACCN_SN", "CONN_SN";
END;$$;


--
-- TOC entry 2181 (class 0 OID 0)
-- Dependencies: 187
-- Name: FUNCTION rz_account_insert(p_username text, p_password text, p_email text, OUT "IS_MADE" boolean, OUT "ACCN_SN" bigint, OUT "CONN_SN" bigint); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_account_insert(p_username text, p_password text, p_email text, OUT "IS_MADE" boolean, OUT "ACCN_SN" bigint, OUT "CONN_SN" bigint) IS 'This function is called when the user''s account was not exist and the LoginServer is configurated to automatically create an account.
You may also want to use this function by your own registration web page.

Inputs are:
  p_username: User ID you wish to register. the ID can''t contain spaces (in such case, set of zero values returned).
  p_password: Plain text password you wish to use.
  p_email: Email address of the user. Leave this NULL if not needed.

Outputs are:
  IS_MADE: If the account is successfully made, then TRUE. but if another account with same username existing, then FALSE.
  ACCN_SN: ACCN_SN of newely made. or existing ACCN_SN when not made.
  CONN_SN: Assigned CONN_SN.';


--
-- TOC entry 209 (class 1255 OID 16810)
-- Name: rz_account_update_reg_date(bigint); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_account_update_reg_date(p_accn_sn bigint) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_account
   SET first_char_create_time = CURRENT_TIMESTAMP
 WHERE accn_sn = p_accn_sn
   AND first_char_create_time IS NULL;$$;


--
-- TOC entry 200 (class 1255 OID 16811)
-- Name: rz_gen_conn_sn(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_gen_conn_sn() RETURNS bigint
    LANGUAGE sql
    AS $$SELECT nextval('rz_conn_sn_seq');$$;


--
-- TOC entry 2182 (class 0 OID 0)
-- Dependencies: 200
-- Name: FUNCTION rz_gen_conn_sn(); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_gen_conn_sn() IS 'See comment on rz_conn_sn_seq for more info.';


--
-- TOC entry 203 (class 1255 OID 16812)
-- Name: rz_login_get_info(text); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_login_get_info(p_username text, OUT "ACCN_SN" bigint, OUT "PWD" text, OUT "NEW_ACC" boolean, OUT "CONN_SN" bigint, OUT "DEF_WORLD_ID" integer, OUT "DEF_CHAR_SN" bigint, OUT "STATUS" rz_account_status) RETURNS SETOF record
    LANGUAGE sql ROWS 1
    AS $$SELECT accn_sn,
       password,
       (first_char_create_time IS NULL)::boolean,
       rz_gen_conn_sn(),
       def_world_id,
       def_char_sn,
       status
  FROM rz_account
 WHERE username = lower(p_username);$$;


--
-- TOC entry 2183 (class 0 OID 0)
-- Dependencies: 203
-- Name: FUNCTION rz_login_get_info(p_username text, OUT "ACCN_SN" bigint, OUT "PWD" text, OUT "NEW_ACC" boolean, OUT "CONN_SN" bigint, OUT "DEF_WORLD_ID" integer, OUT "DEF_CHAR_SN" bigint, OUT "STATUS" rz_account_status); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_login_get_info(p_username text, OUT "ACCN_SN" bigint, OUT "PWD" text, OUT "NEW_ACC" boolean, OUT "CONN_SN" bigint, OUT "DEF_WORLD_ID" integer, OUT "DEF_CHAR_SN" bigint, OUT "STATUS" rz_account_status) IS 'Inputs are:
  p_username: Information with this username will be returned.

Outputs are:
  ACCN_SN: Unique ID number for this account.
  PWD: Password of this account. Compare this value with user''s password input to check the password is correct or not.
  NEW_ACC: If the user never made a character even once, this will return TRUE. otherwise FALSE.
  CONN_SN: Check comment written in rz_conn_sn_seq for more info.
  DEF_WORLD_ID: Default WORLD_ID. You can use this ID for a purpose like automatically select a world by system.
  DEF_CHAR_SN: Default CHAR_SN. CHAR_SN differs between worlds. So this parameter should be used with DEF_WORLD_ID together.
  STATUS: Account status. See a comment on rz_account_status for more info.

If the account is found, return a row. Otherwise, no rows returned.';


--
-- TOC entry 204 (class 1255 OID 16813)
-- Name: rz_server_get_status_info(integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_server_get_status_info(p_world_id integer, p_server_id integer, OUT "IS_RUN" boolean, OUT "SERVABLE" boolean) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 1
    AS $$SELECT ((CURRENT_TIMESTAMP - ss.last_update_time) < s.alive_timeout)::boolean,
       ss.servable
  FROM rz_server        AS s,
       rz_server_status AS ss
 WHERE s.server_id = ss.server_id
   AND s.world_id  = ss.world_id
   AND s.server_id = p_server_id
   AND s.world_id  = p_world_id;$$;


--
-- TOC entry 2184 (class 0 OID 0)
-- Dependencies: 204
-- Name: FUNCTION rz_server_get_status_info(p_world_id integer, p_server_id integer, OUT "IS_RUN" boolean, OUT "SERVABLE" boolean); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_server_get_status_info(p_world_id integer, p_server_id integer, OUT "IS_RUN" boolean, OUT "SERVABLE" boolean) IS 'IS_RUN: If this was FALSE, then something wrong happened to server. e.g. crash, hang or lost internet connection etc.
SERVABLE: This will be TRUE when the server is self-proclaimed I''m ready to do my jobs.

Usually, these both values must be TRUE. If not, the server is not working or inproperly configured.';


--
-- TOC entry 210 (class 1255 OID 24619)
-- Name: rz_server_start(integer, integer, text, text, text, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_server_start(p_world_id integer, p_server_id integer, p_name text, p_version text, p_host_address text, p_port integer, p_type integer, p_max_players_capacity integer, p_update_elapsed_time integer, p_allow_delay_time integer) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_server (server_id,
                       world_id,
                       name,
                       version,
                       address,
                       port,
                       type,
                       max_players_capacity,
                       alive_timeout)
VALUES (p_server_id,
        p_world_id,
        p_name,
        p_version,
        p_host_address,
        p_port, 
        rz_server_type_by_num(p_type),
        p_max_players_capacity,
        ((p_update_elapsed_time + p_allow_delay_time) || ' seconds')::interval)
ON CONFLICT DO NOTHING;

INSERT INTO rz_server_status (server_id,
                              world_id,
                              start_time,
                              last_update_time)
VALUES (p_server_id,
        p_world_id,
        CURRENT_TIMESTAMP,
        CURRENT_TIMESTAMP)
ON CONFLICT (server_id, world_id)
DO UPDATE SET start_time       = EXCLUDED.start_time,
              last_update_time = EXCLUDED.last_update_time,
              servable         = FALSE,
              task_count       = 0,
              cpu_usage        = 0,
              memory_usage     = 0,
              fps              = 0,
              field_count      = 0,
              shutdown_state   = 0;$$;


--
-- TOC entry 205 (class 1255 OID 16815)
-- Name: rz_server_type_by_num(integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_server_type_by_num(type integer) RETURNS rz_server_type
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (CASE type
          WHEN 0 THEN 'LoginServer'
          WHEN 1 THEN 'MasterServer'
          WHEN 2 THEN 'GameServer'
          WHEN 3 THEN 'AppServer'
        END)::rz_server_type;$$;


--
-- TOC entry 2185 (class 0 OID 0)
-- Dependencies: 205
-- Name: FUNCTION rz_server_type_by_num(type integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_server_type_by_num(type integer) IS 'NOTE: If you altered values of this function, then alter rz_server_type_to_num() too!';


--
-- TOC entry 206 (class 1255 OID 16816)
-- Name: rz_server_type_to_num(rz_server_type); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_server_type_to_num(p_server_type rz_server_type) RETURNS integer
    LANGUAGE sql IMMUTABLE
    AS $$SELECT (CASE p_server_type
          WHEN 'LoginServer'  THEN 0
          WHEN 'MasterServer' THEN 1
          WHEN 'GameServer'   THEN 2
          WHEN 'AppServer'    THEN 3
        END)::integer;$$;


--
-- TOC entry 2186 (class 0 OID 0)
-- Dependencies: 206
-- Name: FUNCTION rz_server_type_to_num(p_server_type rz_server_type); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_server_type_to_num(p_server_type rz_server_type) IS 'NOTE: If you altered values of this function, then alter rz_server_type_by_num() too!';


--
-- TOC entry 211 (class 1255 OID 24587)
-- Name: rz_server_update(integer, integer, integer, boolean, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_server_update(p_world_id integer, p_server_id integer, p_active_players integer, p_servable boolean, p_task_count integer, p_cpu_usage integer, p_memory_usage integer, p_field_count integer, p_fps integer) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_server_status (server_id,
                              world_id,
                              start_time,
                              active_players,
                              servable,
                              task_count,
                              cpu_usage,
                              memory_usage,
                              fps,
                              field_count)
VALUES (p_server_id,
        p_world_id,
        CURRENT_TIMESTAMP,
        p_active_players,
        p_servable,
        p_task_count,
        p_cpu_usage,
        p_memory_usage,
        p_fps,
        p_field_count)
ON CONFLICT (server_id, world_id)
DO UPDATE SET active_players   = EXCLUDED.active_players,
              last_update_time = EXCLUDED.last_update_time,
              servable         = EXCLUDED.servable,
              task_count       = EXCLUDED.task_count,
              cpu_usage        = EXCLUDED.cpu_usage,
              memory_usage     = EXCLUDED.memory_usage,
              fps              = EXCLUDED.fps,
              field_count      = EXCLUDED.field_count;$$;


--
-- TOC entry 207 (class 1255 OID 16818)
-- Name: rz_world_get_list(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_world_get_list(OUT "WORLD_ID" integer, OUT "NAME" text, OUT "IP" text, OUT "TYPE" integer, OUT "CUR_PLAYER_COUNT" integer, OUT "MAX_PLAYER_COUNT" integer, OUT "SERVABLE" boolean, OUT "EXPIRE" boolean, OUT "STATE" rz_server_state, OUT "ORD_NUM" integer) RETURNS SETOF record
    LANGUAGE sql STABLE ROWS 5
    AS $$SELECT world_id,
       name,
       host_address, 
       rz_server_type_to_num(type),
       cur_players,
       max_players,
       servable,
       ((CURRENT_TIMESTAMP - last_update_time) >= alive_timeout)::boolean,
       state,
       order_num
  FROM rz_world
 WHERE state <> 'CLOSED'
 ORDER BY order_num, world_id, server_id;$$;


--
-- TOC entry 212 (class 1255 OID 24593)
-- Name: rz_world_insert(integer, text, text, integer, integer, boolean, integer, integer); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_world_insert(p_world_id integer, p_name text, p_host_address text, p_type integer, p_max_players_capacity integer, p_servable boolean, p_state integer, p_alive_timeout integer) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_world (world_id,
                      name,
                      host_address,
                      type,
                      max_players, 
                      last_update_time,
                      servable,
                      state,
                      alive_timeout)
VALUES (p_world_id,
        p_name,
        p_host_address,
        rz_server_type_by_num(p_type),
        p_max_players_capacity,
        CURRENT_TIMESTAMP,
        p_servable,
        'FINE',
        (p_alive_timeout || ' seconds')::interval)
ON CONFLICT (server_id, world_id)
DO UPDATE SET name             = EXCLUDED.name,
              host_address     = EXCLUDED.host_address,
              type             = EXCLUDED.type,
              max_players      = EXCLUDED.max_players,
              last_update_time = EXCLUDED.last_update_time,
              servable         = EXCLUDED.servable,
              state            = EXCLUDED.state,
              alive_timeout    = EXCLUDED.alive_timeout;$$;


--
-- TOC entry 2187 (class 0 OID 0)
-- Dependencies: 212
-- Name: FUNCTION rz_world_insert(p_world_id integer, p_name text, p_host_address text, p_type integer, p_max_players_capacity integer, p_servable boolean, p_state integer, p_alive_timeout integer); Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON FUNCTION rz_world_insert(p_world_id integer, p_name text, p_host_address text, p_type integer, p_max_players_capacity integer, p_servable boolean, p_state integer, p_alive_timeout integer) IS 'TODO: properly insert server state.';


--
-- TOC entry 201 (class 1255 OID 16820)
-- Name: rz_world_set_maintenance(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_world_set_maintenance() RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_world SET state = 'MAINTENANCE' WHERE state = 'FINE';$$;


--
-- TOC entry 202 (class 1255 OID 16821)
-- Name: rz_world_unset_maintenance(); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_world_unset_maintenance() RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_world SET state = 'FINE' WHERE state = 'MAINTENANCE';$$;


--
-- TOC entry 208 (class 1255 OID 16822)
-- Name: rz_world_update(integer, integer, boolean); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_world_update(p_world_id integer, p_cur_players integer, p_servable boolean) RETURNS void
    LANGUAGE sql
    AS $$UPDATE rz_world 
   SET   cur_players      = p_cur_players,
         servable         = p_servable,
         last_update_time = CURRENT_TIMESTAMP
 WHERE world_id = p_world_id;$$;


SET default_with_oids = false;

--
-- TOC entry 180 (class 1259 OID 16823)
-- Name: rz_account; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_account (
    accn_sn bigint NOT NULL,
    username text NOT NULL,
    password text NOT NULL,
    email text,
    def_world_id integer,
    def_char_sn bigint,
    play_time interval DEFAULT '00:00:00'::interval NOT NULL,
    reg_time timestamp with time zone DEFAULT now() NOT NULL,
    status rz_account_status DEFAULT 'ACTIVE'::rz_account_status NOT NULL,
    last_status_update timestamp with time zone DEFAULT now() NOT NULL,
    last_play_time timestamp with time zone,
    last_play_world_id integer,
    last_play_char_sn bigint,
    first_char_create_time timestamp with time zone,
    cash integer DEFAULT 0 NOT NULL
);


--
-- TOC entry 2188 (class 0 OID 0)
-- Dependencies: 180
-- Name: TABLE rz_account; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON TABLE rz_account IS 'ToDo: do not store account password as plain text.

Use rz_account_insert() to create an account.';


--
-- TOC entry 181 (class 1259 OID 16834)
-- Name: rz_account_accn_sn_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE rz_account_accn_sn_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- TOC entry 2189 (class 0 OID 0)
-- Dependencies: 181
-- Name: rz_account_accn_sn_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE rz_account_accn_sn_seq OWNED BY rz_account.accn_sn;


--
-- TOC entry 182 (class 1259 OID 16836)
-- Name: rz_conn_sn_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE rz_conn_sn_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- TOC entry 2190 (class 0 OID 0)
-- Dependencies: 182
-- Name: SEQUENCE rz_conn_sn_seq; Type: COMMENT; Schema: public; Owner: -
--

COMMENT ON SEQUENCE rz_conn_sn_seq IS 'CONN_SN is an unique serial number assigned for each user''s connections.

It''s used to determine which information should be updated on RZ_LOGDB.
(Log is created for each user''s sessions, not for each accounts or characters.)
';


--
-- TOC entry 184 (class 1259 OID 24594)
-- Name: rz_server; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_server (
    server_id integer NOT NULL,
    world_id integer NOT NULL,
    open_time timestamp with time zone DEFAULT now() NOT NULL,
    close_time timestamp with time zone,
    version text,
    name text,
    description text,
    type rz_server_type,
    address text,
    port integer,
    max_players_capacity integer DEFAULT 0 NOT NULL,
    alive_timeout interval DEFAULT '00:01:00'::interval NOT NULL,
    state rz_server_state DEFAULT 'FINE'::rz_server_state NOT NULL
);


--
-- TOC entry 185 (class 1259 OID 24606)
-- Name: rz_server_status; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_server_status (
    server_id integer NOT NULL,
    world_id integer NOT NULL,
    start_time timestamp with time zone,
    active_players integer DEFAULT 0 NOT NULL,
    last_update_time timestamp with time zone,
    servable boolean DEFAULT false NOT NULL,
    task_count integer DEFAULT 0 NOT NULL,
    cpu_usage integer DEFAULT 0 NOT NULL,
    memory_usage integer DEFAULT 0 NOT NULL,
    fps integer DEFAULT 0 NOT NULL,
    field_count integer DEFAULT 0 NOT NULL,
    shutdown_state integer DEFAULT 0 NOT NULL
);


--
-- TOC entry 183 (class 1259 OID 16859)
-- Name: rz_world; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_world (
    server_id integer NOT NULL,
    world_id integer NOT NULL,
    name text NOT NULL,
    host_address text NOT NULL,
    port integer DEFAULT 7501 NOT NULL,
    type rz_server_type DEFAULT 'LoginServer'::rz_server_type NOT NULL,
    cur_players integer DEFAULT 0 NOT NULL,
    max_players integer DEFAULT 0 NOT NULL,
    last_update_time timestamp with time zone,
    servable boolean DEFAULT false NOT NULL,
    alive_timeout interval DEFAULT '00:01:00'::interval NOT NULL,
    state rz_server_state DEFAULT 'FINE'::rz_server_state NOT NULL,
    order_num integer DEFAULT 0 NOT NULL
);


--
-- TOC entry 2026 (class 2604 OID 16873)
-- Name: accn_sn; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_account ALTER COLUMN accn_sn SET DEFAULT nextval('rz_account_accn_sn_seq'::regclass);


--
-- TOC entry 2048 (class 2606 OID 16875)
-- Name: rz_account_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_account
    ADD CONSTRAINT rz_account_pkey PRIMARY KEY (accn_sn);


--
-- TOC entry 2050 (class 2606 OID 16877)
-- Name: rz_account_username_key; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_account
    ADD CONSTRAINT rz_account_username_key UNIQUE (username);


--
-- TOC entry 2054 (class 2606 OID 24605)
-- Name: rz_server_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_server
    ADD CONSTRAINT rz_server_pkey PRIMARY KEY (server_id, world_id);


--
-- TOC entry 2056 (class 2606 OID 24618)
-- Name: rz_server_status_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_server_status
    ADD CONSTRAINT rz_server_status_pkey PRIMARY KEY (server_id, world_id);


--
-- TOC entry 2052 (class 2606 OID 16883)
-- Name: rz_world_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_world
    ADD CONSTRAINT rz_world_pkey PRIMARY KEY (server_id, world_id);


-- Completed on 2016-01-23 12:15:02

--
-- PostgreSQL database dump complete
--

