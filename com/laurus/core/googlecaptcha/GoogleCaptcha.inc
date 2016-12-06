<?php
import("com.laurus.core.googlecaptcha.IGoogleCaptchaService");
class GoogleCaptcha implements IGoogleCaptchaService{
    protected $logger;
    private $seckKey;
     private static $instance;
    public function getSeckKey() {
        return $this->seckKey;
    }
     public static function getInstance() {
        if (self::$instance == null) {
            self::$instance = new GoogleCaptcha();
        }
        return self::$instance;
    }
    public function setSeckKey($seckKey) {
        $this->seckKey = $seckKey;
    }
    public function __construct() {
        $this->logger = Logger::getLogger(__CLASS__);
    }
     public function init(\IServiceConfig $config) {
        $this->setSeckKey($config->getConfigParameter("SEC_KEY"));
       
    }

    /* Google recaptcha API url */

    private $google_url = "https://www.google.com/recaptcha/api/siteverify";
   
    public function VerifyCaptcha($response) {
        try {
            $this->logger->debug($this->getSeckKey());
            $this->logger->debug($this->google_url);
            $this->logger->debug($response);
            $url = $this->google_url . "?secret=" . $this->getSeckKey() .
                    "&response=" . $response;

            $curl = curl_init();
            curl_setopt($curl, CURLOPT_URL, $url);
            curl_setopt($curl, CURLOPT_RETURNTRANSFER, TRUE);
            curl_setopt($curl, CURLOPT_TIMEOUT, 15);
            curl_setopt($curl, CURLOPT_SSL_VERIFYPEER, FALSE);
            curl_setopt($curl, CURLOPT_SSL_VERIFYHOST, FALSE);
            $curlData = curl_exec($curl);
            $this->logger->debug($curlData);
            $this->logger->debug(curl_getinfo($curl));
            $this->logger->debug(curl_error($curl));
            curl_close($curl);
           
            $res = json_decode($curlData, TRUE);
            $this->logger->debug($res);
            if ($res['success'] == 'true') {
                $this->logger->debug("success");
                $this->logger->debug($res['success']);
                return TRUE;
            } else {
                $this->logger->debug("error");
                $this->logger->debug($res['success']);
                throw new Exception("Captcha is Incorrect");
            }
        } catch (exception $e) {
             $this->logger->debug($e);
            throw new Exception("Captcha is Incorrect");
        }
    }

}

