#NMD Glove

Het initiële idee van dit project was het maken van een eigen input device, dat een meerwaarde zou kunnen bieden tijdens live performances. Het moet gebruikt kunnen worden naast andere input apparaten en de gebruiker de mogelijkheid geven op een andere manier om te gaan met het te besturen object.

In de praktijk werd dit een glove die een 3D camera kan besturen en optioneel ook andere sensoren kan hebben die impact hebben op de visual. Hiervoor was een draadloze microcontroller vereist, een accelerator, een mogelijkheid tot zoom -een flex resistor of lichtsensor- en andere input -hier een pulsesensor-.

De glove communiceert via wifi met de backend, die op zijn beurt OSC-signalen uitstuurt naar de visual.

##Server-side
De backend is een Node.js applicatie met een express server, deze verzorgt de socket-connectie met de controller -initieel, later MQTT- en de client, waarin een dashboard draait. Daarnaast luister hij ook op verschillende MQTT kanalen, langswaar de input van de controller wordt doorgegeven. 

Om de input van de controller ook rechtstreeks beschikbaar te maken als OSC-messages, stuurt hij elke (vooraf gedefinieerde) inkomende socket of MQTT message meteen door over UDP. 

De express-app servet ook de client applicatie. Samen draaien ze op een Heroku instantie.

##Client
De client is een React app met een dashboard, hierin kunnen de verschillende inkomende signalen gecontroleerd worden. Om dit visueel weer te geven maakte ik gebruik van d3.js. Ik creerde 2 soorten grafieken, een donut-chart om ranges weer te geven en een bar-chart zodat de verschillende dimensies van de accelerometer op 1 chart zichtbaar zijn. 

Om te luisteren naar nieuwe input van de backend maakte ik een connectie-component. Dit is een EventEmitter-class, die bij elke inkomende message een event triggered. Afhankelijke van het type wordt zo de state van de React app correct geupdate.

Zolang gebruikers client open hebben staan in hun browser kunnen ze OSC-messages ontvangen.

##Glove

###connectie
De eigenlijke controller is een ESP8266 microcontroller. Initieel koos ik deze omwille van zijn kleine vormfactor en prijs. Allereerst probeerde ik hierop een socket-connectie op te zetten. Zo zou alles op dezelfde technologie draaien en was er een rechtstreekse connectie met de backend, zonder al teveel lag. Maar hoewel ik op andere toestellen perfect kon connecteren lukte dit niet met de ESP. Na veel zoeken, en het gebruiken van verschillende servers, bleek dit een issue te zijn met bepaalde poorten. Ik probeerde dit op te lossen in de nginx-settings van mijn server (toen Digital Ocean, later opnieuw Heroku). Maar hiervoor ontbrak me de kennis.

Optie 2: van het MQTT protocol wist ik wel dat dit vlot functioneerde op de ESP. Dit kon ik dan ook zonder veel problemen implementeren. Dat ging echter wel ten kosten van de snelheid.

###accelerometer