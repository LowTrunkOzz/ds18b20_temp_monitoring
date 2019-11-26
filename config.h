/*

*/

int serverPort = YOUR_PORT;
const char* USERNAME = "admin";      /*make this a fairly secure name*/
const char* USERPASS = "admin";      /*make this a STRONG password*/

static const char HELLO_PAGE[] PROGMEM = R"(
{ "title": "SmartHomeTech", "uri": "/", "menu": true, "element": [
    { "name": "caption", "type": "ACText", "value": "<h2>Your Smart Home Updater</h2>",  "style": "text-align:center;color:#2f4f4f;padding:10px;" },
    { "name": "content", "type": "ACText", "value": "Select the menu button for options." } ]
}
)";
