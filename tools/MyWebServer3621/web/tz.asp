<%@ Language="VBScript" %>
<%
' *************************************************

'  ����ASP̽�� V1.93 20060602
'  �����غ� http://www.ajiang.net

' *************************************************

' ��ʹ�������������ֱ�ӽ����н����ʾ�ڿͻ���
'ON ERROR RESUME NEXT
server.scripttimeout=-1
Response.Buffer =true 'false

' ��ҳ������ʱ����ֹ���浼�²���ʧ�ܡ�
Response.Expires = -1
response.write Request("style")':response.end
' ������������б�
Dim OtT(3,15,1)
' ����������
dim okCPUS, okCPU, okOS
' ����������
dim isobj,VerObj,TestObj

T = Request("T")
if T="" then T="ABGH"
%>

<HTML>
<HEAD>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<TITLE>ASP̽��V1.93������http://www.ajiang.net</TITLE>
<style>
<!--
h1 {font-size:14px;color:#3F8805;font-family:Arial;margin:15px 0px 5px 0px}
h2 {font-size:12px;color:#000000;margin:15px 0px 8px 0px}
h3 {font-size:12px;color:#3F8805;font-family:Arial;margin:7px 0px 3px 12px;font-weight: normal;}
BODY,TD{FONT-FAMILY: ����;FONT-SIZE: 12px;word-break:break-all}
tr{BACKGROUND-COLOR: #EEFEE0}
A{COLOR: #3F8805;TEXT-DECORATION: none}
A:hover{COLOR: #000000;TEXT-DECORATION: underline}
A.a1{COLOR: #000000;TEXT-DECORATION: none}
A.a1:hover{COLOR: #3F8805;TEXT-DECORATION: underline}
table{BORDER: #3F8805 1px solid;background-color:#3F8805;margin-left:12px}
p{margin:5px 12px;color:#000000}
.input{BORDER: #111111 1px solid;FONT-SIZE: 9pt;BACKGROUND-color: #F8FFF0}
.backs{BACKGROUND-COLOR: #3F8805;COLOR: #ffffff;text-align:center}
.backq{BACKGROUND-COLOR: #EEFEE0}
.backc{BACKGROUND-COLOR: #3F8805;BORDER: medium none;COLOR: #ffffff;HEIGHT: 18px;font-size: 9pt}
.fonts{	COLOR: #3F8805}
-->
</STYLE>
</HEAD>
<body>



<h1><a href="http://www.ajiang.net/">����</a><a href="http://www.ajiang.net/aspcheck.asp">ASP ̽��</a> V 1.93 - 20060602</h1>
<%
call mmenu()
for qq=1 to len(T)
  call BodyGo(mid(T,qq,1))
next
call mmenu()
%>
<br>
<br>
<table border=0 width=512 cellspacing=1 cellpadding=3 style="margin-left:0px;border:none;background:none">
  <tr style="background:none" align="center"><td>
  <hr width="512" size="1">
  �����غ� (www.ajiang.net) ��Ȩ���� &copy; 2001-2005
  <br>
  <a href="http://www.ajiang.net/">�����غ�</a>
  | <a href="http://www.ajstat.com/">����ͳ��</a>
  | <a href="http://www.51.la/">��Ҫ�����ͳ��</a>
  | <a href="http://www.ajiang.net/products/aspcheck/">����̽��</a>
  | <a href="http://www.ajiang.net/products/aspcheck/">�������°�</a>
  <hr width="512" size="1">
  </td></tr>
</table>  
</body>
</html>

<%












' *******************************************************************************
' ����[ A ] �Ƿ�֧��ASP
' *******************************************************************************
sub aspyes()
%>
<h2>�Ƿ�֧��ASP</h2>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr><td>
    ���������������ʾ���Ŀռ䲻֧��ASP��
    <br>1�����ʱ��ļ�ʱ��ʾ���ء�
    <br>2�����ʱ��ļ�ʱ�������ơ�&lt;&#x25;&#x40;&#x20;&#x4C;&#x61;&#x6E;&#x67;&#x75;&#x61;&#x67;&#x65;&#x3D;&#x22;&#x56;&#x42;&#x53;&#x63;&#x72;&#x69;&#x70;&#x74;&#x22;&#x20;&#x25;&gt;�������֡�
    </td></tr>
  </table>
<%
end sub






' *******************************************************************************
' ����[ B ] �������ſ�
' *******************************************************************************
sub servinfo()
on error resume next
%>
  <h2>�������ſ�</h2>
	<table border=0 width=500 cellspacing=1 cellpadding=3>
	  <tr>
		<td width=250>��������ַ</td><td width=350>���� <%=Request.ServerVariables("SERVER_NAME")%>(IP:<%=Request.ServerVariables("LOCAL_ADDR")%>) �˿�:<%=Request.ServerVariables("SERVER_PORT")%></td>
	  </tr>
	  <%
	  tnow = now():oknow = cstr(tnow)
	  if oknow <> year(tnow) & "-" & month(tnow) & "-" & day(tnow) & " " & hour(tnow) & ":" & right(FormatNumber(minute(tnow)/100,2),2) & ":" & right(FormatNumber(second(tnow)/100,2),2) then oknow = oknow & " (���ڸ�ʽ���淶)"
	  %>
	  <tr>
		<td>������ʱ��</td><td><%=oknow%></td>
	  </tr>
	  <tr>
		<td>IIS�汾</td><td><%=Request.ServerVariables("SERVER_SOFTWARE")%></td>
	  </tr>
	  <tr>
		<td>�ű���ʱʱ��</td><td><%=Server.ScriptTimeout%> ��</td>
	  </tr>
	  <tr>
		<td>���ļ�·��</td><td><%=Request.ServerVariables("PATH_TRANSLATED")%></td>
	  </tr>
	  <tr>
		<td>�������ű�����</td><td><%=ScriptEngine & "/"& ScriptEngineMajorVersion &"."&ScriptEngineMinorVersion&"."& ScriptEngineBuildVersion %> ,<%="JScript/"   %></td>
	  </tr>
	  <%getsysinfo()  '��÷���������%>
	  <tr>
		<td>����������ϵͳ</td><td><%=okOS%></td>
	  </tr>
	  <tr>
		<td>ȫ�ֺͻỰ����</td><td>Application ���� <%=Application.Contents.count%> ��<% if Application.Contents.count>0 then Response.Write "[<a href=""?T=C"">�б�</a>]"%>, 
		Session ���� <%=Session.Contents.count%> ��  <%if Session.Contents.count>0 then Response.Write "[<a href=""?T=D"">�б�</a>]"%></td>
	  </tr>
	  <tr>
		<td>ServerVariables</td><td><%=Request.ServerVariables.Count%> ��  <%if Request.ServerVariables.Count>0 then Response.Write "[<a href=""?T=E"">Request.ServerVariables �б�</a>]"%></td>
	  </tr>
	  <tr>
		<td>������CPUͨ����</td><td><%=okCPUS%> ��</td>
	  </tr>
	  <%

	  call ObjTest("WScript.Shell")
	  if isobj then
	    set WSshell=server.CreateObject("WScript.Shell")
	  %>
	  <tr>
		<td>������CPU����</td><td><%=okCPU%></td>
	  </tr>
	  <tr>
		<td>ȫ������������</td><td><%=WSshell.Environment.count%> ��  <%if WSshell.Environment.count>0 then Response.Write "[<a href=""?T=F"">WSshell.Environment �б�</a>]"%></td>
	  </tr>
	  <%
	  end if
	  %>
	</table>
<%
end sub

%>
<SCRIPT language="JScript" runat="server">
function getJVer(){
  //��ȡJScript �汾
  return ScriptEngineMajorVersion() +"."+ScriptEngineMinorVersion()+"."+ ScriptEngineBuildVersion();
}
</SCRIPT>
<%

' ��ȡ���������ò���
sub getsysinfo()
  on error resume next
  Set WshShell = server.CreateObject("WScript.Shell")
  Set WshSysEnv = WshShell.Environment("SYSTEM")
  okOS = cstr(WshSysEnv("OS"))
  okCPUS = cstr(WshSysEnv("NUMBER_OF_PROCESSORS"))
  okCPU = cstr(WshSysEnv("PROCESSOR_IDENTIFIER"))
  if isempty(okCPUS) then
    okCPUS = Request.ServerVariables("NUMBER_OF_PROCESSORS")
  end if
  if okCPUS & "" = "" then
    okCPUS = "(δ֪)"
  end if
  if okOS & "" = "" then
    okOS = "(δ֪)"
  end if
end sub






' *******************************************************************************
' ����[ C ] Application �����б�
' *******************************************************************************
sub applist()
%>
<h2>Application �����б�</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">�� �� �� ��</td><td width="390">ֵ</td></tr>
  <%for each apps in Application.Contents%>
  <tr><td width="110"><%=apps%></td><td width="390"><%
  if isobject(Application.Contents(apps)) then
    Response.Write "[����]"
  elseif isarray(Application.Contents(apps)) then
    Response.Write "[����]"
  else
    Response.Write cHtml(Application.Contents(apps))
  end if
  %></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' ����[ D ] Session �����б�
' *******************************************************************************
sub seslist()
%>
<h2>Session �����б�</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">�� �� �� ��</td><td width="390">ֵ</td></tr>
  <%for each sens in Session.Contents%>
  <tr><td width="110"><%=sens%></td><td width="390"><%
  if isobject(Session.Contents(sens)) then
    Response.Write "[����]"
  elseif isarray(Session.Contents(sens)) then
    Response.Write "[����]"
  else
    Response.Write cHtml(Session.Contents(sens))
  end if
  %></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' ����[ E ] Request.ServerVariables �����б�
' *******************************************************************************
sub sevalist()
%>
<h2>Request.ServerVariables �����б�(���ͻ�����Ϣ)</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">�� �� �� ��</td><td width="390">ֵ</td></tr>
  <%for each apps in Request.ServerVariables%>
  <tr><td width="110"><%=apps%></td><td width="390"><%=cHtml(Request.ServerVariables(apps))%></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' ����[ F ] Request.ServerVariables �����б�
' *******************************************************************************
sub wsslist()
  on error resume next
  Set WSshell = server.CreateObject("WScript.Shell")
%>
<h2>WScript.Shell.Environments �����б�</h2>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="110">�� �� �� ��</td><td width="390">ֵ</td></tr>
  <%for each envs in WSshell.Environment
  envsa = split(envs,"=")
  %>
  <tr><td width="110"><%=envsa(0)%></td><td width="390"><%=cHtml(envsa(1))%></td></tr><%next%>
</table>
<%
end sub






' *******************************************************************************
' ����[ G ] ������
' *******************************************************************************
sub comlist()
  on error resume next
  OtT(0,0,0) = "MSWC.AdRotator"
  OtT(0,1,0) = "MSWC.BrowserType"
  OtT(0,2,0) = "MSWC.NextLink"
  OtT(0,3,0) = "MSWC.Tools"
  OtT(0,4,0) = "MSWC.Status"
  OtT(0,5,0) = "MSWC.Counters"
  OtT(0,6,0) = "IISSample.ContentRotator"
  OtT(0,7,0) = "IISSample.PageCounter"
  OtT(0,8,0) = "MSWC.PermissionChecker"
  OtT(0,9,0) = "Microsoft.XMLHTTP"
	OtT(0,9,1) = "(Http ���, ���ڲɼ�ϵͳ���õ�)"
  OtT(0,10,0) = "WScript.Shell"
	OtT(0,10,1) = "(Shell ���, �����漰��ȫ����)"
  OtT(0,11,0) = "Scripting.FileSystemObject"
	OtT(0,11,1) = "(FSO �ļ�ϵͳ�����ı��ļ���д)"
  OtT(0,12,0) = "Adodb.Connection"
	OtT(0,12,1) = "(ADO ���ݶ���)"
  OtT(0,13,0) = "Adodb.Stream"
	OtT(0,13,1) = "(ADO ����������, ����������������ϴ�������)"
	
  OtT(1,0,0) = "SoftArtisans.FileUp"
	OtT(1,0,1) = "(SA-FileUp �ļ��ϴ�)"
  OtT(1,1,0) = "SoftArtisans.FileManager"
	OtT(1,1,1) = "(SoftArtisans �ļ�����)"
  OtT(1,2,0) = "Ironsoft.UpLoad"
	OtT(1,2,1) = "(�������, �ϴ����)"
  OtT(1,3,0) = "LyfUpload.UploadFile"
	OtT(1,3,1) = "(���Ʒ���ļ��ϴ����)"
  OtT(1,4,0) = "Persits.Upload.1"
	OtT(1,4,1) = "(ASPUpload �ļ��ϴ�)"
  OtT(1,5,0) = "w3.upload"
	OtT(1,5,1) = "(Dimac �ļ��ϴ�)"

  OtT(2,0,0) = "JMail.SmtpMail"
	OtT(2,0,1) = "(Dimac JMail �ʼ��շ�) <a href='http://www.ajiang.net/products/aspcheck/coms.asp'>�����ֲ�����</a>"
  OtT(2,1,0) = "CDONTS.NewMail"
	OtT(2,1,1) = "(CDONTS)"
  OtT(2,2,0) = "CDO.Message"
	OtT(2,2,1) = "(CDOSYS)"
  OtT(2,3,0) = "Persits.MailSender"
	OtT(2,3,1) = "(ASPemail ����)"
  OtT(2,4,0) = "SMTPsvg.Mailer"
	OtT(2,4,1) = "(ASPmail ����)"
  OtT(2,5,0) = "DkQmail.Qmail"
	OtT(2,5,1) = "(dkQmail ����)"
  OtT(2,6,0) = "SmtpMail.SmtpMail.1"
	OtT(2,6,1) = "(SmtpMail ����)"
	
  OtT(3,0,0) = "SoftArtisans.ImageGen"
	OtT(3,0,1) = "(SA ��ͼ���д���)"
  OtT(3,1,0) = "W3Image.Image"
	OtT(3,1,1) = "(Dimac ��ͼ���д���)"
  OtT(3,2,0) = "Persits.Jpeg"
	OtT(3,2,1) = "(ASPJpeg)"
  OtT(3,3,0) = "XY.Graphics"
	OtT(3,3,1) = "(�������, ͼ��/ͼ����)"
  OtT(3,4,0) = "Ironsoft.DrawPic"
	OtT(3,4,1) = "(�������, ͼ��/ͼ�δ���)"
  OtT(3,5,0) = "Ironsoft.FlashCapture"
	OtT(3,5,1) = "(�������, �๦�� FLASH ��ͼ)"
  OtT(3,6,0) = "dyy.zipsvr"
	OtT(3,6,1) = "(�������, �����ļ�ѹ����ѹ���)"
  OtT(3,7,0) = "hin2.com_iis"
	OtT(3,7,1) = "(�������, ����IIS�������)"
  OtT(3,8,0) = "Socket.TCP"
	OtT(3,8,1) = "(Dimac ��˾�� Socket ���)"
	
%>
<h2>ASP���֧�����</h2><a name="G"></a>

<h3>�� �������Ƿ�֧��</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <FORM action="?T=<%=T%>#G" method="post">
  <tr><td align="center" style="padding:10px 0px">
  ��������ı�����������Ҫ��������� ProgId �� ClassId
  <input class=input type=text value="" name="classname" size=50>
  <input type=submit value=" �� �� " class=backc id=submit1 name=submit1>
<%
Dim strClass
strClass = Trim(Request.Form("classname"))
If "" <> strClass then
Response.Write "<p style=""margin:9px 0px 0px 0px"">"
Dim Verobj1
ObjTest(strClass)
  If Not IsObj then 
	Response.Write "<font color=red>���ź����÷�������֧�� " & strclass & " �����</font>"
  Else
	if VerObj="" or isnull(VerObj) then 
	  Verobj1="�޷�ȡ�ø�����汾"
	Else
	  Verobj1="������汾�ǣ�" & VerObj
	End If
	Response.Write "<font class=fonts>��ϲ���÷�����֧�� " & strclass & " �����" & verobj1 & "</font>"
  End If
end if
%>
  </td></tr>
  </FORM>
</table>

<h3>�� ����ϵͳ�Դ������</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">������Ƽ����</td><td width="120">֧��/�汾</td></tr>
  <%
  k=0
  for i=0 to 13
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<h3>�� �����ļ��ϴ��͹������</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">������Ƽ����</td><td width="120">֧��/�汾</td></tr>
  <%
  k=1
  for i=0 to 5
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<h3>�� �����ʼ��������</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">������Ƽ����</td><td width="120">֧��/�汾</td></tr>
  <%
  k=2
  for i=0 to 6
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<h3>�� �����������</h3>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr class="backs"><td width="380">������Ƽ����</td><td width="120">֧��/�汾</td></tr>
  <%
  k=3
  for i=0 to 8
    call ObjTest(OtT(k,i,0))
  %>
  <tr><td width="380"><%=OtT(k,i,0) & " <font color='#888888'>" & OtT(k,i,1) & "</font>"%></td><td width="120" title="<%=VerObj%>"><%=cIsReady(isobj) & " " & left(VerObj,10)%></td></tr>
  <%next%>
</table>

<p>[<a href="http://www.ajiang.net/products/aspcheck/coms.asp">�鿴�����������ϸ���ܺ����ص�ַ</a>]
<%
	
end sub






' *******************************************************************************
' ����[ H ] ������Ϣ
' *******************************************************************************
sub disklist()
  on error resume next

  ObjTest("Scripting.FileSystemObject")
  if isobj then
	set fsoobj=server.CreateObject("Scripting.FileSystemObject")

%>

<h2>���̺��ļ���</h2>

<h3>�� ������������Ϣ</h3>

<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr align=center class="backs">
	<td width="100">�̷��ʹ�������</td>
	<td width="50">����</td>
	<td width="110">���</td>
	<td width="80">�ļ�ϵͳ</td>
	<td width="80">���ÿռ�</td>
	<td width="80">�ܿռ�</td>
  </tr>
<%

	' ���Դ�����Ϣ���뷨���ԡ�COCOON ASP ̽�롱
	
	set drvObj=fsoobj.Drives
	for each d in drvObj
%>
  <tr align="center" class="backq">
	<td align="right"><%=cdrivetype(d.DriveType) & " " & d.DriveLetter%>:</td>
<%
	if d.DriveLetter = "A" then	'Ϊ��ֹӰ������������������
		Response.Write "<td></td><td></td><td></td><td></td><td></td>"
	else
%>
	<td><%=cIsReady(d.isReady)%></td>
	<td><%=d.VolumeName%></td>
	<td><%=d.FileSystem%></td>
	<td align="right"><%=cSize(d.FreeSpace)%></td>
	<td align="right"><%=cSize(d.TotalSize)%></td>
<%
	end if
%>
  </tr>
<%
	next
%>
</td></tr>
</table>
<p>��<font color=red><b>��</b></font>����ʾ����û�о������ߵ�ǰIISվ��û�жԸô��̵Ĳ���Ȩ�ޡ�

<h3>�� ��ǰ�ļ�����Ϣ</h3>
<%

Response.Flush


	dPath = server.MapPath("./")
	set dDir = fsoObj.GetFolder(dPath)
	set dDrive = fsoObj.GetDrive(dDir.Drive)
%>
<p>�ļ���: <%=dPath%></p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr height="18" align="center" class="backs">
	<td width="75">���ÿռ�</td>
	<td width="75">���ÿռ�</td>
	<td width="75">�ļ�����</td>
	<td width="75">�ļ���</td>
	<td width="200">����ʱ��</td>
  </tr>
  <tr height="18" align="center" class="backq">
	<td><%=cSize(dDir.Size)%></td>
	<td><%=cSize(dDrive.AvailableSpace)%></td>
	<td><%=dDir.SubFolders.Count%></td>
	<td><%=dDir.Files.Count%></td>
	<td><%=dDir.DateCreated%></td>
  </tr>
</td></tr>
</table>

<%
Response.Flush

end if
end sub





' *******************************************************************************
' ����[ I ] �����ٶ�
' *******************************************************************************
sub diskspeed()
  on error resume next

  %>
  <h2>�����ļ������ٶȲ���</h2>
  <%
  ObjTest("Scripting.FileSystemObject")
  if isobj then
	set fsoobj=server.CreateObject("Scripting.FileSystemObject")
	' �����ļ���д���뷨���ԡ��Գ����ӡ�
	
	Response.Write "<p>�����ظ�������д���ɾ���ı��ļ�50��..."

	dim thetime3,tempfile,iserr

    iserr=false
	t1=timer
	tempfile=server.MapPath("./") & "\aspchecktest.txt"
	for i=1 to 50
		Err.Clear

		set tempfileOBJ = FsoObj.CreateTextFile(tempfile,true)
		if Err <> 0 then
			Response.Write "�����ļ�����<br><br>"
			iserr=true
			Err.Clear
			exit for
		end if
		tempfileOBJ.WriteLine "Only for test. Ajiang ASPcheck"
		if Err <> 0 then
			Response.Write "д���ļ�����<br><br>"
			iserr=true
			Err.Clear
			exit for
		end if
		tempfileOBJ.close
		Set tempfileOBJ = FsoObj.GetFile(tempfile)
		tempfileOBJ.Delete 
		if Err <> 0 then
			Response.Write "ɾ���ļ�����<br><br>"
			iserr=true
			Err.Clear
			exit for
		end if
		set tempfileOBJ=nothing
	next
	t2=timer
    if iserr <> true then
	thetime3=cstr(int(( (t2-t1)*10000 )+0.5)/10)
	Response.Write "...����ɣ�<font color=red>" & thetime3 & "����</font>��<br>"
	Response.Flush

%>
</p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr align=center class="backs">
	<td width=350>�� �� �� �� �� �� ��</td>
	<td width=150>���ʱ��(����)</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.zitian.cn/">���������λ�II����������</a></td><td>&nbsp;31��78</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.gdxf.net/wzkj/index.htm">�·���Ϣ�۸���ASP+CGI�ռ�</a></td><td>&nbsp;46��62</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.68l.com/">68����</a></td><td>&nbsp;78</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.diy5.com">��5�ռ�diy5.com����ǿ����<font color=#888888>(P42.4,2GddrEcc,SCSI72.8G)</font></a></td><td>&nbsp;46��78</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.100u.com/?come=aspcheck&keyword=��������">���ſƼ� 100u ����</a></td><td>&nbsp;31��62</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.3366.com.cn">�����������</a></td><td>&nbsp;31��62</td>
  </tr>
  <tr>
	<td><font color=red>��̨������: <%=Request.ServerVariables("SERVER_NAME")%></font>&nbsp;</td><td>&nbsp;<font color=red><%=thetime3%></font></td>
  </tr>
</table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/serverlist.asp" target="_blank">����ռ��̵ļ�ʱʵ������</a>]</p>
<%
end if

Response.Flush
	
	set fsoobj=nothing

end if
end sub






' *******************************************************************************
' ����[ J ] �ű������ٶ�
' *******************************************************************************
sub tspeed()
%>
<h2>ASP�ű����ͺ������ٶȲ���</h2><p>
<%
Response.Flush

	'��л����ͬѧ¼ http://www.5719.net �Ƽ�ʹ��timer����
	'��Ϊֻ����50��μ��㣬����ȥ�����Ƿ����ѡ���ֱ�Ӽ��
	
	Response.Write "����������ԣ����ڽ���50��μӷ�����..."
	dim t1,t2,lsabc,thetime,thetime2
	t1=timer
	for i=1 to 500000
		lsabc= 1 + 1
	next
	t2=timer
	thetime=cstr(int(( (t2-t1)*10000 )+0.5)/10)
	Response.Write "...����ɣ�<font color=red>" & thetime & "����</font>��<br>"


	Response.Write "����������ԣ����ڽ���20��ο�������..."
	t1=timer
	for i=1 to 200000
		lsabc= 2^0.5
	next
	t2=timer
	thetime2=cstr(int(( (t2-t1)*10000 )+0.5)/10)
	Response.Write "...����ɣ�<font color=red>" & thetime2 & "����</font>��<br>"
%></p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr align=center class="backs">
	<td width=350>�����յķ����������ʱ��(����)</td>
    <td width=75>��������</td><td width=75>��������</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.100u.com?come=aspcheck&keyword=��������"
	>���ſƼ� 100u ����, <font color=#888888>2003-11-1</font></a></td><td>&nbsp;181��233</td><td>&nbsp;156��218</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.west263.net/index.asp?ads=ajiang"
	>�������� west263 ����, <font color=#888888>2003-11-1</font></a></td><td>&nbsp;171��233</td><td>&nbsp;156��171</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.163n.com "
	>������� 163n ����,  <font color=#888888>2006-1-4</font></a></td><td>&nbsp;156��171</td><td>&nbsp;140��156</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.9s5.com/"
	>������www.9s5.comȫ����(ASP+PHP+JSP)����,<font color=#888888>2003-11-1</font></a></td><td>&nbsp;171��187</td><td>&nbsp;156��171</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.dnsmy.com/"
	>��Ѷ���� Dnsmy ����, <font color=#888888>2003-11-1</font></a></td><td>&nbsp;155��180</td><td>&nbsp;122��172</td>
  </tr>
  <tr>
	<td><a class="a1" target="_blank" href="http://www.senye.com"
	>ʤ���� senye.com ����, <font color=#888888>2004-3-28</font></a></td><td>&nbsp;156��171</td><td>&nbsp;140��156</td>
  </tr>
  <tr>
	<td><font color=red>��̨������: <%=Request.ServerVariables("SERVER_NAME")%></font>&nbsp;</td><td>&nbsp;<font color=red><%=thetime%></font></td><td>&nbsp;<font color=red><%=thetime2%></font></td>
  </tr>
</table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/serverlist.asp" target="_blank">����ռ��̵ļ�ʱʵ������</a>]</p>
<%
end sub






' *******************************************************************************
' ����[ K ] ���������ٶȲ���
' *******************************************************************************
sub tnet()
%>
<h2>���Ӵ������</h2><a name="K"></a>
<%
 if T<>"K" then
%>
<p>[<a href="?T=K">��ʼ����</a>]</p>
<%
 else
  haveok=false

  if Request("ok") <> "" then haveok=true
  if Request("tm") = "" then haveok=false

  if haveok=false then
%>
<p>���ڲ������뵱ǰ������֮��������ٶȣ����Ե�...<span id="baifen">.</span></p>
<script language="javascript" type="text/javascript">
var acd1;
acd1 = new Date();
acd1ok=acd1.getTime();
</script>
<%
Response.Flush
for i=1 to 1000
  Response.Write "<!--567890#########0#########0#########0#########0#########0#########0#########0#########012345-->" & vbcrlf
  if i mod 100=0 then
%>
<script language="javascript" type="text/javascript">
document.getElementById('baifen').innerHTML = '<%=i/10%>%';
</script>
<%
  end if
next
%>
<script language="javascript" type="text/javascript">
var acd2;
acd2 = new Date();
acd2ok=acd2.getTime();
window.location = '?T=K&ok=ok&tm=' + (acd2ok-acd1ok)
</script>
<%
Response.Flush :Response.end

  else

ttime=clng(Request("tm")) + 1

tnetspeed=100000/(ttime)
tnetspeed2=tnetspeed * 8
twidth=int(tnetspeed * 0.16)+5

if twidth> 300 then twidth=300
tnetspeed=formatnumber(tnetspeed,2,,,0)
tnetspeed2=formatnumber(tnetspeed2,2,,,0)

%><p>������ɣ���ͻ��˴��� 100k �ֽ����ݹ�ʹ��ʱ�� <%=formatnumber(ttime,2)%> ���롣[<a href="?T=K">�ز�</a>]
</p>
<table border=0 width=500 cellspacing=1 cellpadding=3>
  <tr><td align="center" style="padding:10px 0px">
  <table style="margin:0px;border:none" align="center" width="400" border="0" cellspacing=0 cellpadding=0>
    <tr><td width="45">| 56kè</td><td width="160">| 2M ADSL</td><td width=200>| 10M LAN</td></tr>
  </table>
  <table style="margin:0px" class="input" align="center" width="400" border="0" cellspacing=0 cellpadding=0>
    <tr class="input"><td width="<%=twidth%>" class="backs"></td><td width="<%=400-twidth%>">&nbsp;<%=tnetspeed%> kB/s</td></tr>
  </table>
  <p style="margin:10px 0px 0px 0px">����˷������������ٶ��� <%=tnetspeed%> kB/s (�൱��<%=tnetspeed2%>kbps)
  <br><font color="#888888">�����ϵ:  1 Byte(�ֽ�) = 8 bit(λ)</font></p>
  </td></tr>
</table>
<%

  end if
 end if
end sub






' *******************************************************************************
' ����[ L ] ����ȫ������
' *******************************************************************************
sub tsafe()
%>


<h2>����ȫ������</h2>
<p>WScript.Shell <%=okObj("ws")%>, Shell.application <%=okObj("app")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>Shell �������ASP����.exe�ȿ�ִ���ļ����������صİ�ȫ��������ʹ���ļ�ϵͳ���й��ϸ��Ȩ�����õķ������ϣ������Ҳ�ᱻ������������Ȩ�޵ĳ���</td>
    </tr>
  </table>
<p>WScript.Network <%=okObj("net")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>WScript.Network ΪASP�������кʹ���ϵͳ�û�(�M)�ṩ�˿��ܣ����������ʾ���� Σ�ա�����ܴ��ڴ˰�ȫ������</td>
    </tr>
  </table>
<p>Adodb.Stream <%=okObj("ado")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>Adodb.Stream �����������ϴ�ľ��Ȳ���ȫ���򣬴Ӷ����󹥻��ߵ��ƻ�������ͨ����Ҫ��Ȩ�����ã�Adodb.Stream�����ϵͳ��ȫ�����в����������ʹ����������ϴ������С�</td>
    </tr>
  </table>
<p>FSO <%=okObj("fso")%>, XML V1.0 <%=okObj("x1")%>, V2.6 <%=okObj("x2")%>, V3.0 <%=okObj("x3")%>, V4.0 <%=okObj("x4")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>FSO(Scripting.FileSystemObject) �� XML �߱����к͹�����������ļ����ļ��е����������Ȩ�����ò�����������ľ������ƶ����޸�����ɾ���������ϵ��ļ���FSO ����ǳ��õ����֮һ�����ø����������������İ�ȫ��ʩ��</td>
    </tr>
  </table>
<p>HappyTime <%=okObj("hap")%></p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr>
      <td>HappyTime(����ʱ��)�����е�������没��֮һ�����ķ�ֳռ�ô������������������ʱ���п���ɾ�������������õĿ�ִ���ļ�����ϵͳ̱���������������ΪΣ�գ������ķ����������ڸ�Ⱦ�ʹ�������ʱ�ⲡ���Ŀ��ܡ�</td>
    </tr>
  </table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/safe.asp">�������ο������İ�ȫ���÷���</a>]
<%
end sub






' *******************************************************************************
' ����[ M ] ϵͳ�û��ͽ��̼��
' *******************************************************************************
sub userlist()
%>
<h2>ϵͳ�û�(�M)�ͽ��̼��</h2>
<p>��������г���ϵͳ�û��ͽ��̣���˵��ϵͳ���ڰ�ȫ������</p>
  <table border=0 width=500 cellspacing=1 cellpadding=3>
    <tr class="backs">
      <td width="100">�� ��</td><td width="400">���Ƽ�����</td>
    </tr>
<%
  on error resume next
    for each obj in getObject("WinNT://.")
	err.clear
%>
    <tr>
      <td align=center><!--<%=obj.path%>-->
<%
    if err then
      Response.Write "ϵͳ�û�(�M)"
    else
      Response.Write "ϵͳ����"
    end if
%>
      </td>
	  <td><%=obj.Name%><%if err=0 then Response.Write " (" & obj.displayname & ")"%><br><%=obj.path%>
	  </td>
	</tr>
<%
	next 
%>
  </table>
<p>[<a href="http://www.ajiang.net/products/aspcheck/safe.asp">�������ο������İ�ȫ���÷���</a>]
<%
end sub




' *******************************************************************************
' ����[ N ] ���˵�
' *******************************************************************************
sub mmenu()
%>
<h2>���˵�</h2>
  <p>���ٲ鿴: <a href="?T=BG">����ģʽ</a> | <a href="?T=BGHIJ">����ģʽ</a> | <a href="?T=ABGHIJKLMCDEF">����ģʽ</a></p>
  <p>����ֱ��: <a href="?T=B">�ſ�</a>
  | <a href="?T=G">���</a>
  | <a href="?T=F">����</a>
  | <a href="?T=HI">����</a>
  | <a href="?T=J">�����ٶ�</a>
  | <a href="?T=K">������</a>
  | <a href="?T=LHM">��ȫ״��</a></p>
<%
end sub




' *******************************************************************************
' ���������������ӳ���
' *******************************************************************************

' չʾ��Ŀ
sub BodyGo(gCon)
  select case gCon
  case "A"
    call aspyes()
  case "B"
    call servinfo()
  case "C"
    call applist()
  case "D"
    call seslist()
  case "E"
    call sevalist()
  case "F"
    call wsslist()
  case "G"
    call comlist()
  case "H"
    call disklist()
  case "I"
    call diskspeed()
  case "J"
    call tspeed()
  case "K"
    call tnet()
  case "L"
    call tsafe()
  case "M"
    call userlist()
  case "N"
    call mmenu()
  end select
end sub


' ��ⲻ��ȫ���
Function okObj(runstr)
  On Error Resume Next
  Response.Write "<span style=""display:none"">"
  okObj = true
  Err = 0
  Execute runstr & ".exec()"
  If 429 = Err Then
    okObj = false
  end if
  Err = 0
  Response.Write "</span>"
  if okObj then
    okObj="<font color=""red"">�� Σ��</font>"
  else
    okObj="<font color=""green"">�� ��ȫ</font>"
  end if
End Function

' ת���ִ�ΪHTML����
function cHtml(iText)
  cHtml = iText
  cHtml = server.HTMLEncode(cHtml)
  cHtml = replace(cHtml,chr(10),"<br>")
end function

' ת����������Ϊ����
function cdrivetype(tnum)
  Select Case tnum
    Case 0: cdrivetype = "δ֪"
    Case 1: cdrivetype = "���ƶ�����"
    Case 2: cdrivetype = "����Ӳ��"
    Case 3: cdrivetype = "�������"
    Case 4: cdrivetype = "CD-ROM"
    Case 5: cdrivetype = "RAM ����"
  End Select
end function

' ���Ƿ����ת��Ϊ�Ժźʹ��
function cIsReady(trd)
  Select Case trd
    case true: cIsReady="<font class=fonts><b>��</b></font>"
    case false: cIsReady="<font color='red'><b>��</b></font>"
  End Select
end function

' ת���ֽ���Ϊ��д��ʽ
function cSize(tSize)
  if tSize>=1073741824 then
    cSize=int((tSize/1073741824)*1000)/1000 & " GB"
  elseif tSize>=1048576 then
    cSize=int((tSize/1048576)*1000)/1000 & " MB"
  elseif tSize>=1024 then
    cSize=int((tSize/1024)*1000)/1000 & " KB"
  else
    cSize=tSize & "B"
  end if
end function

'�������Ƿ�֧�ּ�����汾���ӳ���
sub ObjTest(strObj)
  'on error resume next
  IsObj=false
  VerObj=""
 
  set TestObj=server.CreateObject (strObj)
  
  If -2147221005 <> Err then		'��л����iAmFisher�ı�����
    IsObj = True
    VerObj = TestObj.version
    if VerObj="" or isnull(VerObj) then VerObj=TestObj.about
  end if
  set TestObj=nothing
End sub

%>