<!--#include file="guke_up.asp"-->
<%
const upload_type=0
dim upload,oFile,formName,SavePath,filename,fileExt,oFileSize
dim EnableUpload
dim arrUpFileType
dim ranNum
dim msg,FoundErr
msg=""
FoundErr=false
EnableUpload=false
%>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=gb2312">
<style type="text/css">
<!--
BODY{
BACKGROUND-COLOR: #E1F4EE;
font-size:9pt
}
.tx1 { height: 20px;font-size: 9pt; border: 1px solid; border-color: #000000; color: #0000FF}
-->
</style>
</head>
<body leftmargin="2" topmargin="5" marginwidth="0" marginheight="0" style="background-color: #D6DFF7" >
<%
		select case upload_type
			case 0
				call upload_0()  'ʹ�û���������ϴ���
			case else
				'response.write "��ϵͳδ���Ų������"
				'response.end
		end select
%>
</body>
</html>
<%
sub upload_0()    'ʹ�û���������ϴ���

	set upload=new upfile_class
	upload.GetData(104857600)             '��������ϴ�100M
	if upload.err > 0 then
		select case upload.err
			case 1
				response.write "����ѡ����Ҫ�ϴ����ļ���"
			case 2
				response.write "���ϴ����ļ��ܴ�С������������ƣ�100M��"
		end select
		response.end
	end if
        SavePath = "pic"                      '����ϴ��ļ���Ŀ¼
	if right(SavePath,1)<>"/" then SavePath=SavePath&"/"       '����ϴ��ļ���·��
		
	for each formName in upload.file
		set ofile=upload.file(formName)
		oFileSize=ofile.filesize
		if oFileSize<100 then
			msg="����ѡ����Ҫ�ϴ����ļ���"
			FoundErr=True		 		
		end if

		fileExt=lcase(ofile.FileExt)
		arrUpFileType=split("gif|jpg|png","|")
		for i=0 to ubound(arrUpFileType)
			if fileEXT=trim(arrUpFileType(i)) then
				EnableUpload=true
				exit for
			end if
		next
		if fileEXT="asp" or fileEXT="asa" or fileEXT="aspx" then
			EnableUpload=false
		end if
		if EnableUpload=false then
			msg="�����ļ����Ͳ������ϴ���\n\nֻ�����ϴ��⼸���ļ����ͣ�gif|jpg|png"
			FoundErr=true
		end if
				
		strJS="<SCRIPT language=javascript>" & vbcrlf
		if FoundErr<>true then
			randomize
			ranNum=int(900*rnd)+100
			filename=year(now)&month(now)&day(now)&hour(now)&minute(now)&second(now)&ranNum&"."&fileExt

			ofile.SaveToFile Server.mappath(SavePath&FileName)
                        strJS=strJS & "parent.document.form1.pic.value='"&SavePath&""&fileName&"';" & vbcrlf'				
		else
			strJS=strJS & "alert('" & msg & "');" & vbcrlf
		  	strJS=strJS & "history.go(-1);" & vbcrlf
		end if
		strJS=strJS & "</script>" & vbcrlf
		response.write strJS
Response.Write "<script>alert('�ϴ��ɹ����ļ����� "&filename&" ');this.location.href='guke_up1.asp';</SCRIPT>"
response.end
		set file=nothing
	next
	set upload=nothing
end sub
%>