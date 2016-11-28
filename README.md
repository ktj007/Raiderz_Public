# FRaiderZ_Public
FRaiderZ Source Code

You will need:

*VS2015 Update 3

*NA RaiderZ Client (http://forum.ragezone.com/f698/release-raiderz-updated-version-client-1106956/) specifically https://mega.nz/#!AJZFmRgY!qGwFKTsMQfwqnUFiE_tisNfkZcA2xyvBpPDWENEVThY

*Data files from Develop/Utils folder (this is needed for the NA client to work with all changes.)

*Postgres SQL Database (with ODBC connector)

*Notepad++ (for the insane amount of text editing and searching you will do during development)

*MAIET SDK (See NA RaiderZ Client link)

*A cup of coffee and a lot of spare time

Open All.sln, and compile every project within. Output directory is Develop/Game/runtime/runtime for all projects, runtime can be used as a staging folder for client/server operations.
Configure server as if it were the NA files. Example GameServer file:

	; GameServer server.ini

	[SERVER_INFO]
	NAME = "GameServer1"
	SERVER_ID = 1
	WORLD_ID = 1
	ODBC = "PostgreSQL ODBC Driver(UNICODE)"
	SOCKET_POOL_SIZE = 1000

	[LOCALE]
	LOCALE = "PWE"

	[CONFIG]
	PUBLIC_IP = "127.0.0.1"
	PORT = 7201
	STAND_ALONE = 0

	[NET]
	MASTER_SERVER_IP = "127.0.0.1"
	MASTER_SERVER_PORT = 7500

	[ACCOUNTDB]
	SERVER = "127.0.0.1"
	DATABASE = "rz_accountdb_test"
	USERNAME = "postgres"
	PASSWORD = "password"

	[DB]
	SERVER = "127.0.0.1"
	DATABASE = "rz_gamedb_test"
	USERNAME = "postgres"
	PASSWORD = "password"

	[LOGDB]
	SERVER = "127.0.0.1"
	DATABASE = "rz_logdb_test"
	USERNAME = "postgres"
	PASSWORD = "password"

	[LOG]
	LOG_PATH = "log"
	LOG_LEVEL = 1
	LOG_TAG = "1"

	[DEBUG]
	DUMP_ENABLE = 1
	FULL_DUMP = 1
	DB_TRACE_ALL = 1
		
You can start the servers in this order.
		
	cd ./MasterServer
	start MasterServer_r.exe
	WAIT 5
	cd ..
	cd ./LoginServer
	start LoginServer_r.exe
	WAIT 5
	cd ..
	cd ./GameServer
	start GameServer_r.exe
	WAIT 5
	cd ..
	cd ./AppServer
	start AppServer_r.exe
	WAIT 5

To stop servers, only kill MasterServer_r.exe

	taskkill /F /IM MasterServer_r.exe

				
Good luck and Happy Holidays!
FRaiderZ Team