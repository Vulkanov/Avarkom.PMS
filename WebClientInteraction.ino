/*
 * В ЭТОМ ФАЙЛЕ НАХОДЯТСЯ ФУНКЦИИ, РЕАЛИЗУЮЩИЕ
 * РАБОТУ WEB ИНТЕРФЕЙСА
 */
 
void processWebClient(){
  EthernetClient client = webServer.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();        
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println();
          
          client.println("<cke:html><cke:body bgcolor=#FFFFFF>AVARKOM PMS WEB INTERFACE</cke:body></cke:html></br>"); 
          client.println(CONTROL_TYPE);
          client.println(SOURCE_STATE);       

          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}

