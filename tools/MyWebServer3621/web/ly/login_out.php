<?php
/*���session*/
session_start();
session_destroy();
header("location:admin_index.php");
//����admin_index.php���ж�session�Ƿ�����ɹ����ɹ��򷵻�index.php
?>