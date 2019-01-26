<?php
class DbConnector
{
    var $conn;
    var $db;
    function getConnector()
    {
        //$conn = new mysqli('localhost','root','', 'sbcdb');
        $conn = new mysqli('localhost', 'root', 'om', 'alberto');
        if ($conn->connect_error) {
            return null;
        }
        return $conn;
    }
    function executeQuery($query)
    {
        $con = $this->getConnector();
        $res = mysqli_query($con, $query);
        mysqli_close($con);
        return $res;
    }
}
?>