<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" lang="gb2312">
<head>
<title>�����Ա�(PHP+ACCESS)</title>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<meta http-equiv="Content-Language" content="gb2312" />
<meta name="author" content="�W�oֹ����QQ:182407777" />
<meta name="Copyright" content="www.wesent.cn,���ɰ�Ȩ,����ת��" />
<meta name="description" content="php,access,php��ѧ" />
<meta content="php,access,php��ѧ���W�oֹ��" name="keywords" />
</head> 
<body>
<BR><BR>
<center><a href="admin_login.htm" target="_blank">���Թ���</a> </center><BR><BR>
<font color="red">�����������£�<BR><BR></font>
<?php 
     include_once("include/conn.php");
     $rs = $conn->Execute('SELECT * FROM contents order by id desc');
     while(!$rs->EOF)  
            { 
              echo "<table><tr><td>������ID:<font color='red'>".$rs->Fields['id']->Value ."</font></td>";
              echo "<td>&nbsp;&nbsp;����:".$rs->Fields['name']->Value ."</td></tr>"; 
              echo "<tr><td colspan='2'>����:".$rs->Fields['content']->Value ."</td></tr></table><br/>";
              echo "- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - "; 
              $rs->MoveNext();
			  }
	  /*�ͷ���Դ*/
	  $rs->Close();
      $conn->Close();
      $rs = null;
      $conn = null;
 ?>
<br><br><br>
<form action="add_messages.php" method="post"> 
    ����:<input type="text" name="user_name" size="60"><br> 
    ����:<textarea name="user_post" rows="20" cols="59"></textarea>
	<input type="submit" value="�ύ����"> 
   </form>
</body>
</html>