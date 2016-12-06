<?php
$IMPORT_FILE_PATH=$_SERVER["WORKSPACE_PEAR_PATH"]."import.inc";
include_once $IMPORT_FILE_PATH;

import('com.laurus.core.curl.GetRequest');

$request=new GetRequest('http://www.google.com');
$request->execute();
?>
