<?php
/**
 * Description of PushNotificationException
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */

interface PushNotificationExceptionType{ 
    const PUSH_NOTIFICATION_SENDING_FAILED=90000;
   
}

class PushNotificationException extends RuntimeException {
    public function __construct($message, $code=0, $previous=null) {
        parent::__construct($message, $code, $previous);
    }
}

?>
