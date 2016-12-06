<?php
import("com.laurus.core.services.IService");
/**
 * Description of IGoogleCaptchaService
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
interface IGoogleCaptchaService extends IService{
      const NAME="GOOGLE_CAPTCHA_SERVICE";
      public function VerifyCaptcha($response);
}

?>
