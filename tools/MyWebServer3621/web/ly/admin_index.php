<?php
session_start();
if($_SESSION['admin']=="OK")
 {
  include_once("include/conn.php");
  $rs = $conn->Execute('SELECT * FROM contents');
  while(!$rs->EOF)
     {
      echo "<table><tr><td>����������:".$rs->Fields['name']->Value ."</td></tr>"; 
      echo "<tr><td>����:".$rs->Fields['content']->Value ."</td></tr></table><br/>";
      echo  "<a href=modify.php?id=".$rs->Fields['id']->Value ." >�޸�</a>      <a href=delete.php?id=".$rs->Fields['id']->Value ." >ɾ��</a>";
	  $rs->MoveNext();
     }
 echo "<br><br><br><br><br><a href=login_out.php>�˳�</a>";
 
 $rs->Close();
 $conn->Close();
 $rs = null;
 $conn = null;
 }
 else
 {
	 header("location:index.php");
 }
?>

