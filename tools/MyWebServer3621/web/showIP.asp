<!-- #include file="connmdb.asp" -->
<htm>
<head>
<meta http-equiv="Content-Type" content="text/html;charset=GB2312" >
<SCRIPT src="sorttable.js"></SCRIPT>
</head>
<body>
<center>
<script>

function openquery(obj)
{   
        
           
                
                 document.all.querytj.style.display='block';
           
		   form1.reset();
		   document.form1.edit.value="0";
		   document.form1.id.value="";
		    document.form1.subm.value="��Ӽ�¼";
}
function checkv()
{
var tmp;
tmp="";
if(document.form1.dw.value=="")
tmp="��ʹ�õ�λ��"
if(document.form1.ip.value=="")
tmp=tmp+" ��IP��ַ��"
if(document.form1.mac.value=="")
tmp=tmp+" ��MAC��ַ��"
if(tmp=="")
return true;
else 
{
alert(tmp+"��Ŀ����Ϊ�գ�");
return false;
}
}
</script>

<%
'if instr(request.ServerVariables("REMOTE_ADDR"),"10.2.140.")=1 then
d=request.querystring("d")
k=request.querystring("k")
if d="" then d="id"
'if d="1" then d="id" 
'if d="2" then d="ʹ�ÿ���" 
'if d="3" then d="IP��ַ" 
set rs1=server.createobject("adodb.recordset")
set rs=server.createobject("adodb.recordset")
sql="select * from pcinfo order by " & d & " "  & k
rs.open sql,conn,1,1


if k="asc" or k="" then 
k="DESC" 
else 
k="asc"
end if 

%>


<A style="font-size:20px"><b>����IP��ַ�ǼǱ�</b></a><br /><br />
���ƣ�<%=rs.recordcount%>̨���ԡ�<button onClick="openquery(this); ">�� ��</button>
<br /><br />
<div id="querytj" style="display:none">

<form  name="form1" action="editIP.asp" method="post" onSubmit="return checkv()" >ʹ�õ�λ<INPUT type="TEXT" id="dw" name="dw" height="16PX"  ></INPUT>
  �������<INPUT type="TEXT" id="pz" name="pz" height="16PX"  ></input>��ע<INPUT type="TEXT" name="bz" height="16PX"  ></input><br><br>
  IP��ַ
  <INPUT type="TEXT" id="ip" name="ip" height="16PX"  ></input>
 ʹ����<INPUT type="TEXT" id="r" name="r" height="16PX"  ></input>
MAC��ַ<INPUT type="TEXT" id="mac" name="mac" height="16PX"  ></input><input id="subm" type="submit" value="ȷ���޸�"></input>
<input id="edit" type="hidden" name="edit" value="<%=request.QueryString("edit")%>" /><input id="id" type="hidden" name="id" value="<%=request.QueryString("id")%>" />
</form>


</div>
<br />
<%
if request.QueryString("edit")="1" and request.QueryString("id") <>"" then 
sqla="select * from pcinfo where id=" & request.QueryString("id")
rs1.open sqla,conn,1,1
%>
<script>

document.all.querytj.style.display='block';
document.all.form1.dw.value="<%=rs1(1)%>";
document.all.form1.r.value="<%=rs1(2)%>";
document.all.form1.pz.value='<%=rs1(4)%>';
document.all.form1.ip.value="<%=ip(rs1(3))%>";
document.all.form1.mac.value="<%=rs1(5)%>";
document.all.form1.bz.value="<%=rs1(6)%>";

</script>
<%
end if
%>
<table border="1" bordercolor="#bbbbFF"  bordercolorlight="#335566" bgcolor="#4084aE">

<tr  style="text-align:center;"><td onclick="window.location='?d=1&k=<%=k%>'" style="cursor: default;">ID</td><td style="cursor: default;" onclick="window.location='?d=2&k=<%=k%>'">ʹ�õ�λ(����)</td><td style="cursor: default;" onclick="window.location='?d=3&k=<%=k%>'">ʹ����</td><td style="cursor: default;" onclick="window.location='?d=4&k=<%=k%>'">IP��ַ</td><td style="cursor: default;" onclick="window.location='?d=5&k=<%=k%>'">�������</td><td style="cursor: default;" onclick="window.location='?d=6&k=<%=k%>'">MAC��ַ</td><td style="cursor: default;" onclick="window.location='?d=7&k=<%=k%>'">��ע</td></tr><%
do while not rs.eof %>
<tr onmouseover='this.style.backgroundColor="#aaaaff"'  onmouseout='this.style.backgroundColor="#4084aE"' ondblclick="window.location='?edit=1&id=<%=rs(0)%>&d=<%=request.querystring("d")%>&k=<%=request.querystring("k")%>';" STYLE="COLOR='#ffbb22';"><td ><%=rs(0)%></td><td><%=rs(1)%></td><td><%=rs(2)%></td><td><%=ip(rs(3))%></td><td><%=rs(4)%></td><td><%=rs(5)%></td><td><%=rs(6)%></td></tr>
<%
rs.movenext
loop




set rs=nothing
set rs1=nothing
set conn=nothing
'end if
function ip(t)
ip=""
on error resume next
a=split(t,".")

for i=0 to 3
if ip<>"" then ip=ip & "."
 if a(i)="" then a(i)="0"
 ip=ip & cstr(cint(a(i)))
next
end function
%>
</table>
</center>
</body>
</html>