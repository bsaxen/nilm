<?php
$username="root";
$password="Milarus!SQL34";
$database="ioant";
$field1-name=$_POST['Value1'];
$field2-name=$_POST['Value2'];
$field3-name=$_POST['Value3'];
$field4-name=$_POST['Value4'];
$field5-name=$_POST['Value5'];
mysql_connect(localhost,$username,$password);
@mysql_select_db($database) or die( "Unable to select database");
$query = "SELECT * FROM stream_49_electricpower WHERE ts BETWEEN('2017-01-16 00:00:00' AND '2017-01-16 23:59:59' ORDER BY datetime DESC ");
mysql_query($query);
mysql_close();
?>