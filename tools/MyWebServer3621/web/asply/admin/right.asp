
<head>
<link href="images/skin.css" rel="stylesheet" type="text/css" />
<meta http-equiv="Content-Type" content="text/html; charset=gb2312" /><style type="text/css">
<!--
body {
	margin-left: 0px;
	margin-top: 0px;
	margin-right: 0px;
	margin-bottom: 0px;
	background-color: #EEF2FB;
}
-->
</style>
<base target="_self">
</head>

<body style="background-color: #F7F8F9">
<table width="100%" border="0" cellpadding="0" cellspacing="0">
  <tr>
    <td width="17" valign="top" background="images/mail_leftbg.gif"><img src="images/left-top-right.gif" width="17" height="29" /></td>
    <td valign="top" background="images/content-bg.gif"><table width="100%" height="31" border="0" cellpadding="0" cellspacing="0" class="left_topbg" id="table2">
      <tr>
        <td height="31"><div class="titlebt">��ӭ����</div></td>
      </tr>
    </table></td>
    <td width="16" valign="top" background="images/mail_rightbg.gif"><img src="images/nav-right-bg.gif" width="16" height="29" /></td>
  </tr>
  <tr>
    <td valign="middle" background="images/mail_leftbg.gif">��</td>
    <td valign="top" bgcolor="#F7F8F9"><table width="98%" border="0" align="center" cellpadding="0" cellspacing="0">
      <tr>
        <td colspan="2" valign="top">��</td>
      </tr>
      <tr>
        <td colspan="2" valign="top"><span class="left_bt">��л��ʹ�� ��Ե����Ƽ� ��ҵ��վ����ϵͳ����</span><br>
              <br>
            <span class="left_txt">&nbsp;<img src="images/ts.gif" width="16" height="16"> ��ʾ��<br>
          ������ʹ�õ���������Ե����Ƽ���<a href="http://www.116cn.cn">www.116cn.cn</a>��������һ�����ڹ�����ҵ��վ����վ����ϵͳ��
		������Ե����Ƽ���Ҫ�ṩ��վ���衢�İ桢������������Ӣ������ע�������������������ص�ҵ���������������ϵ��</span></td>
      </tr>
      <tr>
        <td colspan="2">��</td>
      </tr>
      <tr>
        <td colspan="2" valign="top">
        
    <div align="center">
        
    <table border=0 cellspacing=0 class=list cellpadding="0" width="100%">
	<tr><th colspan=2 height="20">
		<p align="left">���������йز���</th><th colspan=2 height="20">
		<p align="left">���֧���йز���</th></tr>
	<tr>
		<td width="20%" height="20">����������</td>
		<td width="29%" height="20"><%=Request.ServerVariables("SERVER_NAME")%></td>
		<td width="27%" height="20">ADO ���ݶ���</td>
		<td width="24%" height="20"><%=Get_ObjInfo("adodb.connection", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">������IP��</td>
		<td width="29%" height="20"><%=Request.ServerVariables("LOCAL_ADDR")%></td>
		<td width="27%" height="20">FSO �ı��ļ���д��</td>
		<td width="24%" height="20"><%=Get_ObjInfo("scripting.filesystemobject", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">�������˿ڣ�</td>
		<td width="29%" height="20"><%=Request.ServerVariables("SERVER_PORT")%></td>
		<td width="27%" height="20">Stream �ļ�����</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Adodb."&"Stream", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">������ʱ�䣺</td>
		<td width="29%" height="20"><%=Now()%></td>
		<td width="27%" height="20">Microsoft.XMLHTTP��</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Microsoft.XMLHTTP", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">IIS�汾��</td>
		<td width="29%" height="20"><%=Request.ServerVariables("SERVER_SOFTWARE")%></td>
		<td width="27%" height="20">Microsoft.XMLDOM��</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Microsoft.XMLDOM", 0)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">����������ϵͳ��</td>
		<td width="29%" height="20"><%=Request.ServerVariables("OS")%></td>
		<td width="27%" height="20">CDONTS ����SMTP���ţ�</td>
		<td width="24%" height="20"><%=Get_ObjInfo("CDONTS.NewMail", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">�ű���ʱʱ�䣺</td>
		<td width="29%" height="20"><%=Server.ScriptTimeout%> ��</td>
		<td width="27%" height="20">LyfUpload �ϴ������</td>
		<td width="24%" height="20"><%=Get_ObjInfo("LyfUpload.UploadFile", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">վ������·����</td>
		<td width="29%" height="20"><%=request.ServerVariables("APPL_PHYSICAL_PATH")%></td>
		<td width="27%" height="20">AspUpload �ϴ������</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Persits.Upload.1", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">������CPU������</td>
		<td width="29%" height="20"><%=Request.ServerVariables("NUMBER_OF_PROCESSORS")%> ��</td>
		<td width="27%" height="20">SA-FileUp �ϴ������</td>
		<td width="24%" height="20"><%=Get_ObjInfo("SoftArtisans.FileUp", 1)%></td>
	</tr>
	<tr>
		<td width="20%" height="20">�������������棺</td>
		<td width="29%" height="20"><%=ScriptEngine & "/" & ScriptEngineMajorVersion & "." & ScriptEngineMinorVersion & "." & ScriptEngineBuildVersion %></td>
		<td width="27%" height="20">AspJpeg ͼ���������</td>
		<td width="24%" height="20"><%=Get_ObjInfo("Persits.Jpeg",1)%></td>
	</tr>
	</table>
    
	</div>
    
<br>
	<div align="center">
<table width="100%" border="0" cellpadding="3" cellspacing="1" class="list">
      <tr class="hback">
        <th height="25" colspan="2" align="left" class="xingmu">ʹ�ñ�ϵͳ����ȷ�ϵķ�����������������������Ҫ��</th>
      </tr>
      <tr class="hback">
        <td width="48%" bgcolor="#F8F7F5" align="left">JRO.JetEngine(ACCESS&nbsp; ���ݿ�<span class="small2">)��</span>
            <%
		On Error Resume Next
		Server.CreateObject("JRO.JetEngine")
          
		if err=0 then 
		  response.write("��")
		else
		  response.write("��")
		end if	 
		err=0
	 %>        </td>
        <td width="50%" bgcolor="#F8F7F5">���ݿ�ʹ��:
          <%
		On Error Resume Next
		Server.CreateObject("Adodb.Connection")
		if err=0 then 
		  response.write("��,����ʹ�ñ�ϵͳ")
		else
		  response.write("��,����ʹ�ñ�ϵͳ")
		end if	 
		err=0
	%>        </td>
      </tr>
      <tr class="hback">
        <td bgcolor="#F8F7F5" align="left" width="48%"><span class="small2">FSO</span>�ı��ļ���д<span class="small2">��</span>
        <%=Get_ObjInfo("scripting.filesystemobject", 0)%>
    </td>
        <td height="20" bgcolor="#F8F7F5" width="50%"> Adodb.Stream: 
					<%Server.CreateObject("Adodb.Stream")
					if err=0 then 
					  response.write("��")
					else
					  response.write("��")
					end if	 
					err=0%>		  </td>
      </tr>
    </table>
	</div>
<%
Function Get_ObjInfo(obj, ver)
	On Error Resume Next
	Dim objTest, sTemp
	Set objTest = Server.CreateObject(obj)
	If Err.Number <> 0 Then
		Err.Clear
		Get_ObjInfo = "<font class=red><b>��</b></font>&nbsp;<font class=gray>��֧��</font>"
	Else
		sTemp = ""
		If ver = 1 Then
			sTemp = objTest.version
			If IsNull(sTemp) Then sTemp = objTest.about
			sTemp = Replace(sTemp, "Version", "")
			sTemp = "&nbsp;<font class=tims><font class=blue>" & sTemp & "</font></font>"
		End If
		Get_ObjInfo = "<b>��</b>&nbsp;<font class=gray>֧��</font>" & sTemp
	End If
	Set objTest = Nothing
	If Err.Number <> 0 Then Err.Clear
End Function
%>

</td>
      </tr>
      <tr>
        <td height="15" colspan="3"><table width="100%" height="1" border="0" cellpadding="0" cellspacing="0" bgcolor="#CCCCCC">
          <tr>
            <td></td>
          </tr>
        </table></td>
      </tr>
      <tr>
        <td width="1%">��</td>
        <td class="left_txt"><img src="images/icon-mail2.gif" width="16" height="11"> 
		<a href="mailto:��ϵ���䣺web@115cn.cn">��ϵ���䣺web@115cn.cn</a>&nbsp;&nbsp;&nbsp; 
		QQ��ѯ��84723090&nbsp; 545177708 <br>
              <img src="images/icon-phone.gif" width="17" height="14"> �ٷ���վ��<a href="http://www.116cn.cn">http://www.116cn.cn</a>
		<a href="http://www.115cn.cn">http://www.115cn.cn</a> 
		�绰��0592-5983163</td>
        <td>��</td>
      </tr>
    </table></td>
    <td background="images/mail_rightbg.gif">��</td>
  </tr>
  <tr>
    <td valign="bottom" background="images/mail_leftbg.gif"><img src="images/buttom_left2.gif" width="17" height="17" /></td>
    <td background="images/buttom_bgs.gif"><img src="images/buttom_bgs.gif" width="17" height="17"></td>
    <td valign="bottom" background="images/mail_rightbg.gif"><img src="images/buttom_right2.gif" width="16" height="17" /></td>
  </tr>
</table>
</body>
