# Lulu
Lulu is an ESP8266 powered "bridge" for connecting a MindFlex EEG headset with [Princess Luna](https://github.com/Thorinair/Princess-Luna) and VRChat. The device is battery powered and meant to be kept in a pocket while it reads serial data streamed from the MindFlex headset. This data is then translated to a more easily readable format and sent to Princess Luna for further processing, logging, graphing, etc. Attention values are also streamed as OSC data to VRChat for controlling an avatar.

## Requirements
* [Arduino Brain Library](https://github.com/kitschpatrol/Brain)
* [OSC Library For ESP8266](https://github.com/stahlnow/OSCLib-for-ESP8266)