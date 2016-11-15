<?php

	// db
	require_once("mdb2.php");

class ServerStatusDB
{
	public $dsn = array(
		"phptype"=>"mssql",
		"username"=>"dev",
		"password"=>"dev",
		"hostspec"=>"SH_DB",
		"database"=>"RZ_GAMEDB"
		);

	public $options = array(
		"debug"=>2,
		"result_buffering"=>false,
		);

	public $mdb2;

	public $res;
	
	public function Open()
	{
		$this->mdb2 =& MDB2::factory($this->dsn, $this->options);	
		
		if (PEAR::isError($this->mdb2)) {
			die($mdb2->getMessage());
		}
	}
	public function Query($query)
	{
		 $this->res =& $this->mdb2->query($query);
		
		if (PEAR::isError($this->res)) {
			die($this->res->getMessage());
		}
	}
	public function Fetch()
	{
		if (PEAR::isError($res)) {
			die($this->res->getMessage());
		}

		$row = $this->res->fetchRow(MDB2_FETCHMODE_ASSOC);
	
		return $row;
	}
	public function Close()
	{
		$this->mdb2->disconnect();
	}
}

$string = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<XML>
XML;

	$db = new ServerStatusDB();
	$db->Open();

	$query = "SELECT s.WorldID, s.ServerID, s.IP, s.Port, s.[Type], CASE WHEN DATEDIFF(ss, ss.UpdateDt, GETDATE()) < (s.UpdateElapsedTm + s.AllowDelayTm) THEN 1 ELSE 0 END AS IsRun FROM dbo.RZ_SERVER s WITH (NOLOCK), dbo.RZ_SERVER_STATUS ss WITH (NOLOCK) WHERE ss.WorldID = s.WorldID AND ss.ServerID = s.ServerID AND EndDt = CAST('9999-12-31 00:00:00' AS DATETIME)";
	
	$db->Query($query);
	
	// popcorn server always active.
	$str_xml = "<Server name=\"" . "Popcorn" . "\" ip=\"" . "192.168.0.13" . "\" active=\"" . "true" . "\" />\n";
	$string .= $str_xml;
	
	while ($row = $db->Fetch())
	{
	
  		$IP = $row['ip'];
		$ServerType = $row['type'];
		$IsRun = $row['isrun'];

		$str_ServerName = "";
		$str_ip = $IP;
		$str_active = "false";
		
		if ($IsRun == 1) $str_active = "true";
		
		
		if ($ServerType == 0)	// default server type
		{
		//	if ($IP == "192.168.0.13")		// Popcorn
		//	{
		//		$str_ServerName = "Popcorn(Stable)";
		//	}
		//	else 
			if ($IP == "192.168.0.6")
			{
				$str_ServerName = "Icecream(Develop)";
			}
			else if ($IP == "192.168.0.37")
			{
				$str_ServerName = "Cola(Stable)";
			}
			else
			{
				continue;
			}
		}
		else
		{
			continue;
		}
				
		$str_xml = "<Server name=\"" . $str_ServerName . "\" ip=\"" . $str_ip . "\" active=\"" . $str_active . "\" />\n";
		$string .= $str_xml;
	}
		
	$str_localhost = "<Server name=\"" . "localhost" . "\" ip=\"" . "localhost" . "\" active=\"" . "true" . "\" />\n";
	$string .= $str_localhost;
		
	$db->Close();
	
$string_tail = "</XML>\n";	
	// <Server name="Icecream(개발서버)" ip="icecream" active="true" />
	// <Server name="Popcorn(안정서버)" ip="popcorn" active="false" />
	// <Server name="localhost(로컬서버)" ip="localhost" active="true" />
// </XML>
// XML;

	$string .= $string_tail;
	
echo $string;

?>