import './styles.scss';

import { FiX } from 'react-icons/fi';

import SwitchButton from '../SwitchButton';

interface Props {
  closeModal(): void;
  checked: boolean;
  onChange(): void;
}

const NewDeviceAdd = ({ closeModal, checked, onChange }: Props) => {
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
          <input />
        </div>
        <div className="input-group">
          <h3>Nome da entrada</h3>
          <input />
        </div>
        <div className="input-group">
          <h3>Nome da saída</h3>
          <input />
        </div>
        <div className="input-group">
          <h3 className="switch-alarm">Ativará o alarme?</h3>
          <SwitchButton checked={checked} onChange={onChange} />
        </div>
      </section>

      <button>ADICIONAR</button>
    </div>
  );
};

export default NewDeviceAdd;
