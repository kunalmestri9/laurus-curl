<?php

import("com.laurus.core.actions.CoreActionHttp");
import("com.laurus.core.paymentgateway.Instamojo");
/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of ActGoogleCaptcha
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
class ActGetPaymentInfo extends CoreActionHttp {
    protected $logger;

    public function __construct() {
        $this->logger = Logger::getLogger(__CLASS__);
    }

    public function executeInvoke(\HttpRequest $wr) {
         $this->logger->debug(__CLASS__ . "" . __METHOD__);
        $this->validateRequestParameters($wr, array(
            "payment_id" => "Payment Id",
            "payment_request_id" => "Payment Request Id"
        ));
        $api = new Instamojo($_SERVER["AK"],$_SERVER['ATO']);
        $this->logger->debug("ekde");
        try {
            $response = $api->paymentRequestStatus($wr->getParameter("payment_request_id"));
            $wr->setParameter("prId",$wr->getParameter("payment_request_id"));
            $wr->setParameter("pyId",$wr->getParameter("payment_id"));
            $wr->setParameter("ts",$response["payments"][0]["status"]);
            $simpleList=new SimpleList($response);
            $wr->removeSessionAttribute("tsDet");
            $wr->setSessionAttribute("tsDet",$response["payments"][0]);
            $wr->setJSONResponseBean($simpleList);
            $this->logger->debug($simpleList);
        } catch (Exception $e) {
            print('Error: ' . $e->getMessage());
        }
    }

}

?>
