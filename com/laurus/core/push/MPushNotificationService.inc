<?php

import("com.laurus.core.push.IPushNotificationService");

class MPushNotificationService implements IPushNotificationService {

    private $logger;
    private $googleNotificationurl;
    private $googleApiKey;
    private $spocoCertificated;
    private $passphrase;
    private $appleNotificationUrl;
    private static $instance;
    public function getAppleNotificationUrl() {
        return $this->appleNotificationUrl;
    }

    public function setAppleNotificationUrl($appleNotificationUrl) {
        $this->appleNotificationUrl = $appleNotificationUrl;
    }
    public function getPassphrase() {
        return $this->passphrase;
    }

    public function setPassphrase($passphrase) {
        $this->passphrase = $passphrase;
    }
   
    public function getSpocoCertificated() {
        return $this->spocoCertificated;
    }

    public function setSpocoCertificated($spocoCertificated) {
        $this->spocoCertificated = $spocoCertificated;
    }
    public function getGoogleNotificationurl() {
        return $this->googleNotificationurl;
    }

    public function setGoogleNotificationurl($googleNotificationurl) {
        $this->googleNotificationurl = $googleNotificationurl;
    }

    public function __construct() {
        $this->logger = Logger::getLogger(__CLASS__);
    }

    public function getGoogleApiKey() {
        return $this->googleApiKey;
    }

    public function setGoogleApiKey($googleApiKey) {
        $this->googleApiKey = $googleApiKey;
    }

    public function sendNotification(\SimpleList $registrationList, \Notification $message, $notificationType) {
        $this->logger->debug("Send Notification");
        $this->logger->debug($notificationType);

        if (IPushNotificationType::GOOGLE_PUSH_NOTIFICATION == $notificationType) {
            $this->sendAndroidNotification($registrationList, $message);
        } else if (IPushNotificationType::APPLE_PUSH_NOTIFICATION == $notificationType) {
            $this->sendIosNotification($registrationList, $message);
        }
    }

    public function sendIosNotification(\SimpleList $deviceTokenList, \Notification $notmessage) {
        $this->logger->debug($deviceTokenList);

        $passphrase =$this->getPassphrase();
       // $notmessage=$this->tr_to_utf($notmessage);
        $payload = '{"aps":{"alert":"' . $notmessage->getMessage() . '","payload":"'. json_encode($notmessage).'","sound":"default"}}';
        $payload=  json_encode($payload);
        $ctx = stream_context_create();
        $this->logger->debug('conex is online.'.$ctx);
        stream_context_set_option($ctx, 'ssl', 'local_cert', base64_decode($this->getSpocoCertificated()));
        stream_context_set_option($ctx, 'ssl', 'passphrase', $passphrase);
        for ($i=0;$i<$deviceTokenList->size();$i++) {
            $this->logger->debug('In loo');
            // wait for some time
            sleep(1);

            // Open a connection to the APNS server
            $fp = stream_socket_client($this->getAppleNotificationUrl(), $err, $errstr, 60, STREAM_CLIENT_CONNECT | STREAM_CLIENT_PERSISTENT, $ctx);

            if (!$fp) {
                $this->logger->debug('Apple service is online.'.$errstr);
                throw new Exception("Failed to connect:".$err." ".$errstr);
            } else {
                $this->logger->debug('Apple service is online.');
            }

            // Build the binary notification
            $msg = chr(0) . pack('n', 32) . pack('H*', $deviceTokenList->get($i)) . pack('n', strlen($payload)) . $payload;

            // Send it to the server
            $result = fwrite($fp, $msg, strlen($msg));

            if (!$result) {
                $this->logger->debug('Undelivered message count: ' . $deviceTokenList->get($i)); 
            } else {
                $this->logger->debug('Delivered message count: ' . $deviceTokenList->get($i));
            }

            if ($fp) {
                fclose($fp);
                $this->logger->debug('The connection has been closed by the client');
            }
        }
    }

    public function tr_to_utf($text) {
        $text = trim($text);
        $search = array('Ü', 'Þ', 'Ð', 'Ç', 'Ý', 'Ö', 'ü', 'þ', 'ð', 'ç', 'ý', 'ö');
        $replace = array('Ãœ', 'Åž', '&#286;ž', 'Ã‡', 'Ä°', 'Ã–', 'Ã¼', 'ÅŸ', 'ÄŸ', 'Ã§', 'Ä±', 'Ã¶');
        $new_text = str_replace($search, $replace, $text);
        return $new_text;
    }

    public function sendAndroidNotification(\SimpleList $registrationList, \Notification $message) {
        $registrationArray = $registrationList->toArray();
        $this->logger->debug($registrationArray);
        $fields = array(
            'registration_ids' => $registrationArray,
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
        $this->logger->debug(curl_getinfo($ch));
        if ($result === FALSE) {
            throw new PushNotificationException("Notification can not be sent", PushNotificationExceptionType::PUSH_NOTIFICATION_SENDING_FAILED);
        }
        // Close connection
        curl_close($ch);
        $this->logger->debug($result);
    }

    public function init(\IServiceConfig $config) {
        $this->setGoogleNotificationurl($config->getConfigParameter("GOOGLE_GCM_URL"));
        $this->setGoogleApiKey($config->getConfigParameter("GOOGLE_API_KEY"));
        $this->setSpocoCertificated($config->getConfigParameter("SPOCO_CERTIFICATED"));
        $this->setPassphrase($config->getConfigParameter("PASS_PHRASE"));
        $this->setAppleNotificationUrl($config->getConfigParameter("APPLE_PUSH_URL"));
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