/* eslint-disable react-hooks/exhaustive-deps */
import { useState, useEffect } from 'react';
import './App.scss';
import './global.scss';

import mqtt from 'mqtt';

import ModalApp from './components/ModalApp';
import SwitchButton from './components/SwitchButton';
import NewDeviceFound from './components/NewDeviceFound';
import NewDeviceAdd from './components/NewDeviceAdd';
import DeviceCard from './components/DeviceCard';
import Csv from './components/Csv';
import Alarm from './components/Alarm';
import WelcomeModal from './components/WelcomeModal';
import { mqttConnect, mqttSub, mqttPublish, options } from './utils/mqtt';

interface DeviceProps {
  mac?: string;
  type?: string;
  room?: string;
  input?: string;
  output?: string;
  state?: number;
  temperature?: number;
  humidity?: number;
  isAlarm?: number;
  lastUpdate?: string;
}

interface LogsProps {
  date?: string;
  device?: string;
  event?: string;
}

function App() {
  const [client, setClient] = useState<mqtt.MqttClient>();
  const [connectStatus, setConnectStatus] = useState('');
  const [modalIsOpen, setModalIsOpen] = useState(false);
  const [welcomeModalIsOpen, setWelcomeModalIsOpen] = useState(true);
  const [newDeviceFound, setNewDeviceFound] = useState(false);
  const [isAlarmDevice, setIsAlarmDevice] = useState(false);
  const [activeAlarm, setActiveAlarm] = useState(false);
  const [playingAlarm, setPlayingAlarm] = useState(false);
  const [deviceInfo, setDeviceInfo] = useState<DeviceProps>();
  const [devices, setDevices] = useState<DeviceProps[]>([]);
  const [newDeviceHost] = useState('fse2021/170139981/dispositivos/+');
  const [logs, setLogs] = useState<LogsProps[]>([]);

  const broker = {
    host: 'broker.mqttdashboard.com',
    port: '8000',
  };

  const sub = {
    topic: '',
    qos: 2,
  };

  const registerDevice = (mac: string, type: string) => {
    let object: DeviceProps = { mac, type };
    if (object !== undefined) {
      setDeviceInfo(object);
      const host = newDeviceHost.replace('+', mac!);
      sub.topic = host;
      mqttSub(sub, client!);
      setModalIsOpen(true);
      setNewDeviceFound(true);
    }
  };

  const includeDevice = (device: DeviceProps) => {
    device.mac = deviceInfo?.mac;
    device.type = deviceInfo?.type;
    device.state = 0;
    device.isAlarm = isAlarmDevice === true ? 1 : 0;
    device.temperature = -1;
    device.humidity = -1;
    device.lastUpdate = new Date().toLocaleString();
    devices.push(device);
    setDevices(devices);
    const host = newDeviceHost.replace('+', device?.mac!);
    const data = {
      json_type: 10,
      device,
    };
    const context = {
      topic: host,
      qos: 2,
      payload: JSON.stringify(data),
    };
    mqttPublish(context, client!);
    sub.topic = host;
    mqttSub(sub, client!);
    setModalIsOpen(false);
    setNewDeviceFound(false);

    addLogs(device, "register");
  };

  const removeDevice = (mac: string) => {
    let device: DeviceProps = devices?.find(
      (item: DeviceProps) => item.mac === mac
    )!;

    let oldDevices = [...devices];

    let index = oldDevices.indexOf(device);
    if (index !== -1) {
      oldDevices.splice(index, 1);
    }
    setDevices(oldDevices);

    const data = {
      json_type: 11,
      device,
    };

    const host = newDeviceHost.replace('+', mac!);
    const context = {
      topic: host,
      qos: 2,
      payload: JSON.stringify(data),
    };
    mqttPublish(context, client!);
    addLogs(device, "remove");
  };

  const updatedDevice = (mac: string, type: string, payloadMessage: string) => {
    const res = JSON.parse(payloadMessage);

    let device: DeviceProps = devices.find(
      (item: DeviceProps) => item.mac === mac
    )!;

    if (device !== undefined || device >= 0) {
      if (device.type === 'energia') {
        device.temperature = res.temperature;
        device.humidity = res.humidity;
      }
      device.mac = mac;
      device.state = res.state;
      device.lastUpdate = new Date().toLocaleString();

      if (device.isAlarm && device.state && activeAlarm) {
        setPlayingAlarm(true);
      } else {
        setPlayingAlarm(false);
      }

      const oldDevice = devices;
      let indexDevice = devices?.findIndex((x) => x.mac === mac);
      setDevices([]);
      oldDevice![indexDevice!] = device;
      setDevices(oldDevice);
      addLogs(device, "update");
    }
  };

  const toggleDevice = (mac: string) => {
    let device: DeviceProps = devices?.find(
      (item: DeviceProps) => item.mac === mac
    )!;
    const host = newDeviceHost.replace('+', device.mac!);
    device.state = device.state === 1 ? 0 : 1;
    const data = {
      json_type: 12,
      device,
    };
    const context = {
      topic: host,
      qos: 2,
      payload: JSON.stringify(data),
    };
    let oldDevice = devices;
    let indexDevice = devices?.findIndex((x) => x.mac === device.mac);
    oldDevice![indexDevice!] = device;
    setDevices(oldDevice);
    mqttPublish(context, client!);
    console.log('==== Device: ', device);
  };

  useEffect(() => {
    const messageReceived = (payload: any) => {
      const res = JSON.parse(payload.message);
      const { json_type, type, mac } = res;

      let mac_device: string;

      switch (json_type) {
        case 0:
          // CADASTRAR ESP32 - 0 recebe | 10 - envia
          // {"json_type": 0, "type": "bateria", "mac": "946A7727FC11"}
          mac_device = mac;
          registerDevice(mac_device, type);
          break;

        case 1:
          // REMOVER ESP32 - 1 recebe | 11 - envia
          mac_device = payload.topic.slice(31, payload.topic.length);
          removeDevice(mac_device);
          break;

        case 2:
          // ATUALIZAR DADOS - 2 recebe | 12 envia
          // {"json_type": 12, "temperature": 40, "humidity": 77}
          // {json_type: 12, state: 1}
          mac_device = payload.topic.slice(31, payload.topic.length);
          updatedDevice(mac_device, type, payload.message);
          break;

        case 3:
          mac_device = payload.topic.slice(31, payload.topic.length);
          updatedDevice(mac_device, type, payload.message);
          break;

        default:
          break;
      }
    };

    if (client) {
      client.on('connect', () => {
        setConnectStatus('Connected');
      });
      client.on('error', (err) => {
        console.error('Connection error: ', err);
        client.end();
      });
      client.on('reconnect', () => {
        setConnectStatus('Reconnecting');
      });
      client.on('message', (topic, message) => {
        const payload = { topic, message: message.toString() };
        messageReceived(payload);
      });
    }
  }, [client]);

  useEffect(() => {}, [devices]);

  useEffect(() => {
    function init() {
      const topic = newDeviceHost.replace('+', '');
      sub.topic = topic;
      mqttSub(sub, client!);
    }

    init();
  }, [client]);

  function addLogs(device: DeviceProps, event: string) {
    const date = new Date().toLocaleString();
    let state: string = '';

    if (event === 'register') {
      state = 'Cadastro de Dispositivo | MAC: ' + device.mac;
    } else if (event === 'remove') {
      state = 'Descadastro de Dispositivo | MAC: ' + device.mac;
    } else {
      if (
        device.input?.search(/temperatura/i) ||
        device.input?.search(/termometro/i)
      ) {
        state = 'T: ' + device.temperature + '; U: ' + device.humidity;
      } else {
        state = device.state === 0 ? 'OFF' : 'ON';
      }
    }

    const data = {
      date: date,
      device: device.input,
      event: state,
    };

    let newLogs = logs;

    setLogs([]);
    newLogs.push(data);
    setLogs(newLogs);
  }

  function closeModal() {
    setModalIsOpen(false);
    setNewDeviceFound(false);
  }

  function initConnection() {
    mqttConnect(
      `ws://${broker.host}:${broker.port}/mqtt`,
      options,
      setClient,
      mqtt
    );
    setDevices([]);
    setWelcomeModalIsOpen(false);
  }

  function addDevice() {
    setNewDeviceFound(false);
  }

  return (
    <div id="home">
      <ModalApp
        modalIsOpen={modalIsOpen || newDeviceFound}
        closeModal={() => closeModal()}
      >
        {newDeviceFound ? (
          <NewDeviceFound
            mode={deviceInfo?.type!}
            mac={deviceInfo?.mac!}
            closeModal={() => closeModal()}
            addDevice={() => addDevice()}
          />
        ) : (
          <NewDeviceAdd
            submitDevice={includeDevice}
            closeModal={() => closeModal()}
            checked={isAlarmDevice}
            onChange={() => setIsAlarmDevice(!isAlarmDevice)}
          />
        )}
      </ModalApp>
      <WelcomeModal
        modalIsOpen={welcomeModalIsOpen}
        onClick={() => initConnection()}
      />
      <header>
        <h1>Central de Controle</h1>

        <section>
          <div>
            <h3>Ativar alarme</h3>
            <SwitchButton
              checked={activeAlarm}
              onChange={() => {
                if (!activeAlarm) {
                  setActiveAlarm(true);
                } else {
                  setActiveAlarm(false);
                  setPlayingAlarm(false);
                }
              }}
            />
            {playingAlarm && <Alarm />}
          </div>
          <Csv data={logs} />
        </section>
      </header>

      <main>
        {devices.map((item) => (
          <DeviceCard
            key={item.mac}
            room={item.room!}
            mac={item.mac!}
            type={item.type!}
            input={item.input!}
            output={item.output!}
            state={item.state}
            temperature={item.temperature!}
            humidity={item.humidity!}
            lastUpdate={item.lastUpdate!}
            removeDevice={() => removeDevice(item.mac!)}
            toogleDevice={() => toggleDevice(item.mac!)}
          />
        ))}
      </main>
    </div>
  );
}

export default App;
