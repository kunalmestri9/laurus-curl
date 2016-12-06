<?php
/**
 * Description of MActionHttpCallerExtended
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
import('com.laurus.core.actioncaller.MActionHttpCaller');

class MActionHttpCallerExtended extends MActionHttpCaller{
    
    
    public function callCommandRequest(IRequest $request) {
        try{
            $this->logger->debug("executing request");
            $request->execute();
        }catch (Exception $e){
            $this->logger->debug($e);
        }
    }
    public function run() {
        for ($index = 0; $index < $this->urlList->size(); $index++) {
            $this->callCommandRequest($this->urlList->get($index));
            $this->logger->debug("start sleep");
            sleep($this->getInterval()); //Interval should be mentain for 2 mins
            $this->logger->debug("waking up");
        }
    }    
}

?>
