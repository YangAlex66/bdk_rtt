<!--#include file="conn.asp"-->
<%
'Response.Addheader "Content-Type","text/html; charset=gb2312"
Response.Charset="GB2312"
ip=request.servervariables("REMOTE_ADDR")
title=nohtml(request.form("title"))
name=nohtml(request.form("name"))
tel=nohtml(request.form("tel"))
content=nohtml(request.form("content"))
code=Get_SafeStr(request.form("code"))


Function telpd(strPassword)
	Dim regEx
	Set regEx = new RegExp
	regEx.IgnoreCase = false
	regEx.global = false
	regEx.Pattern = "(^0[0-9]{2,3}-[0-9]{7,8}$)|(^0?1[0-9]{10}$)"
	if regEx.Test(strPassword) then
		telpd = true
	else
		telpd = false
	end if
	set regEx = Nothing
End Function

function incode(fString)
if not isnull(fString) then
    fString = Replace(fString, CHR(9), "&nbsp;")
    fString = Replace(fString, CHR(10) & CHR(10), "</p><p>")
    fString = Replace(fString, CHR(10), "<br/>")
    incode = fString
end if
end function

if Code<>CStr(session("CheckCode")) then
response.write "��֤�벻��ȷ"
response.end
end if

if title=""  then
response.write "�������ⲻ��Ϊ��"
response.end
end if
if name="" then
response.write "������������Ϊ��"
response.end
end if

if telpd(tel)=false then
response.write "�绰������д����ȷ,��������д����ʽ�磺0592-5983163��13400693163"
response.end
end if

if content="" then
response.write "���ݲ���Ϊ��"
response.end
end if
content=incode(content)

conn.execute("insert into book(title,name,tel,content,ip) values('"&title&"','"&name&"','"&tel&"','"&content&"','"&ip&"')") 
response.write "�������Գɹ�!��ȴ�����Ա��ˡ�"
conn.close:set conn=nothing
response.end
%>
