<?php
import('com.laurus.core.curl.GenericRequest');
/*
 * @author Kunal Mestri
 */
class PostRequest extends GenericRequest {
    public function getMethodType(){
        return "POST";
    }
    public function __construct($url,$parameters=null,$sessionId=null){
        parent::__construct($url, $parameters,$sessionId);
    }
}

?>