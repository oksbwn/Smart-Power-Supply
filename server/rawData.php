<?php 
    include('dbConnector.php');
    $dbObject=new DbConnector();
    if($_GET['TYPE']==1) //Return the latest Values
        $res=$dbObject-> executeQuery('SELECt * FROM `dps5015` ORDER BY `sl_no` DESC LIMIT 1');   
           
    if($_GET['TYPE']==2) //Return data of today
        $res=$dbObject-> executeQuery('SELECt * FROM `dps5015` WHERE `d5logtime` LIKE \''.$_GET['QRY'].' %\'  ORDER BY `sl_no` DESC LIMIT 1000');

    if($_GET['TYPE']==3) //Resturn data of a session
        $res=$dbObject-> executeQuery('SELECt * FROM `dps5015` WHERE `d5session`='.$_GET['QRY'].' ORDER BY `sl_no` DESC LIMIT 1000');    

    if($_GET['TYPE']==4) //Return data of latest session
        {
            $res=$dbObject-> executeQuery('SELECt `d5session` FROM `dps5015` ORDER BY `sl_no` DESC LIMIT 1');
           
            if ($res->num_rows > 0) {
                if($row = $res->fetch_assoc()) {
                    $res=$dbObject-> executeQuery('SELECt * FROM `dps5015` WHERE `d5session`='.$row["d5session"].' ORDER BY `sl_no` DESC');    
                }
            }else {
                $myObj->error = "Sorry No Data";
                echo json_encode(array($myObj));;
                die();
            }
        }
    $obj= new stdClass();
    if ($res->num_rows > 0) {
        $jsonArray=array();
        while($row = $res->fetch_assoc()) {
            
            $obj= new stdClass();
            $obj->iv=$row["d5involtage"];
            $obj->ov=$row["d5outvoltage"];
            $obj->op=$row["d5outpower"];
            $obj->os=$row["d5status"];
            $obj->session=$row["d5session"];
            $obj->oa=$row["d5outcurrent"];
            $obj->time=$row["d5logtime"];
            
            array_push($jsonArray,$obj);
        }
       echo  json_encode($jsonArray);
    } else {
        $obj->error = "Sorry No Data";
        echo json_encode(array($obj));;
        die();
    }
  
?>