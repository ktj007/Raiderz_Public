<?php

	require_once("mdb2.php");
	require_once("lib/helper_api.php");
	require_once("feedback_common.php");

$string = <<<XML
<?xml version="1.0" encoding="UTF-8"?>
<XML>
</XML>
XML;

	$xml = new SimpleXMLElement($string);


	$db = new FeedbackDB();
	$db->Open();


	$query = "SELECT fid, name, field, pos, dir, comment, RegDate FROM Feedback " .
			 "ORDER BY fid DESC";

	$db->Query($query);

	while ($row = $db->Fetch())
	{
  		$fid = $row['fid'];
	  	$name = stripslashes($row['name']);
		$field = $row['field'];
		$pos = $row['pos'];
		$dir = $row['dir'];
		$comment = stripslashes(change_to_utf($row['comment']));
	  	//$comment = nl2br(stripslashes(change_to_utf($row['comment'])));
		$regdate = $row['regdate'];


		// xml
		$f1 = $xml->addChild("Feedback");
		$f1->addAttribute('name', $reporter);
		$f1->addAttribute('date', $regdate);
		$f1->addAttribute('field', $field);
		$f1->addAttribute('pos', $pos);
		$f1->addAttribute('dir', $dir);
		
		$comment_element = $f1->addChild("Comment", $comment);

	}
	
	echo $xml->asXML();

	$db->close();
/*
<?xml version="1.0" encoding="UTF-8"?>
<XML>
	<Feedback name="birdkr" date="09-05-04" field="9001" pos="40000 40000 0" dir="0 1 0" >
		<Comment>무궁화 꽃이 피었습니다.</Comment>
	</Feedback>
</XML>
*/
?>



