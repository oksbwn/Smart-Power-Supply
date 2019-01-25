<?php
    include('dbConnector.php');
    $dbObject=new DbConnector();
    $dbObject-> executeQuery('INSERT INTO `dps5015` (`sl_no`, `d5involtage`, `d5outvoltage`, `d5outpower`, `d5status`, `d5session`,`d5outcurrent`,`d5logtime`) VALUES (NULL, '.($_GET['iv']/100).', '.($_GET['ov']/100).', '.($_GET['op']/100).','. ($_GET['os']).', '.$_GET['session'].', '.($_GET['oa']/100).','.time().')');
    // echo .' '..'  '..' '..' '.;
    // echo time();
?>