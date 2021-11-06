import './styles.scss';

interface Props {
  closeModal(): void;
  addDevice(): void;
  mode: string;
  mac: string;
}

const NewDeviceFound = ({ closeModal, addDevice, mode, mac }: Props) => {
  return (
    <div id="new-device-found">
      <h2>Dispositivo encontrado!</h2>

      <div className="info-box">
        <h3>Modo:</h3>
        <p>{mode}</p>
      </div>
      <div className="info-box">
        <h3>MAC:</h3>
        <p>{mac}</p>
      </div>

      <footer>
        <button className="refuse" onClick={closeModal}>
          RECUSAR
        </button>
        <button onClick={addDevice}>ADICIONAR</button>
      </footer>
    </div>
  );
};

export default NewDeviceFound;
