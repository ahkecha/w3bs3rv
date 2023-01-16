<?php
   session_start();
   unset($_SESSION["username"]);
   unset($_SESSION["password"]);

   header('Refresh: 1; URL = login.php');
   echo 'You have cleaned session, redirecting you to login page...';
?>
