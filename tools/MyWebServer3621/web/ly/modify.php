<?
 session_start();
 if($_SESSION['admin']=="OK")
 {
  include_once("include/conn.php");
  $sql="select * from contents where id=".$_GET['id']; /*$_GET['id']�Ǵ��ݹ����Ĳ��� */
  $rs = $conn->Execute($sql);
  $name=$rs->Fields['name']->Value;
  $content=$rs->Fields['content']->Value;
  $id=$rs->Fields['id']->Value;
 ?>

  <form action="modify_save.php" method="post" name="name1">
    ID  :<?=$id?><input type="hidden" name="id" value=<?=$id?> > 
    ����:<?=$name?><br> 
    ����:<textarea name="post_contents" rows="10" cols="50"><?=$content?></textarea> 
    <input type="submit" value="�ύ�޸�"> 
   </form>
 <?
  }
	  $rs->Close();
      $conn->Close();
      $rs = null;
      $conn = null;
?>

