<?php
import("com.laurus.core.collection.SimpleList");
import("com.laurus.core.services.IService");
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
interface IGoogleNotificationService extends IService{
    const NAME="GCM_SERVICE";
    public function sendNotification(\SimpleList $registrationList,$message);
    public function sendMessageToChrome($channelId,$message,$authToken);
    public function authenticateForToken();
    public function getAccessToken($code);
}

?>
