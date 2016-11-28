--
-- PostgreSQL database dump
--

-- Dumped from database version 9.5.0
-- Dumped by pg_dump version 9.5.0

-- Started on 2016-01-23 12:15:55

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET row_security = off;

--
-- TOC entry 2106 (class 1262 OID 16397)
-- Dependencies: 2105
-- Name: rz_logdb_test; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON DATABASE rz_logdb_test IS 'RZ_LOGDB:
  This DB stores server logs. You can put all logs here from every worlds (at your option).';


--
-- TOC entry 182 (class 3079 OID 12355)
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: -
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- TOC entry 2108 (class 0 OID 0)
-- Dependencies: 182
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: -
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

--
-- TOC entry 183 (class 1255 OID 17294)
-- Name: rz_conn_single_log(bigint, integer, integer, bigint, inet); Type: FUNCTION; Schema: public; Owner: -
--

CREATE FUNCTION rz_conn_single_log(p_conn_sn bigint, p_server_id integer, p_world_id integer, p_accn_sn bigint, p_conn_ip inet) RETURNS void
    LANGUAGE sql
    AS $$INSERT INTO rz_connect (conn_sn, server_id, world_id, accn_sn, conn_ip)
                VALUES (p_conn_sn, p_server_id, p_world_id, p_accn_sn, p_conn_ip);$$;


SET default_with_oids = false;

--
-- TOC entry 180 (class 1259 OID 17295)
-- Name: rz_connect; Type: TABLE; Schema: public; Owner: -
--

CREATE TABLE rz_connect (
    log_sn bigint NOT NULL,
    conn_sn bigint NOT NULL,
    server_id integer NOT NULL,
    world_id integer NOT NULL,
    accn_sn bigint NOT NULL,
    conn_ip inet NOT NULL,
    log_time timestamp with time zone DEFAULT now() NOT NULL
);


--
-- TOC entry 181 (class 1259 OID 17302)
-- Name: rz_connect_log_sn_seq; Type: SEQUENCE; Schema: public; Owner: -
--

CREATE SEQUENCE rz_connect_log_sn_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


--
-- TOC entry 2109 (class 0 OID 0)
-- Dependencies: 181
-- Name: rz_connect_log_sn_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: -
--

ALTER SEQUENCE rz_connect_log_sn_seq OWNED BY rz_connect.log_sn;


--
-- TOC entry 1984 (class 2604 OID 17304)
-- Name: log_sn; Type: DEFAULT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_connect ALTER COLUMN log_sn SET DEFAULT nextval('rz_connect_log_sn_seq'::regclass);


--
-- TOC entry 1986 (class 2606 OID 17306)
-- Name: rz_connect_pkey; Type: CONSTRAINT; Schema: public; Owner: -
--

ALTER TABLE ONLY rz_connect
    ADD CONSTRAINT rz_connect_pkey PRIMARY KEY (log_sn);


-- Completed on 2016-01-23 12:15:57

--
-- PostgreSQL database dump complete
--

