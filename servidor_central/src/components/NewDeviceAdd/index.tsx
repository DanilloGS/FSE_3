import { useState } from 'react';
import './styles.scss';

import { FiX } from 'react-icons/fi';

import SwitchButton from '../SwitchButton';

interface DeviceProps {
  mac?: string;
  type?: string;
  room?: string;
  input?: string;
  output?: string;
  state?: number;
  temperature?: number;
  humidity?: number;
  lastUpdate?: string;
}

interface Props {
  submitDevice(device: DeviceProps): void;
  closeModal(): void;
  checked: boolean;
  onChange(): void;
}

const NewDeviceAdd = ({
  submitDevice,
  closeModal,
  checked,
  onChange,
}: Props) => {
  const [room, setRoom] = useState('');
  const [input, setInput] = useState('');
  const [output, setOutput] = useState('');

  return (
    <div id="new-device-add">
      <header>
        <h2>Novo dispositivo</h2>
        <button onClick={closeModal}>
          <FiX color="#000" size={30} />
        </button>
      </header>

      <section>
        <div className="input-group">
          <h3>Cômodo do dispositivo</h3>
          <input value={room} onChange={e => setRoom(e.target.value)} />
        </div>
        <div className="input-group">
          <h3>Nome da entrada</h3>
          <input value={input} onChange={e => setInput(e.target.value)} />
        </div>
        <div className="input-group">
          <h3>Nome da saída</h3>
          <input value={output} onChange={e => setOutput(e.target.value)} />
        </div>
        <div className="input-group">
          <h3 className="switch-alarm">Ativará o alarme?</h3>
          <SwitchButton checked={checked} onChange={onChange} />
        </div>
      </section>

      <button onClick={() => submitDevice({ room, input, output })}>
        ADICIONAR
      </button>
    </div>
  );
};

export default NewDeviceAdd;
