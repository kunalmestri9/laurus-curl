<?php
import("com.laurus.core.sms.ISmsNotificationService");
/**
 * Description of MSmsNotificationService
 *
 * @author Amruta Vichare <amruta.vichare@laurus-it.com>
 */
class MSmsNotificationService implements ISmsNotificationService {
    private $logger;
    private $userName;
    private $hash;
    private $smsUrl;
    private static $instance;
    public function getSmsUrl() {
        return $this->smsUrl;
    }

    public function setSmsUrl($smsUrl) {
        $this->smsUrl = $smsUrl;
    }

    public function getUserName() {
        return $this->userName;
    }

    public function setUserName($userName) {
        $this->userName = $userName;
    }

    public function getHash() {
        return $this->hash;
    }

    public function setHash($hash) {
        $this->hash = $hash;
    }
     public function __construct() {
        $this->logger = Logger::getLogger(__CLASS__);
    }
    public function init(\IServiceConfig $config) {
        $this->setUserName($config->getConfigParameter("USER_NAME"));
        $this->setHash($config->getConfigParameter("HASH"));
        $this->setSmsUrl($config->getConfigParameter("SMS_URL"));
     
    }

    public function sendSms(\SimpleList $mobileNoList, $message) {
        // Textlocal account details
	$username = $this->getUserName();
	$hash = $this->getHash();
	
	// Message details
	$numbers = $mobileNoList->toArray();
	$sender = urlencode('TXTLCL');
	$message = rawurlencode($message);
 
	$numbers = implode(',', $numbers);
 
	// Prepare data for POST request
	$data = array('username' => $username, 'hash' => $hash, 'numbers' => $numbers, "sender" => $sender, "message" => $message);
 
	// Send the POST request with cURL
	$ch = curl_init($this->getSmsUrl());
	curl_setopt($ch, CURLOPT_POST, true);
	curl_setopt($ch, CURLOPT_POSTFIELDS, $data);
	curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
	$response = curl_exec($ch);
	curl_close($ch);
	$this->logger->debug($response);
	// Process your response here
	//echo $response;
    }

    public static function getInstance() {
        if (self::$instance == null) {
            self::$instance = new MSmsNotificationService();
        }
        return self::$instance;
    }    
}

?>
