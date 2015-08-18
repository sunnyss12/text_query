<?php
$buffer=$_REQUEST["term"];
//$buffer="test";
$socket = socket_create(AF_INET,SOCK_DGRAM,SOL_UDP);
if(!$socket)
    echo "socket_create";
$port = 8888;
$ip="192.168.1.117";
$len1 = strlen($buffer);
//echo $buffer,$len1;
if($buffer)
    if(-1 == socket_sendto($socket,$buffer,$len1,0,$ip,$port)){
        echo "socket_sendto";
    }
$buffer = "";
$ip2="";
$port2="";
if( -1 == socket_recvfrom($socket,$buffer,1024,0,$ip2,$port2))
{
    echo "socket_recvfrom";
}
$arr = json_decode($buffer,TRUE);


for($i=0;$i<count($arr);$i++)
{
    $word[$i] = $arr[$i]["word"];
}
echo json_encode($word);

//echo $buffer;
socket_close($socket);

?>
