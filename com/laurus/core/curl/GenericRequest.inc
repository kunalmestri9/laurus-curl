<?php
import('com.laurus.core.curl.IRequest');
/*
 * @author Kunal Mestri
 */
abstract class GenericRequest implements IRequest {
    private $url;
    private $port;
    private $curlObj;
    private $connectionTimeout;
    private $parameterArray;
    private $response;
    private $info;
    private $sessionId;
    private $body;
    private $headers;
    private $requestHeaders;
    
    public function __construct($url,$parameters=null,$sessionId=null){
        $this->logger=Logger::getLogger(__CLASS__);
        $this->url=$url;
        if($parameters!=null){
            if(!is_array($parameters)){
                throw new Exception("Second Parameter must be Parameters Array");
            }
            $this->parameterArray=$parameters;
        }
        if($sessionId!=null)
            $this->sessionId=$sessionId;
        
        $this->requestHeaders=array();
    }

    public function setURL( $url) {
        $this->url=$url;
    }
    public function setPort($port) {
        $this->port=$port;
    }
    public function execute() {
        try {
            $this->curlObj=curl_init();
            $this->create();
            // $resp=curl_exec($this->curlObj);
            //var_dump($resp);
            //$response= curl_getinfo( $this->curlObj );
            //var_dump($response);
            $responseAndInfo=$this->executeAndHandleResponse($this->curlObj);
            $this->handleError();
            $this->response=$responseAndInfo[0];
            $this->info=$responseAndInfo[1];
        }catch(exception $e) {
            throw new Exception($e->getMessage(). " : hence Request Could not be created ");
        }
    }
    public function getResponseText(){
        return $this->body;
    }
    
    private function executeAndHandleResponse($curl){
        $resp=curl_exec($curl);
        $response= curl_getinfo($curl) ;
        
        if ($response['http_code'] == 301 || $response['http_code'] == 302){
            if ( $headers = get_headers($response['url']) ){
                foreach( $headers as $value ){
                    if ( substr( strtolower($value), 0, 9 ) == "location:" ){
                        curl_setopt($curl,CURLOPT_URL, $value);
                         return $this->executeAndHandleResponse($curl);
                    }
                   
                }
            }
         }
        // Then, after your curl_exec call:
        $header_size = curl_getinfo($curl, CURLINFO_HEADER_SIZE);
        $this->logger->debug($header_size);
        $this->logger->debug($response);
        
        $this->setResponseHeaders(substr($resp, 0, $header_size));
        $this->body = substr($resp, $header_size);
        $this->logger->debug($this->body);
        return array($resp,$response);
    }
    
    public function setResponseHeaders($headersText){
        $headersArray=preg_split('/\n|\r\n?/', $headersText);//explode(PHP_EOL, $headersText);
        $this->logger->debug($headersArray);
        $this->headers=array();
        foreach ($headersArray as $value) {
            $rr=explode(":", $value);
            if(count($rr)>1){
                $this->headers[$rr[0]]=$rr[1];
            }
        }
        $this->logger->debug($this->headers);
    }
    
    public function getBody() {
        return $this->body;
    }

    public function setBody($body) {
        $this->body = $body;
    }

    public function getHeaders() {
        return $this->headers;
    }
    
    
    public function getHeaderValue($key){
        return isset($this->headers[$key])? $this->headers[$key] : false;
    }
    
    public function setHeaders($headers) {
        $this->headers = $headers;
    }

        
    protected function handleError() {
        if(!curl_errno($this->curlObj)) {
            $info = curl_getinfo($this->curlObj);
            // 'Took ' . $info['total_time'] . ' seconds to send a request to ' . $info['url'];
        }else {
           // echo curl_errno($this->curlObj);
            throw new Exception('Curl error: ' . curl_error($this->curlObj));
        }

    }
    protected function create() {
        if($this->url==null) {
            throw new Exception("Malformed URL exception: URL should not be null");
        }
        if($this->getMethodType()=="GET"){
            $serializedArray=$this->getSerializedParameters($this->parameterArray);
            if($serializedArray!=null){
                $this->url=$this->url."?".$serializedArray;
            }
        }
        $this->setExtraOptions(CURLOPT_URL, $this->url);
        $this->setBasicOptions();
        if($this->port!=null)
            $this->setExtraOptions(CURLOPT_PORT,$this->port);
        if($this->sessionId!=null){
           $this->setExtraOptions (CURLOPT_COOKIE, $this->sessionId);
        }    
        if(count($this->requestHeaders>0))
            $this->setExtraOptions(CURLOPT_HTTPHEADER, $this->requestHeaders);
    }
    protected function setBasicOptions() {
        if(isset($_SERVER['CURL_DEFAULT_USERAGENT'])){
            $default_user_agent=$_SERVER['CURL_DEFAULT_USERAGENT'];
        }else
            $default_user_agent="Mozilla/5.0 (Windows; U; Windows NT 5.1; rv:1.7.3) Gecko/20041001 Firefox/0.10.1" ;
        $this->setExtraOptions(CURLOPT_USERAGENT,$default_user_agent );
        //$this->setMethodType($this->getMethodType());
        if($this->getMethodType()=="POST"){
            $serializedArray=$this->getSerializedParameters($this->parameterArray);
            if($serializedArray!=null){
                $this->setExtraOptions(CURLOPT_POSTFIELDS, $serializedArray);
            }
        }
        if($this->connectionTimeout == null)
            $this->setExtraOptions(CURLOPT_CONNECTTIMEOUT_MS,5000);
        else
            $this->setExtraOptions(CURLOPT_CONNECTTIMEOUT_MS,$this->connectionTimeout);

        $this->setExtraOptions(CURLOPT_RETURNTRANSFER, true );
        $this->setExtraOptions(CURLOPT_AUTOREFERER, true );
        $this->setExtraOptions(CURLOPT_SSL_VERIFYPEER,false);
        $this->setExtraOptions(CURLOPT_SSL_VERIFYHOST,false);
        $this->setExtraOptions(CURLOPT_IPRESOLVE,CURL_IPRESOLVE_V4);
        $this->setExtraOptions(CURLOPT_HEADER,true);
        $this->setExtraOptions(CURLOPT_TIMEOUT, 20);
        $this->setExtraOptions(CURLOPT_MAXREDIRS, 10 );
    }
    public function setExtraOptions($key,$val) {
        curl_setopt($this->curlObj, $key, $val);
    }
    
    public function addRequestHeader($key,$val){
        $this->requestHeaders[]= $key . " : " . $val;
    }
    public function setConnectionTime($ms) {
        $this->connectionTimeout=$ms;
    }
    public function setMethodType($methodName){
        if($methodName=="POST"){
            $this->setExtraOptions(CURLOPT_POST,TRUE);
        }else if($methodName=="GET"){
            $this->setExtraOptions(CURLOPT_HTTPGET,TRUE);
        }
    }
    abstract public function getMethodType();

    function getSerializedParameters($puraArray,$superKey=null){
        if($puraArray==null){
            return null;
        }
        $fields_string="";
        if($superKey==null){
            foreach($puraArray as $key=>$value) {
                if(is_array($value)){
                    $fields_string .= $this->getSerializedParameters($value,$key);
                }else{
                    $fields_string .= $key.'='.$value.'&';
                }
            }
        }else{
            foreach($puraArray as $value) {
                    $fields_string .= $superKey.'='.$value.'&';
            }
            $superKey=null;
        }

	return rtrim($fields_string, '&');
    }
}

?>
