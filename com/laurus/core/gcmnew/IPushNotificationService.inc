<?php
import("com.laurus.core.collection.SimpleList");
import("com.laurus.core.services.IService");
import("com.laurus.core.push.ent.*");
import("com.laurus.core.push.exceptions.*");


/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
interface IPushNotificationService extends IService{
    const NAME="PUSH_SERVICE";
    public function sendNotification(\SimpleList $registrationList, \Notification $message);
    
}


?>
