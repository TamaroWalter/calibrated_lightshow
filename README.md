## Calibrated light show project

### Ziele

Bewegungen mithilfe einer IMU erkennen und mit Licht zu visualisieren

### Aufbau

Es gibt insgesamt 4 Mikrocontroller.

- 1 Mikrocontroller steuert mithilfe von WLED die LED-Matrix an. Dieser Mikrocontroller wird nicht verändert. Er hat die WLED-Software installiert und hört den MQTT server gruppe2/api zu.
- 2 Mikrocontroller steuern die IMU, lesen deren Daten aus und publishen auf MQTT das Kommando, welches ausgeführt werden soll
- 1 Mikrocontroller ist dafür da, auf gruppe2/api eine JSON zu publishen, dass die richtigen Kommandos für die led besitzt. Es besitzt ein Dictionary aus Kommando -> json. Später ist das
    die Hauptsteuereinheit, welches das Kommando der IMU liest und dann abhängig davon die richtige JSON an die LED matrix schickt.


### Links

[WLED json-api](https://kno.wled.ge/interfaces/json-api/#setting-new-values)
### Meilensteine

- Zuerst die lichter ansprechen
- Dann mit der IMU Bewegungen erkennen
- Beides verknüpfen
- Alles von einem Roboterarm machen lassen

### JSON
## Fragmente
```
{
  "on": true,
  "bri": 128,
  "seg": [
    {
      "fx": 183,
      "sx": 128,
      "ix": 128,
      "col": [[255, 0, 0]]
    }
  ]
}
```
Notiz: bri brightness, fx id effect, sx speed, ix intensity, col primary color
