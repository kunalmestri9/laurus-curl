<?php
import("com.laurus.core.push.IPushNotificationService");

class MPushNotificationService implements IPushNotificationService {
    private $logger;
    private $googleNotificationurl;
    private $googleApiKey;
    private static $instance;
    
    public function getGoogleNotificationurl() {
        return $this->googleNotificationurl;
    }

    public function setGoogleNotificationurl($googleNotificationurl) {
        $this->googleNotificationurl = $googleNotificationurl;
    }

        
    public function __construct() {
        $this->logger=  Logger::getLogger(__CLASS__);
    }
    

    public function getGoogleApiKey() {
        return $this->googleApiKey;
    }

    public function setGoogleApiKey($googleApiKey) {
        $this->googleApiKey = $googleApiKey;
    }
    public function sendNotification(\SimpleList $registrationList,  \Notification $message){
        $this->sendAndroidNotification($registrationList, $message);
    }
    
    public function sendAndroidNotification(\SimpleList $registrationList,  \Notification $message) {
        $registrationArray=$registrationList->toArray();
        $this->logger->debug($registrationArray);
        $fields = array(
            'registration_ids' => $registrationArray ,
            'data' => $message->getMessage()
        );
        $headers = array(
            'Authorization: key=' . $this->getGoogleApiKey(),
            'Content-Type: application/json'
        );
        
        // Open connection
        $ch = curl_init();
 
        // Set the url, number of POST vars, POST data
        curl_setopt($ch, CURLOPT_URL, $this->googleNotificationurl);
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_HTTPHEADER, $headers);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        // Disabling SSL Certificate support temporarly
        curl_setopt($ch, CURLOPT_SSL_VERIFYPEER, false);
        curl_setopt($ch, CURLOPT_POSTFIELDS, json_encode($fields));
        // Execute post
        $result = curl_exec($ch);
        if ($result === FALSE) {
            throw new PushNotificationException("Notification can not be sent",PushNotificationExceptionType::PUSH_NOTIFICATION_SENDING_FAILED);
        }
        // Close connection
        curl_close($ch);
        $this->logger->debug($result);
    }

    public function init(\IServiceConfig $config) {
        $this->setGoogleNotificationurl($config->getConfigParameter("GOOGLE_GCM_URL"));
        $this->setGoogleApiKey($config->getConfigParameter("GOOGLE_API_KEY"));
    }

     /**
     * Giving Singleton behaviour
     * @return <IPushNotificationService>
     */
    public static function getInstance() {
        if (self::$instance == null) {
            self::$instance = new MPushNotificationService();
        }
        return self::$instance;
    }

    
   
}
?>