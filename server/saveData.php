<?php
    include('dbConnector.php');
    $dbObject=new DbConnector();
    date_default_timezone_set('Asia/Kolkata');
    $inputVoltage= $_GET['iv']/100;
    $outputVoltage=  $_GET['ov']/100;
    $outputPower=  $_GET['op']/100;
    $outputStatus= $_GET['os'];
    $sessionID=  $_GET['session'];
    $outputCurrent=$_GET['oa']/100;
    

    $dbObject-> executeQuery('INSERT INTO `dps5015` (`sl_no`, `d5involtage`, `d5outvoltage`, `d5outpower`, `d5status`, `d5session`,`d5outcurrent`,`d5logtime`) VALUES (NULL, '.$inputVoltage.', '.$outputVoltage.', '.$outputPower.','.$outputStatus.', '.$sessionID.', '.$outputCurrent.',NULL)');
    // echo .' '..'  '..' '..' '.;
    // echo time();

?>