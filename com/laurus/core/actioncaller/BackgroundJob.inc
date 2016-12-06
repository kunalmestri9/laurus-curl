<?php
/**
 * Description of BackgroundJob
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
class BackgroundJob {
    
    public function run(){
        // Sudo method , you must need to overide it.
    }
    
    public function start(){
        $this->run();
    }
    
}

?>
