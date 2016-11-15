<html>
 <head>
  <title>PHP Test</title>
 </head>
 <body>
  <!-- testing sessions -->
  <?php session_start(); ?>
  
  <!-- testing browscap.ini -->
  <?php
   echo $_SERVER['HTTP_USER_AGENT'] . "<br/><br/>";
   $browser = get_browser(null, true);
   print_r($browser);
   echo "<br/><br/>";
  ?>
  
  <!-- a general PHP test -->
  <?php phpinfo(); ?>
 </body>
</html>