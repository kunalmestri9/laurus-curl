<?php
/**
 * Description of MActionHttpCaller
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
import('com.laurus.core.curl.GetRequest');
import('com.laurus.core.curl.PostRequest');
import('com.laurus.core.actioncaller.IActionHttpCaller');
import('com.laurus.core.actioncaller.BackgroundJob');
import('com.laurus.core.actioncaller.IBackgroundJob');
import('com.laurus.core.collection.SimpleList');

class MActionHttpCaller extends BackgroundJob implements IActionHttpCaller,IBackgroundJob{
    /**
     *
     * @var SimpleList 
     */
    protected $urlList;
    protected $interval;
    protected $logger;
    public function getInterval() {
        return $this->interval;
    }

    public function setInterval($interval) {
        $this->interval = $interval;
    }

    public function __construct($simpleList){
        $this->urlList=$simpleList;
        $this->interval=10;
        $this->logger=Logger::getLogger(__CLASS__);
    }
    public function callCommand($url) {
        $request=new GetRequest();
        $request->setURL($url);
        $request->execute();
        //$request->getResponseText();
        echo $url;
    }
 
    public function run() {
        for ($index = 0; $index < $this->urlList->size(); $index++) {
            $this->callCommand($this->urlList->get($index));
            echo "start sleep";
            sleep($this->getInterval()); //Interval should be mentain for 2 mins
            echo "end sleep";
        }
    }    
}

?>
