<?php
import("com.laurus.core.gcm.IGoogleNotificationService");
$path=$_SERVER["WORKSPACE_PATH"].DIRECTORY_SEPARATOR.$_SERVER["THIRDPARTIES_DIR"];
set_include_path($path );
importLib("Google", "Client.php");
class GoogleNotificationService implements IGoogleNotificationService {
    private $logger;
    private $url;
    private $googleApiKey;
    private static $instance;
    /**
     *
     * @var Google_Client 
     */
    private $client;
    
    public function __construct() {
        $this->logger=  Logger::getLogger(__CLASS__);
    }
    
        
    private function configureGoogleClient(IServiceConfig $config){
        $this->logger->debug("Starting configuring google clinet");
        $this->client = new Google_Client();
        $this->client->setClientId('1001812061127-uv3crap644q65ub2s8o4r50d0l474lto.apps.googleusercontent.com');
        $this->client->setClientSecret('dN-ll4KESuZ3eaGpw5e1TLkR');
        $this->client->setRedirectUri('http://copyme.laurus-host.in/app/copyme-api/callback');
        $this->client->setDeveloperKey('AIzaSyBHexW02z5nvC7i_h5WHsvlr1UOnxWaS0M');
        $this->client->setScopes(array(
            "https://www.googleapis.com/auth/gcm_for_chrome"
        ));
        $this->logger->debug("Completed");
    }
    
    public function authenticateForToken(){
        if($this->client->getAccessToken()){
            return $this->client->getAccessToken();
        }else{
            return $this->client->createAuthUrl();
        }
    }
    
    public function getUrl() {
        return $this->url;
    }
    
    public function setUrl($url) {
        $this->url = $url;
    }

    public function getGoogleApiKey() {
        return $this->googleApiKey;
    }

    public function setGoogleApiKey($googleApiKey) {
        $this->googleApiKey = $googleApiKey;
    }
       
    public function sendMessageToChrome($id,$message,$authToken){
        $url ="https://www.googleapis.com/gcm_for_chrome/v1/messages";
        $data = json_encode(array(
            'channelId' => $id,
            'subchannelId' => "1",
            'payload'=> $message
        ));
        $ch = curl_init();
        $curlConfig = array(
            CURLOPT_URL            => $url,
            CURLOPT_POST           => true,
            CURLOPT_RETURNTRANSFER => true,
            CURLOPT_POSTFIELDS     => $data,
            CURLOPT_SSL_VERIFYPEER => false,
            CURLOPT_SSL_VERIFYHOST => false,
            CURLOPT_MAXREDIRS      => 20,
            CURLOPT_TIMEOUT        => 1,
            CURLOPT_IPRESOLVE      => CURL_IPRESOLVE_V4,
            CURLOPT_HTTPHEADER     => array(
                'Authorization: Bearer ' . $authToken,
                'Content-Type: application/json'
            )
        );
        curl_setopt_array($ch, $curlConfig);
        $result = curl_exec($ch);
        $this->logger->debug("Reqest Execution completed");
        $this->logger->debug(curl_getinfo($ch));
        $this->logger->debug(curl_error($ch));
        
        if(strstr($result,"error")){
            throw new Exception('Curl failed: ' . curl_error($ch));
        }
        curl_close($ch);
        
    }
    
    public function sendNotification(\SimpleList $registrationList, $message) {
        $registrationArray=$registrationList->toArray();
        $this->logger->debug($registrationArray);
        $fields = array(
            'registration_ids' => $registrationArray ,
            'data' => $message
        );
        $headers = array(
            'Authorization: key=' . $this->getGoogleApiKey(),
            'Content-Type: application/json'
        );
        // Open connection
        $ch = curl_init();
 
        // Set the url, number of POST vars, POST data
        curl_setopt($ch, CURLOPT_URL, $this->url);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        // Disabling SSL Certificate support temporarly
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($fields));
        // Execute post
        $result = curl_exec($ch);
        if ($result === FALSE) {
            throw new Exception('Curl failed: ' . curl_error($ch));
        }
        // Close connection
        curl_close($ch);
        $this->logger->debug($result);
    }

    public function init(\IServiceConfig $config) {
        $this->setUrl($config->getConfigParameter("GOOGLE_GCM_URL"));
        $this->setGoogleApiKey($config->getConfigParameter("GOOGLE_API_KEY"));
        $this->configureGoogleClient($config);
    }

     /**
     * Giving Singleton behaviour
     * @return <IGoogleNotificationService>
     */
    public static function getInstance() {
        if (self::$instance == null) {
            self::$instance = new GoogleNotificationService();
        }
        return self::$instance;
    }

    public function getAccessToken($code) {
        if ($code) {
            $this->client->authenticate($code);
            $_SESSION['access_token'] = $this->client->getAccessToken();
        }
        return $_SESSION['access_token'];
    }
   
}
?>