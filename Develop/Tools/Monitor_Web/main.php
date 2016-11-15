<?php
require_once("mdb2.php");

class ReplayData
{
	public $rows = array();
	
	public function __construct() 
	{

	}
	public function GetRow()
	{
		return count($this->rows);
	}
	
	public function GetLastComputer()
	{
		return $this->rows[0]['computer'];
	}
	public function GetLastFPS()
	{
		return $this->rows[0]['fps'];
	}
	public function GetLastLoadingTime()
	{
		return $this->rows[0]['loadingtime'];
	}
	public function GetLastRegTimeStr()
	{
		return $this->rows[0]['regdate'];
	}
}

class ReplayDB
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
	public function Fetch($replay_data)
	{
		$res =& $this->mdb2->query("SELECT TOP 200 lid, computer, fps, loadingtime, regdate FROM ReplayLog order by lid desc");
		
		if (PEAR::isError($res)) {
			die($res->getMessage());
		}

		$counter = 0;
		while (($row = $res->fetchRow(MDB2_FETCHMODE_ASSOC))) {

			$replay_data->rows[$counter]['lid'] = $row['lid'];
			$replay_data->rows[$counter]['computer'] = $row['computer'];
			$replay_data->rows[$counter]['fps'] = $row['fps'];
			$replay_data->rows[$counter]['loadingtime'] = $row['loadingtime'];
			$replay_data->rows[$counter]['regdate'] = $row['regdate'];
			
			$counter++;
		}
	}
	
	public function Close()
	{
		$this->mdb2->disconnect();
	}
}

class Chart
{
	public function PrintFPSChartHtml($data)
	{
		$data_str = "";
		for ($i = $data->GetRow()-1; $i >= 0; $i--)
		{
			$data_str = $data_str . $data->rows[$i]['fps'] . ",";
		}
		$data_str = substr($data_str, 1, -1);
		
		$str = "<img src=\"http://chart.apis.google.com/chart?chs=600x400&amp&chxt=y&chd=t:" . $data_str . "&amp;cht=ls&amp;\" alt=\"FPS chart\" />";
		echo $str;
		
		//echo "<br/>fps data = " . $data_str;
	}
	
	public function PrintLoadingTimeChartHtml($data)
	{
		$data_str = "";
		for ($i = $data->GetRow()-1; $i >= 0; $i--)
		{
			$data_str = $data_str . $data->rows[$i]['loadingtime'] . ",";
		}
		$data_str = substr($data_str, 1, -1);
		
		$str = "<img src=\"http://chart.apis.google.com/chart?chs=600x400&amp&chxt=y&chd=t:" . $data_str . "&amp;cht=ls&amp;\" alt=\"LoadingTime chart\" />";
		echo $str;
		
		//echo "<br/>fps data = " . $data_str;
	}

}

function PrintHistory($data)
{
	echo "<Table border=1>\n<tr width = 100><td><B>³¯Â¥</B></td><td width=80><B>computer</B></td><td width=50><B>FPS</B></td><td width=110><B>Loading Time</B></td></tr>\n";
	$data_str = "";
	for ($i = 0; $i < $data->GetRow(); $i++)
	{
		echo "<TR>\n";
		echo "<TD align=center>" . $data->rows[$i]['regdate'] . "</TD>\n";
		echo "<TD align=center>" . $data->rows[$i]['computer'] . "</TD>\n";
		echo "<TD align=center>" . $data->rows[$i]['fps'] . "</TD>\n";
		echo "<TD align=center>" . $data->rows[$i]['loadingtime'] . "</TD>\n";
		echo "</TR>\n";
	}
	echo "</Table>\n";
}

function GetLastScreenShotFiles($dir)
{
	$handle = @opendir($dir);
	while ($file = @readdir($handle))
	{
		$list[] = $file;
	}
	@closedir($handle);
	
	if (count($list) > 0)
	{
		$screen_path = $list[count($list)-1] . "/";
		
		echo "path = " . $screen_path . "<BR/>\n";
		
		$handle2 = @opendir($dir . $screen_path);
		while ($file = @readdir($handle2))
		{
			if ($file == ".") continue;
			if ($file == "..") continue;
			
			$list2[] = $screen_path . $file;
		}
		@closedir($handle2);
	}
	
	return $list2;
}

function PrintScreenShot()
{
	$path = "web_incoming/screenshot/";

	echo "<TABLE cellpadding=5>\n";
	
	$counter = 0;
	
	$dir = $path;
	$screen_path = "";
	
	$dir = GetLastScreenShotFiles($path);
	
	while (list($key, $value) = each($dir))
	{
		if ($counter == 0)
		{
			echo "<TR>";
		}
		
		echo "<TD>";
		echo "<a href='" . $path . $value . "' border=0>" . "<img src='" . $path . $value . "'  width=240 height=180/>" . "</a>";
		echo "</TD>\n";
		
		
		$counter++;
		
		if ($counter == 5)
		{
			echo "</TR>\n";
			$counter = 0;
		}
	}
	echo "</TABLE>\n";
}


$replay_data = new ReplayData();
$db = new ReplayDB();
$chart = new Chart();

//
$db->Open();
$db->Fetch($replay_data);
$db->Close();

?>