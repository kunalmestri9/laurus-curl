<?php
import('com.laurus.core.curl.GenericRequest');
/*
 * @author Kunal Mestri
 */
class GetRequest extends GenericRequest {
    public function getMethodType(){
        return "GET";
    }
    public function __construct($url,$parameters=null,$sessionId=null){
        parent::__construct($url, $parameters,$sessionId);
    }

}

?>