## Index

- [Hardware Environment](#hardwareEnvironment)
- [Software Environment](#softwareEnvironment)
- [Run](#run)
- [Test packet capture file](#Testpacketcapturefile)
- [Code review](#codereview)


<a name="hardwareEnvironment"></a>
### Hardware Environment

- W5100S with Nucleo f103rb
  - connecting Micro USB to Nucleo f103rb
  - connecting Ethernet Cable to W5100S_ETH_SHIELD

![W5100S_PIN](https://github.com/min-hs/W5100S_MQTT/blob/main/image/W5100S_PIN.jpg)

![W5100S_PINOUT](https://github.com/min-hs/W5100S_MQTT/blob/main/image/W5100S_PINOUT.jpg)

<a name="softwareEnvironment"></a>
### software Environment

- Device Setting Program : STM32CubeIDE 1.6.1

- Compile Program : ST-LINK

- compile method

  - Git-hub source file download
  - workspace_1.6.1 -> W5100S MQTT in folder, run `.project` file
  - workspace path select

  ![IDE_Luncher](https://github.com/min-hs/W5100S_MQTT/blob/main/image/IDE_Luncher.jpg)

  - Open main.c file and build 
  - Debug start
  - If you have ST-LINK, Run Debug

![IDE_excute](https://github.com/min-hs/W5100S_MQTT/blob/main/image/IDE_excute.jpg)

![IDE_Debug](https://github.com/min-hs/W5100S_MQTT/blob/main/image/IDE_Debug.jpg)

<a name="run"></a>
### Run

- Demo Environment & Program

  - Window 10
  - Hercules

- Demo Result

  - First, `mosquitto.conf` modify

  +Add 

  `bind_address 192.168.xxx.xxx`

  `allow_anonymous true`

  ![mqtt_conf](https://github.com/min-hs/W5100S_MQTT/blob/main/image/mqtt_conf.jpg)

  - mosquitto excute -Port 1883

  `mosquitto -c mosquitto.conf -p 1883 -v`

  ![mqtt_broker](https://github.com/min-hs/W5100S_MQTT/blob/main/image/mqtt_broker.jpg)

  

  - W5100S MQTT_sub run

  ![mqtt_sub](https://github.com/min-hs/W5100S_MQTT/blob/main/image/mqtt_sub.jpg)

  ![mqtt_sub_connect](https://github.com/min-hs/W5100S_MQTT/blob/main/image/mqtt_sub_connect.jpg)

  - MQTT_pub send message

  `mosquitto_pub -h 192.168.xxx.xxx -t W5x00 -m "hello W5100S"`

  ![mqtt_pub_sendmsg](https://github.com/min-hs/W5100S_MQTT/blob/main/image/mqtt_pub_sendmsg.jpg)

<a name="Testpacketcapturefile"></a>
### Test packet capture file

![Capture Test](https://github.com/min-hs/W5100S_MQTT/blob/main/image/mqtt_capture.jpg)
[w5100S_MQTT_cap.zip](https://github.com/min-hs/W5100S_MQTT/files/6890306/w5100S_MQTT_cap.zip)

<a name="Codereview"></a>
### Code review
- main.c code flow

![Loopback_codeflow](https://github.com/min-hs/W5100S_MQTT/blob/main/image/Mqtt_codeflow.jpg)
