<%
'=================================================================== 
'ʹ��˵�� 
'�������˵�� 
'PapgeSize �����ҳÿһҳ�ļ�¼�� 
'GetRS ���ؾ�����ҳ��Recordset������ֻ�� 
'GetConn �õ����ݿ����� 
'GetSQL �õ���ѯ��� 
'���򷽷�˵�� 
'ShowPage ��ʾ��ҳ������,Ψһ�Ĺ��÷��� 
' 
'=================================================================== 

Const Btn_First="��ҳ" '�����һҳ��ť��ʾ��ʽ 
Const Btn_Prev="��һҳ" '����ǰһҳ��ť��ʾ��ʽ 
Const Btn_Next="��һҳ" '������һҳ��ť��ʾ��ʽ 

Const Btn_Last="βҳ" '�������һҳ��ť��ʾ��ʽ 
Const XD_Align="Center" '�����ҳ��Ϣ���뷽ʽ 
Const XD_Width="100%" '�����ҳ��Ϣ���С 

Class Xdownpage 
Private XD_PageCount,XD_Conn,XD_Rs,XD_SQL,XD_PageSize,Str_errors,int_curpage,str_URL,int_totalPage,int_totalRecord,XD_sURL 


'================================================================= 
'PageSize ���� 
'����ÿһҳ�ķ�ҳ��С 
'================================================================= 
Public Property Let PageSize(int_PageSize) 
If IsNumeric(Int_Pagesize) Then 
XD_PageSize=CLng(int_PageSize) 
Else 
str_error=str_error & "PageSize�Ĳ�������ȷ" 
ShowError() 
End If 
End Property 
Public Property Get PageSize 
If XD_PageSize="" or (not(IsNumeric(XD_PageSize))) Then 
PageSize=10 
Else 
PageSize=XD_PageSize 
End If 
End Property 

'================================================================= 
'GetRS ���� 
'���ط�ҳ��ļ�¼�� 
'================================================================= 
Public Property Get GetRs() 
Set XD_Rs=Server.createobject("adodb.recordset") 
XD_Rs.PageSize=PageSize 
XD_Rs.Open XD_SQL,XD_Conn,1,1 
If not(XD_Rs.eof and XD_RS.BOF) Then 
If int_curpage>XD_RS.PageCount Then 
int_curpage=XD_RS.PageCount 
End If 
XD_Rs.AbsolutePage=int_curpage 
End If 
Set GetRs=XD_RS 
End Property 

'================================================================ 
'GetConn �õ����ݿ����� 
' 
'================================================================ 
Public Property Let GetConn(obj_Conn) 
Set XD_Conn=obj_Conn 
End Property 

'================================================================ 
'GetSQL �õ���ѯ��� 
' 
'================================================================ 

Public Property Let GetSQL(str_sql) 
XD_SQL=str_sql 
End Property 



'================================================================== 
'Class_Initialize ��ĳ�ʼ�� 
'��ʼ����ǰҳ��ֵ 
' 
'================================================================== 
Private Sub Class_Initialize 
'======================== 
'�趨һЩ�������a��ֵ 
'======================== 
XD_PageSize=10 '�趨��ҳ��Ĭ��ֵΪ10 
'======================== 
'��ȡ��ǰ���ֵ 
'======================== 
If request("page")="" Then 
int_curpage=1 
ElseIf not(IsNumeric(request("page"))) Then 
int_curpage=1 
ElseIf CInt(Trim(request("page")))<1 Then 
int_curpage=1 
Else 
Int_curpage=CInt(Trim(request("page"))) 
End If 

End Sub 

'==================================================================== 
'ShowPage ������ҳ������ 
'����ҳ��ǰһҳ����һҳ��ĩҳ���������ֵ��� 
' 
'==================================================================== 
Public Sub ShowPage() 
Dim str_tmp 
XD_sURL = GetUrl() 
int_totalRecord=XD_RS.RecordCount 
If int_totalRecord<=0 Then 
str_error=str_error & "�ܼ�¼��Ϊ�㣬����������" 
Call ShowError() 
End If 
If int_totalRecord="" then 
int_TotalPage=1 
Else 
If int_totalRecord mod PageSize =0 Then 
int_TotalPage = CLng(int_TotalRecord / XD_PageSize) 
Else 
int_TotalPage = int(int_TotalRecord / XD_PageSize)+1
End If 

End If 

If Int_curpage>int_Totalpage Then 
int_curpage=int_TotalPage 
End If 

'================================================================== 
'��ʾ��ҳ��Ϣ������ģ������Լ�Ҫ���������λ�� 
'================================================================== 
response.write "" 
str_tmp=ShowFirstPrv 
response.write str_tmp 
str_tmp=ShowNextLast 
response.write str_tmp 
str_tmp=ShowPageInfo 
response.write str_tmp 

str_tmp=showNumBtn 
response.write str_tmp 

response.write "" 
End Sub 

'==================================================================== 
'ShowFirstPrv ��ʾ��ҳ��ǰһҳ 
' 
' 
'==================================================================== 
Private Function ShowFirstPrv() 
Dim Str_tmp,int_prvpage 
If int_curpage=1 Then 
str_tmp=Btn_First&" "&Btn_Prev & " "
Else 
int_prvpage=int_curpage-1 
str_tmp="<a href="""&XD_sURL & "1" & """>" & Btn_First&"</a> <a href=""" & XD_sURL & CStr(int_prvpage) & """>" & Btn_Prev&"</a> " 
End If 
ShowFirstPrv=str_tmp 
End Function 

'==================================================================== 


'ShowNextLast ��һҳ��ĩҳ 
' 
' 
'==================================================================== 
Private Function ShowNextLast() 
Dim str_tmp,int_Nextpage 
If Int_curpage>=int_totalpage Then 
str_tmp=Btn_Next & " " & Btn_Last & " "
Else 
Int_NextPage=int_curpage+1 
str_tmp="<a href=""" & XD_sURL & CStr(int_nextpage) & """>" & Btn_Next&"</a> <a href="""& XD_sURL & CStr(int_totalpage) & """>" & Btn_Last&"</a> " 
End If 
ShowNextLast=str_tmp 
End Function 


'==================================================================== 
'ShowNumBtn ���ֵ��� 
' 
' 
'==================================================================== 
Private Function showNumBtn() 
Dim i,str_tmp 

str_tmp=str_tmp &"ת�� <select name=""select"" onchange=""javascript:window.location.href=this.options[this.selectedIndex].value"">"
For i=1 to int_totalpage 
'str_tmp=str_tmp & "[<a href=""" & XD_sURL & CStr(i) & """>"&i&"</a>] " 
if i=int_curpage then
str_tmp=str_tmp &"<option selected value='"&XD_sURL & CStr(i) &"'>"&i&"</option>"
else
str_tmp=str_tmp &"<option value='"&XD_sURL & CStr(i) &"'>"&i&"</option>"

end if
Next 
str_tmp=str_tmp &"</select> ҳ"
showNumBtn=str_tmp 
End Function 


'==================================================================== 
'ShowPageInfo ��ҳ��Ϣ 
'����Ҫ�������޸� 
' 
'==================================================================== 
Private Function ShowPageInfo() 
Dim str_tmp 
str_tmp="ҳ�Σ�"&int_curpage&"/"&int_totalpage&"ҳ ��"&int_totalrecord&"����¼ "&XD_PageSize&"��/ҳ " 
ShowPageInfo=str_tmp 
End Function 
'================================================================== 
'GetURL �õ���ǰ��URL 
'����URL������ͬ����ȡ��ͬ�Ľ�� 
' 
'================================================================== 
Private Function GetURL() 
Dim strurl,str_url,i,j,search_str,result_url 
search_str="page=" 

strurl=Request.ServerVariables("URL") 
Strurl=split(strurl,"/") 
i=UBound(strurl,1) 

str_url=strurl(i)'�õ���ǰҳ�ļ��� 

str_params=Trim(Request.ServerVariables("QUERY_STRING")) 
If str_params="" Then 
result_url=str_url & "?page=" 
Else 
If InstrRev(str_params,search_str)=0 Then 
result_url=str_url & "?" & str_params &"&page=" 
Else 
j=InstrRev(str_params,search_str)-2 
If j=-1 Then 
result_url=str_url & "?page=" 
Else 
str_params=Left(str_params,j) 
result_url=str_url & "?" & str_params &"&page=" 
End If 
End If 
End If 
GetURL=result_url 
End Function 

'==================================================================== 
' ���� Terminate �¼��� 
' 
'==================================================================== 
Private Sub Class_Terminate 
XD_RS.close 
Set XD_RS=nothing 
End Sub 
'==================================================================== 


'ShowError ������ʾ 
' 
' 
'==================================================================== 
Private Sub ShowError() 
If str_Error <> "" Then 
Response.Write("" & str_Error & "") 
Response.End 
End If 
End Sub 
End class 
%>