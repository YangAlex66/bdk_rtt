<!-- #include file="connmdb.asp" -->
<htm>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=GB2312" >
</head>
<body>
<center>
<%
response.write "���IP:" & request.ServerVariables("REMOTE_ADDR") & "<BR>"
d=request.querystring("d")
k=request.querystring("k")
if d="" then d="id"
if d="1" then d="id" 
if d="2" then d="ʹ�ÿ���" 
if d="3" then d="IP��ַ" 

set rs=server.createobject("adodb.recordset")
sql="select id,ʹ�ÿ���,��������,IP��ַ,MAC��ַ from pcinfo order by " & d & " "  & k
rs.open sql,conn,1,1
if k="" then k="DESC" else k=""
%>

<A style="font-size:20px"><b>�칫�õ���IP��ַ�ǼǱ�</b></a><br /><br />
���ƣ�<%=rs.recordcount%>̨���ԡ�
<br /><br />

</div>

<table border="1" bordercolor="#bbbbFF"  bordercolorlight="#335566" bgcolor="#4084aE">
<thead>
<tr  style="text-align:center;"><td onclick="window.location='?d=1&k=<%=k%>'" style="cursor: default;">ID</td><td style="cursor: default;" onclick="window.location='?d=2&k=<%=k%>'">ʹ�õ�λ(����)</td><td style="cursor: default;" onclick="window.location='?d=3&k=<%=k%>'">�������</td><td style="cursor: default;" onclick="window.location='?d=4&k=<%=k%>'">IP��ַ</td><td style="cursor: default;" onclick="window.location='?d=5&k=<%=k%>'">MAC��ַ</td></tr></thead><tbody><%
do while not rs.eof %>


<tr  STYLE="COLOR='#ffbb22';"><td ><%=rs(0)%></td><td><%=rs(1)%></td><td><%=rs(2)%></td><td><%=ip(rs(3))%></td><td><%=rs(4)%></td></tr>
<%
rs.movenext
loop
set rs=nothing
set conn=nothing
 

function ip(t)
ip=""
'on error resume next
a=split(t,".")

for i=0 to 3
if ip<>"" then ip=ip & "."
if a(i)="" then a(i)="0"
 ip=ip & cstr(cint(a(i)))
next
end function
%>
</tbody>
</table>
</center>
</body>
</html>