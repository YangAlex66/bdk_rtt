<? require_once('inc/config.php'); ?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" />
<title>�����PHP+Access���¹���By xinyuefei.com</title>
<link rel="shortcut icon" href="images/favicon.ico" />
<link href="style/css.css" rel="stylesheet" type="text/css" />
</head>
<body><div id="box">
	<div id="header"></div>
	<div id="content">
		<div id="logo">�����PHP+Access���¹���</div>
		<? $articleid=$_GET['id'];
		$rs=new COM("adodb.recordset");
		$rs->open("select * from [article] where [articleid]=".$articleid,$conn,1,1);
		$articletitle = $rs->Fields(1)->value;
		$articlecontent = $rs->Fields(2)->value; ?>
		<div id="navi">����λ�ã�<a href="index.php">��ҳ</a><img src="images/arrow2.gif" /><a href="index.php">�����б�</a><img src="images/arrow2.gif" /><? echo $articletitle; ?></div>
		<div id="detail">
			<h1><? echo $articletitle; ?></h1>
			<? echo stripslashes($articlecontent); ?>
		</div>
		<div id="links">
			<ul>
			<li><a href="http://xinyuefei.com/">��ҳ</a></li>
			<li><a href="http://xinyuefei.com/">����</a></li>
			<li class="last"><a href="http://xinyuefei.com/contact.htm">��ϵ</a></li>
			</ul>
		</div>
	</div>
	<div id="footer"></div>
</div></body>
</html>
<? $rs->Close(); $rs=null;
$conn->Close(); $conn=null; ?>