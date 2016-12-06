<?php
/**
 *
 * @author Kunal Mestri <kunal@laurus-it.com>
 */
interface IWebSocket {
    public function init();
    public function listen();
    public function handshake($receved_header,$client_conn, $host, $port);
    public function encode($text);
    public function decode($text);
    public function start();
}
?>