<?php
$con=mysqli_connect("localhost","root","my_password","my_db");

if (mysqli_connect_errno())
{
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
}

$sql="SELECT * FROM stream_49_electricpower WHERE ts BETWEEN('2017-01-16 00:00:00' AND '2017-01-16 23:59:59' ORDER BY ts DESC ");

if ($result=mysqli_query($con,$sql))
{
  while ($row=mysqli_fetch_row($result))
  {
    printf ("%s (%s)\n",$row[0],$row[1]);
  }
  mysqli_free_result($result);
}
mysqli_close($con);
?>
