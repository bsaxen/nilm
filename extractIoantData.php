<?php
$con=mysqli_connect("localhost","root","mypassword","mydb");

if (mysqli_connect_errno())
{
  echo "Failed to connect to MySQL: " . mysqli_connect_error();
}
echo "Connected to Database";

$sql="SELECT * FROM stream_49_electricpower WHERE ts BETWEEN TIMESTAMP('2017-01-16 00:00:00') AND TIMESTAMP('2017-01-16 23:59:59') ORDER BY ts ASC ";

echo $sql;

if ($result=mysqli_query($con,$sql))
{
  while ($row=mysqli_fetch_assoc($result))
  {
    printf ("%s %s\n",$row['ts'],$row['value']);
  }
  mysqli_free_result($result);
}
echo "Close Database";
mysqli_close($con);
?>

