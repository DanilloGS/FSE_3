import './styles.scss';

import { FiTrash2, FiPower } from 'react-icons/fi';

interface Props {
  mac?: string;
  type?: string;
  room?: string;
  input?: string;
  output?: string;
  state?: number;
  temperature?: number;
  humidity?: number;
  lastUpdate?: string;
  removeDevice(): void;
  toogleDevice(): void;
}

const DeviceCard = ({
  room,
  mac,
  type,
  input,
  output,
  state,
  temperature,
  humidity,
  lastUpdate,
  toogleDevice,
  removeDevice,
}: Props) => {
  return (
    <div id="device-card">
      <div className="info-group">
        <h2>{room} - </h2>
        <p>MAC: {mac}</p>
      </div>

      <div className="info-group">
        <h3>Modo:</h3>
        <p>{type}</p>
      </div>

      <div className="info-group">
        <h3>Entrada:</h3>
        <p>{input}</p>
      </div>

      <div className="info-group">
        <h3>Saída:</h3>
        <p>
          {output} -{' '}
          {state ? (
            <span className="state-on">ON</span>
          ) : (
            <span className="state-off">OFF</span>
          )}
        </p>

        {type === "energia" ? (
          <>
            <h3 className="temp-and-humi">Temperatura:</h3>
            <p>{temperature} ºC</p>

            <h3 className="temp-and-humi">Umidade:</h3>
            <p>{humidity} %</p>
          </>
        ) : null}
      </div>

      <footer>
        <div className="info-group">
          <h3>Última atualização:</h3>
          <p>{lastUpdate}</p>
        </div>

        <aside>
          <button onClick={toogleDevice}>
            <FiPower color="#22ac57" size={26} />
            <h3 className="on-off-device">DESLIGAR DISPOSITIVO</h3>
          </button>
          <button onClick={removeDevice}>
            <FiTrash2 color="#a32929" size={26} />
            <h3>REMOVER DISPOSITIVO</h3>
          </button>
        </aside>
      </footer>
    </div>
  );
};

export default DeviceCard;
