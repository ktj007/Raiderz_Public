<html>
 <head>
  <title>소울헌트 모니터링</title>
 </head>
 <body>
 
 <H2><center>소울헌트 모니터링</center></H2>
 <br/>
 
 <?php 
 include "main.php";
 ?>
 
 <div id='recent'>
 <center>
 
 <H3><center>최근 현황</center></H3>
 
 <table border=1>
 <tr>
 	<td width=150>컴퓨터</td><td width=200><?php echo $replay_data->GetLastComputer(); ?></td>
</tr>
<tr>
	<td>FPS</td><td><?php echo $replay_data->GetLastFPS(); ?></td>
</tr>
<tr>
	<td>Loading Time</td><td><?php echo $replay_data->GetLastLoadingTime(); ?> 초</td>
</tr>
 <tr>
	<td width=150>최근 실행 시간</td><td width=200><?php echo $replay_data->GetLastRegTimeStr(); ?></td>
</tr>

</table>
</center>
 </div>
 
 <br/><br/>
 <div id='graph'>
 <center>
 <H3>FPS 그래프</H3>
 
 <br/>
 <?php $chart->PrintFPSChartHtml($replay_data); ?>
 
 <br/><br/>
 <H3>Loading Time 그래프</H3>
 
 
 <br/>
 <?php $chart->PrintLoadingTimeChartHtml($replay_data); ?>
 
 </center>
 </div>
 
 
 <br/><br/>
 
 <div id='screenshot'>
 <center>
 <H3>최근 스크린샷</H3>
 
 <?php PrintScreenShot(); ?>
 </center>
 </div>
 
 <br/><br/><br/>
 
 <div id='history'>
 <center>
 <H3>History</H3>
 
  <?php PrintHistory($replay_data); ?>
 
 </center>
 </div>
 
 <br/><br/>
 
 </body>
</html>