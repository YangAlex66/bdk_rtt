<%
Function Get_SafeStr(str)
Get_SafeStr = Replace(Replace(Replace(Replace(Replace(Trim(str), "'", ""), Chr(34), ""), ";", ""),"=",""),">","")
End Function

db="../db/sywl.asp"       
         On Error Resume Next
	dim ConnStr
	dim conn
		ConnStr = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=" & Server.MapPath(db)
		Set conn = Server.CreateObject("ADODB.Connection")
	conn.Open connstr
	If Err Then
		err.Clear
		Set Conn = Nothing
		Response.Write "���ݿ����ӳ�������Conn.asp�ļ��е����ݿ�������á�"
		Response.End
	End If
sub CloseConn()
	On Error Resume Next
	If IsObject(Conn) Then
		conn.close
		set conn=nothing
	end if
end sub
%>
