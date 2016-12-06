<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of Notification
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
class Notification {
    private $messageArray;
    private $targetData;
    private $targetDataType;
    private $title;
    private $message;
    private $msgCount;
    private $soundName;
    
    
    public function __construct() {
        $this->messageArray=array();
    }
    
    public function getMessage(){
        $this->messageArray["message"]=substr($this->message,0,100);
        $this->messageArray["title"]=substr($this->title,0,50);
        $this->messageArray["msgcnt"]=$this->msgCount;
        $this->messageArray["notId"]=rand(1, 20000);
        if($this->targetDataType){
            $this->messageArray["targetData"]=  $this->targetData;
            $this->messageArray["targetDataType"]= $this->targetDataType;
        }
        $this->messageArray["soundname"]=  $this->soundName;
        
        return $this->messageArray;
    }
    
    public function getMessageArray() {
        return $this->messageArray;
    }

    public function setMessageArray($messageArray) {
        $this->messageArray = $messageArray;
    }

    public function getTargetData() {
        return $this->targetData;
    }

    public function setTargetData($targetData) {
        $this->targetData = $targetData;
    }

    public function getTargetDataType() {
        return $this->targetDataType;
    }

    public function setTargetDataType($targetDataType) {
        $this->targetDataType = $targetDataType;
    }

    public function getTitle() {
        return $this->title;
    }

    public function setTitle($title) {
        $this->title = $title;
    }

    public function getMsgCount() {
        return $this->msgCount;
    }

    public function setMsgCount($msgCount) {
        $this->msgCount = $msgCount;
    }

    public function getSoundName() {
        return $this->soundName;
    }

    public function setSoundName($soundName) {
        $this->soundName = $soundName;
    }

    public function setMessage($message) {
        $this->message = $message;
    }
  
    public function toJson() {
        $map = new Map();
        $map->put("messageArray", $this->getMessageArray());
        $map->put("targetData", $this->getTargetData());
        $map->put("targetDataType", $this->getTargetDataType());
        $map->put("message", $this->getMessage());
        $map->put("msgCount", $this->getMsgCount());
        $map->put("soundName", $this->getSoundName());
        return $map->toJson();
    }
            
}

?>
