<!--#include file="conn.asp"-->
<%
if not session("check")="checked" then
response.Redirect "login.asp"
end if

set rs=server.CreateObject("adodb.recordset")
sql="select * from admin"
rs.open sql,conn,1,1

  set rs1=server.CreateObject("adodb.recordset")
  sql1="select * from admin"
  rs1.open sql1,conn,1,3

  if request.querystring("act")="xg" then
  rs1("username")=trim(request.form("username"))
  rs1("sh")=trim(request.form("sh"))
  rs1("fyts")=clng(trim(request.form("fyts")))
  if trim(request.form("password"))<>"" then rs1("password")=trim(request.form("password"))
  rs1("wzmc")=trim(request.form("wzmc"))
  rs1("mc")=trim(request.form("mc"))

  'if InStr(trim(request.form("bq")),"���������<a target='_blank' href='http://www.7wb.org' title='����ͬ־����'>����ͬ־����</a>")>0 then
  rs1("bq")=trim(request.form("bq"))
  'else  
 ' rs1("bq")=trim(request.form("bq"))&"<br/>���������<a target='_blank' href='http://www.7wb.org' title='����ͬ־����'>����ͬ־����</a>"  
  'end if                 
  rs1.update
  rs1.close
response.write "<script language='javascript'>" & chr(13)
		response.write "alert('�����ɹ���');" & Chr(13)
		response.write "window.document.location.href='pass.asp';"&Chr(13)
		response.write "</script>" & Chr(13)
Response.End
  else
  %>
<head>
<style type="text/css">
<!--
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #F8F9FA;
}
-->
</style>

<link href="images/skin.css" rel="stylesheet" type="text/css" />
</head>

<body>
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="17" valign="top" background="images/mail_leftbg.gif"><img src="images/left-top-right.gif" width="17" height="29" /></td>
    <td valign="top" background="images/content-bg.gif"><table width="100%" height="31" border="0" cellpadding="0" cellspacing="0" class="left_topbg" id="table2">
      <tr>
        <td height="31"><div class="titlebt">��վ����</div></td>
      </tr>
    </table></td>
    <td width="16" valign="top" background="images/mail_rightbg.gif"><img src="images/nav-right-bg.gif" width="16" height="29" /></td>
  </tr>
  <tr>
    <td height="71" valign="middle" background="images/mail_leftbg.gif">��</td>
    <td valign="top" bgcolor="#F7F8F9"><table width="100%" height="138" border="0" cellpadding="0" cellspacing="0">
      <tr>
        <td height="13" valign="top">&nbsp;</td>
      </tr>
      <tr>
        <td valign="top"><table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
          <tr>
            <td class="left_txt">��ǰλ�ã���վ����</td>
          </tr>
          <tr>
            <td >
            
<form name="form1" method="post" action="pass.asp?act=xg">
  <table width="100%" class="list" cellpadding="0" style="border-collapse: collapse" border="1" bordercolor="#BFC4CA">
  <tr><th colspan="2" height="25" bgcolor="#EEF2FB">
  ��վ����</th></tr>
  <tr><td width="18%" height="25">
  <p align="right">�û�����</td><td width="81%" height="25">
  &nbsp;<input type="text" name="username" value="<%=rs("username")%>"></td></tr>
  <tr><td width="18%" height="25"><p align="right">�����룺</td>
	<td width="81%" height="25">&nbsp;<input type="text" name="password"> 
	�粻�޸��뱣��Ϊ��</td></tr>

  <tr>
	<td width="18%" align="right" height="25">
  ��վ���ƣ�</td><td width="81%" height="25">
  &nbsp;<input type="text" name="wzmc" size="40" value="<%=rs("wzmc")%>"></td>
	</tr>
	<tr>
		<td width="18%" align="right">    
	������ˣ�</td><td width="81%">    
	&nbsp;<input type="radio" value="1" <%if rs("sh")=1 then response.write "checked"%> name="sh">����&nbsp; 
	<input type="radio" name="sh" <%if rs("sh")=0 then response.write "checked"%> value="0">�ر�</td>
	</tr>

	<tr>
		<td width="18%" align="right">    
	���Է�ҳ��</td><td width="81%">    
	&nbsp;<input type="text" name="fyts" size="10" value="<%=rs("fyts")%>">��</td>
	</tr>
	
		<tr>
		<td width="18%" align="right" height="25">    
	���ݿ������</td><td width="81%" height="25">    
	&nbsp;<a href="back.asp?action=back">�������ݿ�</a> <a href="back.asp?action=ys">ѹ�����ݿ�</a></td>
	</tr>

	<tr>
		<td width="18%" align="right">    
	�ײ���Ϣ��</td><td width="81%">    
	&nbsp;<textarea rows="6" name="bq" cols="80"><%=rs("bq")%></textarea></td>
	</tr>

  <tr><td width="98%" colspan="2" height="25">    
	<p align="center">    <input type="submit" name="Submit" value="�� ��">
    <input type="reset" name="Submit2" value="��  ��"></td></tr>
  </table>
</form>
            </td>
          </tr>
          </table>
          </td>
      </tr>
    </table></td>
    <td background="images/mail_rightbg.gif">��</td>
  </tr>
  <tr>
    <td valign="middle" background="images/mail_leftbg.gif"><img src="images/buttom_left2.gif" width="17" height="17" /></td>
      <td height="17" valign="top" background="images/buttom_bgs.gif"><img src="images/buttom_bgs.gif" width="17" height="17" /></td>
    <td background="images/mail_rightbg.gif"><img src="images/buttom_right2.gif" width="16" height="17" /></td>
  </tr>
</table>
<%
end if
rs.close
conn.close
set rs=nothing
set rs1=nothing
set conn=nothing
%>
</body>
</html>