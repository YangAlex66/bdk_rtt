<!--#include file="../fydy.asp"-->
<!--#include file="conn.asp"-->
<%
session("url")=GetUrl() 
if not session("check")="checked" then
response.Redirect "login.asp"
end if
action=request("action")
key=request("key")
classid=clng(request("classid"))

'�������� 
Set mypage=new xdownpage 
'�õ����ݿ����� 
mypage.getconn=conn 
'sql��� 

if key="" then
mypage.getsql="select * from book order by id desc"
else
mypage.getsql="select * from book where Title like '%" & key & "%'  order by id desc"
end if


'����ÿһҳ�ļ�¼������Ϊ5�� 
mypage.pagesize=20
'����Recordset
set rs=mypage.getrs()  
'��ʾ��ҳ��Ϣ������������ԣ���set rs=mypage.getrs()�Ժ�,��������λ�õ��ã����Ե��ö�� 

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
.style1 {
	border-width: 1px;
}
-->
</style>
<SCRIPT language=javascript>
function CheckAll(form)
{
  for (var ii=0;ii<form.elements.length;ii++)
    {
    var e = form.elements[ii];
    if (e.Name != "chkAll")
       e.checked = form.chkAll.checked;
    }
}
function Checked()
{
	var jj = 0
	for(ii=0;ii < document.form.elements.length;ii++){
		if(document.form.elements[ii].name == "adid"){
			if(document.form.elements[ii].checked){
				jj++;
			}
		}
	}
	return jj;
}

function DelAll()
{
	if(Checked()  <= 0){
		alert("������ѡ��1����Ϣ!");
	}	
	else{
		if(confirm("ȷ��Ҫɾ��ѡ���������\n�˲��������Իָ���")){
			form.action="delbook.asp?del=data";
			form.submit();
		}
	}
}
function sh()
{
	if(Checked()  <= 0){
		alert("������ѡ��1����Ϣ!");
	}	
	else{
		if(confirm("ȷ��Ҫͨ����ѡ���������")){
			form.action="sh.asp?sh=data";
			form.submit();
		}
	}
}
</SCRIPT>
<link href="images/skin.css" rel="stylesheet" type="text/css" />
<script type="text/javascript" src="../js/jquery-1.4.2.js"></script>
<script type="text/javascript">
$(function(){
$("#bs tr").hover(function(){
$(this).addClass("bs");
},function(){
$(this).removeClass();
});
});
</script>
</head>

<body>
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="17" valign="top" background="images/mail_leftbg.gif"><img src="images/left-top-right.gif" width="17" height="29" /></td>
    <td valign="top" background="images/content-bg.gif"><table width="100%" height="31" border="0" cellpadding="0" cellspacing="0" class="left_topbg" id="table2">
      <tr>
        <td height="31"><div class="titlebt">���Թ���</div></td>
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
            <td class="left_txt" colspan="2">��ǰλ�ã����Թ���</td>
          </tr>
          <tr>
            <td >
            
 <div align="center">          
            
 <table width="100%" border="1" cellpadding="0" cellspacing="0" class="list" bordercolor="#C0C0C0" style="border-collapse: collapse">
   <form method="get" action="">
  <tr> 
    <th align=left width="100%" height="25" align="center" colspan="9" style="padding-left:5px; padding-right:5px; padding-top:2px; padding-bottom:2px">
    <span style="float:right"><INPUT title=��� onclick=sh() type=button value=��� name=Submit>&nbsp;<INPUT title=ɾ�� onclick=DelAll() type=button value=ɾ�� name=Submit></span>
<input type="text" name="key" value="<%=key%>" size="20"> 
<input type="submit" value=" �� �� " name="B1">
	</th>
  </tr>
  </form>
<form id="bs" name=form method=post action="?action=xg">  
  <tr> 
    <td height="25" align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b><span class="style1">ID</span></b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<span class="style1"><b>��������</b></span></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>����</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>�绰</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b><span class="style1">����</span></b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>�ö�</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
	<b>���</b></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB" width="10%">
	<b><span class="style1">����</span></b></td>
    <td width="5%" align="center" style="padding-left: 2px; padding-right: 2px" bgcolor="#EEF2FB">
                  <input id=chkAll onClick=CheckAll(this.form) type=checkbox value=checkbox name=chkAll style="font-weight: 700"></td>
  </tr>
  <%

for i=1 to mypage.pagesize 
if not rs.eof then 

%>
  <tr bgcolor="#FFFFFF"> 
    <td height="22" align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%=rs("id")%></td>
	<input name=id type=hidden value="<%=rs("id")%>" >
    <td style="padding-left: 2px; padding-right: 2px" class="style1"><%=left(rs("title"),30) %></td>
    <td style="padding-left: 2px; padding-right: 2px;text-align:center" class="style1"><%=rs("name")%></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%=rs("tel")%></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%=rs("sj")%></td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><%
    if rs("zd")=1 then
    response.write "<a href=top.asp?action=qxtj&id="&rs("id")&"><font color=red>��</font></a>"
    else
    response.write "<a href=top.asp?action=tj&id="&rs("id")&">��</a>"
    end if
    %>
    </td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1">
    <%if rs("sh")=1 then
    response.write "<a href=sh.asp?action=qxtj&id="&rs("id")&">��</a>"
    else
    response.write "<a href=sh.asp?action=tj&id="&rs("id")&"><font color=red>��</font></a>"
    end if
    %>

    </td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1">
    <a href="lyhf.asp?id=<%=rs("id")%>">�ظ�</a> <a href="delbook.asp?id=<%=rs("id")%>">ɾ��</a> </td>
    <td align="center" style="padding-left: 2px; padding-right: 2px" class="style1"><input type="checkbox" name="adid" value="<%=rs("id")%>" onClick=Checked(form)></td>
  </tr>
<%
 rs.movenext 
 else 
exit for 
end if
 next                                                    
%>

</table>
	
	</div>
	<div align="center">
	<table border=0 cellspacing=1 class=navi width="100%">
	<tr>
	<td align=left height=30 width="120">
		<p align="center">
		��</td>
	  <td align=center height=30><% mypage.showpage() %></td>
	</tr>
	</table>
	</div>
<% 
rs.close
set rs=nothing
conn.close
set conn=nothing
%>
           
            
            </td>
         <td >
            
 ��</td>
          </tr>
          </table>
          </form>
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

</body>
</html>