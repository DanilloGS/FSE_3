import './styles.scss';

import { FiTrash2, FiPower } from 'react-icons/fi';

interface Props {
  mac: string;
  mode: string;
  input?: string;
  output?: string;
  temperature?: string;
  humidity?: string;
  lastUpdate: string;
}

const DeviceCard = ({
  mac,
  mode,
  input,
  output,
  temperature,
  humidity,
  lastUpdate,
}: Props) => {
  return (
    <div id="device-card">
      <div className="info-group">
        <h2>Quarto - </h2>
        <p>MAC: {mac}</p>
      </div>

      <div className="info-group">
        <h3>Modo:</h3>
        <p>{mode}</p>
      </div>

      <div className="info-group">
        <h3>Entrada:</h3>
        <p>
          {input} - <span>ON</span>
        </p>
      </div>

      <div className="info-group">
        <h3>Saída:</h3>
        <p>
          {output} - <span>ON</span>
        </p>

        <h3 className="temp-and-humi">Temperatura:</h3>
        <p>{temperature} ºC</p>

        <h3 className="temp-and-humi">Umidade:</h3>
        <p>{humidity} %</p>
      </div>

      <footer>
        <div className="info-group">
          <h3>Última atualização:</h3>
          <p>{lastUpdate}</p>
        </div>

        <aside>
          <button>
            <FiPower color="#22ac57" size={26} />
            <h3 className="on-off-device">DESLIGAR DISPOSITIVO</h3>
          </button>
          <button>
            <FiTrash2 color="#a32929" size={26} />
            <h3>REMOVER DISPOSITIVO</h3>
          </button>
        </aside>
      </footer>
    </div>
  );
};

export default DeviceCard;
