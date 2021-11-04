import { useState } from 'react';
import './App.scss';
import './global.scss';

import { FiDownload } from 'react-icons/fi';

import Button from './components/Button';
import ModalApp from './components/ModalApp';
import SwitchButton from './components/SwitchButton';
import NewDeviceFound from './components/NewDeviceFound';
import NewDeviceAdd from './components/NewDeviceAdd';

function App() {
  const [modalIsOpen, setModalIsOpen] = useState(true);
  const [newDeviceFound, setNewDeviceFound] = useState(true);
  const [alarm, setAlarm] = useState(false);
  const [checked, setChecked] = useState(false);

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
          </div>
          <Button
            title="Download logs"
            icon={<FiDownload color="#fff" size={20} />}
          />
        </section>
      </header>
    </div>
  );
}

export default App;
