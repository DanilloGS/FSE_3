import './styles.scss';

interface Props {
  closeModal(): void;
  addDevice(): void;
}

const NewDeviceFound = ({ closeModal, addDevice }: Props) => {
  return (
    <div id="new-device-found">
      <h2>Dispositivo encontrado!</h2>

      <div className="info-box">
        <h3>Modo:</h3>
        <p>energia</p>
      </div>
      <div className="info-box">
        <h3>MAC:</h3>
        <p>84cca85d1f9c</p>
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
