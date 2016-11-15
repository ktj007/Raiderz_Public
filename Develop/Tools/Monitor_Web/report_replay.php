<?php

require_once("mdb2.php");

class ReportDB
{
	public $dsn = array(
		"phptype"=>"mssql",
		"username"=>"dev",
		"password"=>"dev",
		"hostspec"=>"SH_DB",
		"database"=>"SoulHuntMonitor"
		);

	public $options = array(
		"debug"=>2,
		"result_buffering"=>false,
		);

	public $mdb2;

	public function Open()
	{
		$this->mdb2 =& MDB2::factory($this->dsn, $this->options);	
		
		if (PEAR::isError($this->mdb2)) {
			die($mdb2->getMessage());
		}
	}
	public function Query($computer, $fps, $LoadingTime)
	{
		$query = "INSERT INTO ReplayLog(Computer, fps, LoadingTime, RegDate) VALUES (" . "\"" . $computer . "\", " . $fps . ", " . $LoadingTime . ", GetDate());";
		 $res =& $this->mdb2->query($query);
		
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}
	}
	
	public function Close()
	{
		$this->mdb2->disconnect();
	}
}

$fps = $_GET['fps'];
$LoadingTime = $_GET['LoadingTime'];
$computer = $_GET['computer'];

$db = new ReportDB();
$db->Open();
$db->Query($computer, $fps, $LoadingTime);
$db->Close();

echo "OK<br/>\n";

?>