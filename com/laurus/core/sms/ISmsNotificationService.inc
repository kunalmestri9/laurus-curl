<?php
import("com.laurus.core.collection.SimpleList");
import("com.laurus.core.services.IService");
/**
 * Description of ISmsNotificationService
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
interface ISmsNotificationService extends IService{
    const NAME="SMS_SERVICE";
    public function sendSms(\SimpleList $mobileNoList,$message);
}

?>
