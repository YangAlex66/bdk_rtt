<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<!--#include Virtual = "conn.asp"-->
<!--#include file="fydy.asp"-->
<%
set rs=conn.execute("select top 1 * from admin")
webmc=rs("wzmc")
bq=rs("bq")
sh=rs("sh")
fyts=clng(rs("fyts"))
if fyts=0 then fyts=5
rs.close:set rs=nothing

Function IsSelfRefer()
Dim sHttp_Referer, sServer_Name
sHttp_Referer = CStr(Request.ServerVariables("HTTP_REFERER"))
sServer_Name = CStr(Request.ServerVariables("SERVER_NAME"))
If Mid(sHttp_Referer, 8, Len(sServer_Name)) = sServer_Name Then
IsSelfRefer = True
Else
IsSelfRefer = False
End If
End Function

function getip(ip)     

       strIpArr = Split(ip,".")
 
       strGetUserIP = strIpArr(0)
       strGetUserIP = strGetUserIP & "." & strIpArr(1)
       strGetUserIP = strGetUserIP & "." & strIpArr(2)
       strGetUserIP = strGetUserIP & ".*"
       getip=strGetUserIP
end function


'�������� 
Set mypage=new xdownpage 
'�õ����ݿ����� 
mypage.getconn=conn 
'sql��� 
if sh=1 then
mypage.getsql="select * from book where sh=1 order by zd desc, id desc"
else
mypage.getsql="select * from book order by zd desc, id desc"
end if

'����ÿһҳ�ļ�¼������Ϊ5�� 
mypage.pagesize=fyts
'����Recordset
set rs=mypage.getrs()  
'��ʾ��ҳ��Ϣ������������ԣ���set rs=mypage.getrs()�Ժ�,��������λ�õ��ã����Ե��ö�� 

%>
<head>
<meta content="text/html; charset=gb2312" http-equiv="Content-Type" />
<title><%=webmc%></title>
<meta content="<%=webmc%>" name="keywords" />
<meta content="<%=webmc%>" name="description" />
<link rel="stylesheet" type="text/css" href="www.163sy.cn.css"  />
<script type="text/javascript" src="js/jquery-1.4.2.js"></script>
<script type="text/javascript">
   $(document).ready(function() {  	   
	   $("#tj").click(function(){
		      $("#lysave").hide();$("#ts").show();
		      $.post("booksave.asp", $("#lysave").serialize(),function(data,textStatus){
                      alert(data);
                      $("#lysave").show();$("#ts").hide();
                      if(data=="�������Գɹ�!��ȴ�����Ա��ˡ�"){
                      $("#lysave")[0].reset();
                      $("#yzt").attr("src","checkcode.asp?t="+new Date().getTime())
                      }
                      if(data=="��֤�벻��ȷ"){
                      $("#yzt").attr("src","checkcode.asp?t="+new Date().getTime())
                      } 

					}
			);
	   })
	    
	})

</script>
</head>
<body>
<div id="book">
<div id="top" style="color:red"><%=webmc%></div>
<div class="cyleft1">
<div class="rbt1"><!--<span style="float:right;padding-right:10px;">δ������ԣ�<b style="color:red;"><%=conn.execute("select count(*) from book where sh<>1")(0)%>��</b></span>/-->����λ�ã�������ҳ �� �����б�</div>   

</div>
<div class="cyleft2">
<%

for i=1 to mypage.pagesize 
 
if not rs.eof then 
title=gotTopic(rs("TITLE"),100)

lyhf=rs("hf")
if lyhf<>"" then
lyhf=lyhf
else
if rs("sh")=1 then
lyhf="��������� ���޻ظ���"
else
lyhf="���Դ����У�"
end if
end if


response.write"<ul class='booklb'>"
response.write"<li class='lyzt'><span class='lynr1' style='color:green'>ʱ�䣺"&Format_Time(rs("sj"),4)&"</span>���ԣ�"&i&"  ���⣺<font style='color:blue'>"&title&"</font></li>"& vbCrLf

response.write"<li class='lynr'><label class='lybt'>�������ݣ�</label><br/>"&rs("content")&"<br/><span class='lysx'>������"&rs("name")&" �绰��" &left(rs("tel"),6)&"******* IP��"&getip(rs("ip"))&"</span></li>"& vbCrLf
 
response.write"<li class='lyhf'><label class='lybt'><font style='color:red'>�ظ����ݣ�</font></label><br/>"&lyhf&"</li>"& vbCrLf
response.write "</ul>"
 
 rs.movenext 
 else 
exit for 
end if
 next
                                                  
%>
<div class='clear'></div>
<div class="xtfy"><% mypage.showpage() %></div>
<div class="qxly">�������������</div>
<div id="ts" style="display:none;text-align:center;color:red;font-size:14px">�����ύ�����У����Ժ�</div>
<form id="lysave">
<ol class="lylb">
<li>�������⣺<input class="bd" name="title" size="50" type="text" /> *</li>
<li>����������<input class="bd" name="name" type="text" /> *</li>
<li>��ϵ�绰��<input class="bd" name="tel" type="text" /> * ��ʽ�磺0592-5983163��13400693163</li> 
<li><span>�������ݣ�</span><textarea class="bd" cols="80" name="content" rows="5"></textarea></li>
<li>�� ֤ �룺<input class=wenbenkuang type="text" name="code" maxLength=4 size=5> <img id="yzt" src="checkcode.asp"/></li>
<li><input style="margin-left:60px" name="Submit1" id="tj" type="button" value="�ύ" /> <input type="reset" value="����"/></li>
</ol>
</form>
</div>
<div class="cyleft3"></div>
<div id="bottom"><%=bq%></div>
</div>
</body>
</html>
<%
rs.close
set rs=nothing
conn.close
set conn=nothing
%>
