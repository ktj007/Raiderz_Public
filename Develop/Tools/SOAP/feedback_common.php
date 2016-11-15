<?php
	require_once("mdb2.php");

class FeedbackDB
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


?>



