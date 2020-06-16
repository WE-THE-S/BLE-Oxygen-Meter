#ifndef __OTA_HPP__
#define __OTA_HPP__

#include <esp_attr.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "./config.hpp"

 
/* Server Index Page */
DRAM_ATTR const char* serverIndex = "
<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>\
<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>\
<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>\
<label id='file-input' for='file'>   Choose file...</label>\
<input type='submit' class=btn value='Update'>\
<br><br>\
<div id='prg'></div>\
<br><div id='prgbar'><div id='bar'></div></div><br></form>\
<script>\
function sub(obj){\
var fileName = obj.value.split('\\\\');\
document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];\
};\
$('form').submit(function(e){\
e.preventDefault();\
var form = $('#upload_form')[0];\
var data = new FormData(form);\
$.ajax({\
url: '/update',\
type: 'POST',\
data: data,\
contentType: false,\
processData:false,\
xhr: function() {\
var xhr = new window.XMLHttpRequest();\
xhr.upload.addEventListener('progress', function(evt) {\
if (evt.lengthComputable) {\
var per = evt.loaded / evt.total;\
$('#prg').html('progress: ' + Math.round(per*100) + '%');\
$('#bar').css('width',Math.round(per*100) + '%');\
}\
}, false);\
return xhr;\
},\
success:function(d, s) {\
console.log('success!') \
},\
error: function (a, b, c) {\
}\
});\
});\
</script> \
<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px} \
input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777} \
#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer} \
#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px} \
form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center} \
.btn{background:#3498db;color:#fff;cursor:pointer}</style>";

class OTA {
    private:
        const char* ssid = "THES3";
        const char* password = "1234123412";
        OTA() {
            WiFi.begin(this->ssid, this->password);
            while (WiFi.status() != WL_CONNECTED) {
                delay(500);
            }
        };
        WebServer server(80);
    public:
        OTA* getInstance() const {
            static OTA instance;
            return &instance;
        }
        void start(){
            MDNS.begin(host);
            ESP_LOGI(typename(this),"mDNS responder started");
            /*return index page which is stored in serverIndex */
            server.on("/", HTTP_GET, []() {
                server.sendHeader("Connection", "close");
                server.send(200, "text/html", serverIndex);
            });
            /*handling uploading firmware file */
            server.on("/update", HTTP_POST, []() {
                server.sendHeader("Connection", "close");
                server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
                ESP.restart();
            }, []() {
                HTTPUpload& upload = server.upload();
                if (upload.status == UPLOAD_FILE_START) {
                ESP_LOGA(typename(this), "Update: %s\n", upload.filename.c_str());
                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
                    Update.printError(Serial);
                }
                } else if (upload.status == UPLOAD_FILE_WRITE) {
                /* flashing firmware to ESP*/
                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
                    Update.printError(Serial);
                }
                } else if (upload.status == UPLOAD_FILE_END) {
                if (Update.end(true)) { //true to set the size to the current progress
                    ESP_LOGA(typename(this), "Update Success: %u\nRebooting...\n", upload.totalSize);
                } else {
                    Update.printError(Serial);
                }
                }
            });
            server.begin();
            while(1){
                server.handleClient();
                delay(1);
            }
        }
    
};
#endif