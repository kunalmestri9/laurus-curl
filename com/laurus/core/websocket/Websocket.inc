<?php
import("com.laurus.core.websocket.IWebSocket");
import("com.laurus.core.http.HttpRequest");
import("com.laurus.core.util.*");
import("com.laurus.core.services.BootStrapLoader");
import("com.laurus.core.controller.Controller");
import("com.laurus.core.modelconfig.ModelConfig");
import("com.laurus.core.collection.map.Map");        
/**
 * Description of Websocket
 * @deprecated 
 * Please use WSMainServer instead
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
class Websocket implements IWebSocket{
    private $host;
    private $port;
    private $socket;
    private $clients;
    private $clientsArray;
    private $connectCommand;
    private $disconnectCommand;
    public function __construct($host,$port) {
        $this->host=$host;
        $this->port=$port;
        $this->clientsArray=new Map();
    }
    
    public function decode($text) {
        $length = ord($text[1]) & 127;
	if($length == 126) {
		$masks = substr($text, 4, 4);
		$data = substr($text, 8);
	}
	elseif($length == 127) {
		$masks = substr($text, 10, 4);
		$data = substr($text, 14);
	}
	else {
		$masks = substr($text, 2, 4);
		$data = substr($text, 6);
	}
	$text = "";
	for ($i = 0; $i < strlen($data); ++$i) {
		$text .= $data[$i] ^ $masks[$i%4];
	}
	return $text;
    }

    public function encode($text) {
        $b1 = 0x80 | (0x1 & 0x0f);
	$length = strlen($text);
	if($length <= 125)
		$header = pack('CC', $b1, $length);
	elseif($length > 125 && $length < 65536)
		$header = pack('CCn', $b1, 126, $length);
	elseif($length >= 65536)
		$header = pack('CCNN', $b1, 127, $length);
        
	return $header.$text;
    }

    public function handshake($receved_header,$client_conn, $host, $port) {
        $headers = array();
        $matches=NULL;
	$lines = preg_split("/\r\n/", $receved_header);
	foreach($lines as $line){
		$line = chop($line);
		if(preg_match('/\A(\S+): (.*)\z/', $line, $matches)){
			$headers[$matches[1]] = $matches[2];
		}
	}

	$secKey = $headers['Sec-WebSocket-Key'];
	$secAccept = base64_encode(pack('H*', sha1($secKey . '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')));
	//hand shaking header
	$upgrade  = "HTTP/1.1 101 Web Socket Protocol Handshake\r\n" .
	"Upgrade: websocket\r\n" .
	"Connection: Upgrade\r\n" .
	"WebSocket-Origin: $host\r\n" .
	"WebSocket-Location: ws://$host:$port/demo/shout.php\r\n".
	"Sec-WebSocket-Accept:$secAccept\r\n\r\n";
	socket_write($client_conn,$upgrade,strlen($upgrade));
    }

    public function init() {
        //Create TCP/IP sream socket
        $this->socket = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        //reuseable port
        socket_set_option($this->socket, SOL_SOCKET, SO_REUSEADDR, 1);    
        //bind socket to specified host
        socket_bind($this->socket, 0, $this->port);
        //listen to port
        socket_listen($this->socket);
        //create & add listning socket to the list
        $this->clients = array($this->socket);
    }

    public function listen() {
        //start endless loop, so that our script doesn't stop
        while (true) {
            //manage multipal connections
            $changed = $this->clients;
            $null=NULL;
            //returns the socket resources in $changed array
            socket_select($changed, $null, $null, 0, 10);

            //check for new socket
            if (in_array($this->socket, $changed)) {
                    $socket_new = socket_accept($this->socket); //accpet new socket
                    $this->clients[] = $socket_new; //add socket to client array
                    $header = socket_read($socket_new, 1024); //read data sent by the socket
                    $this->handshake($header, $socket_new, $this->host, $this->port); //perform websocket handshake
                    $ip=null;
                    $port=null;
                    socket_getpeername($socket_new, $ip,$port); //get ip address of connected socket
                    $uniqueId=base64_encode($ip."|".$port);
                    $this->clientsArray->put($uniqueId,$socket_new);
                    $connectCommand=$this->getConnectCommand();
                    if($connectCommand!=null){
                        $_REQUEST["command"]=$connectCommand;
                        $_REQUEST["uniqueId"]=$uniqueId;
                        $this->executeController();
                    }
                    $response = $this->encode(json_encode(array('type'=>'system', 'action'=>'connected' ,'uniqueId'=>$uniqueId))); //prepare json data
                    //$response = $this->encode(json_encode(array('type'=>'system', 'message'=>$uniqueId .' connected'))); //prepare json data
                    //echo $response;
                    $this->sendMessage($response,$uniqueId); //notify all users about new connection
                    //make room for new socket
                    $found_socket = array_search($this->socket, $changed);
                    unset($changed[$found_socket]);
            }

            //loop through all connected sockets
            foreach ($changed as $changed_socket) {	
                    //check for any incomming data
                    while(socket_recv($changed_socket, $buf, 40000, 0) >= 1){
                        $received_text = $this->decode($buf); //unmask data
                        $tst_msg = json_decode($received_text); //json decode 
                        session_start();
                        $wr=new HttpRequest();
                        foreach ($tst_msg as $key => $value) {
                            $wr->setParameter($key,$value);
                        }
                        $this->executeController();
                        //prepare data to be sent to client
                        $jsonRenderer=new JsonObjectWriter();
                        $vJson=$jsonRenderer->renderJson();
                        $response_text = $this->encode($vJson);
                        if(isset($_REQUEST["FOR_SOCK"])){
                            $this->sendMessage($vJson,$_REQUEST["FOR_SOCK"]);
                        }else
                            $this->sendMessage($response_text);
                        $response_text=null;
                        session_destroy();
                        unset($_REQUEST["command"]);
                        $buf=null;
                        //send data
                        break 2; //exist this loop
                    }

                    $buf = @socket_read($changed_socket, 1024, PHP_NORMAL_READ);
                    if ($buf === false) { // check disconnected client
                        // remove client for $clients array
                        $found_socket = array_search($changed_socket, $this->clients);
                        socket_getpeername($changed_socket,$ip,$port);
                        $uniqueId=base64_encode($ip."|".$port);
                        $disconnectCommand=$this->getDisconnectCommand();
                        if($connectCommand!=null){
                            $_REQUEST["command"]=$disconnectCommand;
                            $_REQUEST["uniqueId"]=$uniqueId;
                            $this->executeController();
                        }
                        unset($this->clients[$found_socket]);
                        //notify all users about disconnected connection
                        $response = $this->encode(json_encode(array('type'=>'system','action'=>'disconnected', 'uniqueId'=>$uniqueId)));
                        $this->sendMessage($response);
                    }
            }
        }
        // close the listening socket
        socket_close($this->socket);
    }

    public function start() {
        set_time_limit(0);
        $this->init();
        $this->listen();
    }  
    public function sendMessage($msg,$for=null){
        if($for!=null){
            if($this->clientsArray->contains($for)){
                $changed_socket1=$this->clientsArray->get($for)->getValue();
                echo $changed_socket1;
                @socket_write($changed_socket1,$msg,strlen($msg));
            }else{
                echo "No such socket present";
            }
            return;
        }
        foreach($this->clients as $changed_socket){
            @socket_write($changed_socket,$msg,strlen($msg));
        }
        return true;
    }
    
    public function executeController(){
        $bootStrap= new	BootStrapLoader();
        $bootStrap->init();
        $modelConfig=ModelConfig::getInstance();
        $controller=Controller::getInstance();
        $controller->setCommandConfig($modelConfig);
        $controller->execute();
    }

    public function getConnectCommand() {
        return $this->connectCommand;
    }

    public function setConnectCommand($connectCommand) {
        $this->connectCommand = $connectCommand;
    }

    public function getDisconnectCommand() {
        return $this->disconnectCommand;
    }

    public function setDisconnectCommand($disconnectCommand) {
        $this->disconnectCommand = $disconnectCommand;
    }


}

?>