<?php
import("com.laurus.core.actions.CoreActionHttp");
import("com.laurus.core.googlecaptcha.IGoogleCaptchaService");
import("com.laurus.kayo.auth.exceptions.IAuthExceptionType");

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Description of ActGoogleCaptcha
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
class ActGoogleCaptcha extends CoreActionHttp{
    public function executeInvoke(\HttpRequest $wr) {
        $this->validateRequestParameters($wr, array(
            "recapcharesponse" => "Captcha"
        ));
    try{
        $cap = ServiceManager::getInstance()->getService(IGoogleCaptchaService::NAME);
        $verified = $cap->VerifyCaptcha($wr->getParameter("recapcharesponse"));
    }catch(\Exception $exc){
         throw new ValidationException("Captcha is Incorrect",IAuthExceptionType::CAPTCHA_IS_CORRECT);
    }
}
}

?>
