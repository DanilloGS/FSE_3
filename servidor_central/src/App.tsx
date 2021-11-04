import { useState } from 'react';
import './App.scss';
import './global.scss';

import ModalApp from './components/ModalApp';
import SwitchButton from './components/SwitchButton';
import NewDeviceFound from './components/NewDeviceFound';
import NewDeviceAdd from './components/NewDeviceAdd';
import DeviceCard from './components/DeviceCard';
import Csv from './components/Csv';
// import Alarm from './components/Alarm';

function App() {
  const [modalIsOpen, setModalIsOpen] = useState(true);
  const [newDeviceFound, setNewDeviceFound] = useState(true);
  const [alarm, setAlarm] = useState(false);
  const [checked, setChecked] = useState(false);

  const mockData = [
    { date: '04/11/2021 22:54:17', device: 'Ar-condicionado', event: 'ON' },
    { date: '27/10/2021 03:14:54', device: 'Sensor de presença', event: 'ON' },
    { date: '02/11/2021 19:31:15', device: 'Sensor de fumaça', event: 'OFF' },
  ];

  function closeModal() {
    setModalIsOpen(false);
    setNewDeviceFound(false);
  }

  function addDevice() {
    setNewDeviceFound(!newDeviceFound);
  }

  return (
    <div id="home">
      <ModalApp
        modalIsOpen={modalIsOpen || newDeviceFound}
        closeModal={() => closeModal()}
      >
        {newDeviceFound ? (
          <NewDeviceFound
            closeModal={() => closeModal()}
            addDevice={() => addDevice()}
          />
        ) : (
          <NewDeviceAdd
            closeModal={() => closeModal()}
            checked={alarm}
            onChange={() => setAlarm(!alarm)}
          />
        )}
      </ModalApp>
      <header>
        <h1>Central de Controle</h1>

        <section>
          <div>
            <h3>Ativar alarme</h3>
            <SwitchButton
              checked={checked}
              onChange={() => setChecked(!checked)}
            />
            {/* {checked && <Alarm />} */}
          </div>
          <Csv data={mockData} />
        </section>
      </header>

      <main>
        <DeviceCard
          mac="84cca85d1f9c"
          mode="energia"
          input="Sensor de Temperatura"
          output="Ar-condicionado"
          temperature="30"
          humidity="70"
          lastUpdate="04/11/2021 22:54:17"
        />
      </main>
    </div>
  );
}

export default App;
